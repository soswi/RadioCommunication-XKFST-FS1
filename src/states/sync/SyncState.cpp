#include "SyncState.h"
#include "states/StateIds.h"
#include "state/StateMachine.h"
#include <Arduino.h>
#include <any>
#include "esp_timer.h" // Required for hardware timers

// ============================================================================
// Protocol & Timing Constants
// ============================================================================
const int TX_PIN = 5; // GPIO for the transmitter data line.
const int RX_PIN = 4; // GPIO for the receiver data line.
const int LED_BUILTIN = 8; // Onboard LED for the ESP32-C3.

// Handshake pulse durations (in microseconds).
// Defines the valid time windows for the handshake signals.
const unsigned long INITIATION_PULSE_MIN_US = 15000;
const unsigned long INITIATION_PULSE_MAX_US = 20000;
const unsigned long CONFIRMATION_PULSE_MIN_US = 20000;
const unsigned long CONFIRMATION_PULSE_MAX_US = 25000;

const unsigned int PREAMBLE_PULSE_COUNT = 20; // Number of pulses for clock discovery. More pulses = better average but slower sync.
const unsigned long PULSE_TIMEOUT_US = 50000; // Max wait time for a single pulse edge. Prevents infinite blocking.

// Global variable to share the measured pulse width between receiver and the final synced state.
// NOTE: This is not thread-safe but acceptable here as sync protocol is modal.
unsigned long discoveredPulseWidth = 0;

// Forward declaration of the ISR function from the main .ino file
// This is needed to re-attach the interrupt later.
void IRAM_ATTR handleRadioPulse();


// ============================================================================
// Sub-state definitions
// ============================================================================

// --- Shared Final States ---

// Sub-state for when the sub-machine has no active task.
template<typename SubStateIdType>
class IdleSyncSubState : public State<SubStateIdType> {
public:
    void handle() override { /* NOP, consumes no CPU cycles until a new state is set. */ }
    SubStateIdType getStateId() const override { return SubStateIdType::Idle; }
};

// Sub-state for handling synchronization failures.
template<typename SubStateIdType>
class TimeoutSyncSubState : public State<SubStateIdType> {
public:
    void handle() override {
        // Log the failure and transition the sub-FSM to Idle.
        // The parent SyncState will detect this and exit the sync process.
        Serial.println("  Sub-State: TIMEOUT! Synchronization failed.");
        this->machine_->setState(SubStateIdType::Idle);
    }
    SubStateIdType getStateId() const override { return SubStateIdType::Timeout; }
};

/**
 * Final sub-state. Uses a non-blocking hardware timer for a precise,
 * synchronized action (LED blink) on both devices.
 */
template<typename SubStateIdType>
class SyncedSyncSubState : public State<SubStateIdType> {
private:
    esp_timer_handle_t syncTimer; // Handle for the high-resolution hardware timer.
    bool timerStarted = false;    // Flag to ensure the timer is started only once per entry.
    volatile static bool timerFired; // Flag set by the timer ISR to signal completion. Must be volatile.

    /**
     * @brief One-shot timer ISR.
     * Executes the final synchronized action. Kept minimal and fast.
     */
    static void IRAM_ATTR onSyncTimer(void* arg) {
        // This code runs in an interrupt context.
        digitalWrite(LED_BUILTIN, HIGH);
        delayMicroseconds(500); // Short, blocking delay is acceptable within a one-shot ISR.
        digitalWrite(LED_BUILTIN, LOW);
        timerFired = true; // Signal the main loop that the action is complete.
    }

public:
    SyncedSyncSubState() {
        // Set up the timer but don't start it yet.
        const esp_timer_create_args_t timer_args = {
            .callback = &onSyncTimer,
            .name = "sync_timer"
        };
        esp_timer_create(&timer_args, &syncTimer);
        timerFired = false;
    }

    ~SyncedSyncSubState() {
        // Clean up the timer resource.
        esp_timer_delete(syncTimer);
    }

    void handle() override {
        // The handle() method now works as a non-blocking poller.
        if (!timerStarted) {
            // On first entry, start the one-shot timer.
            Serial.println("  Sub-State: SYNCHRONIZED! Starting final timed event.");
            esp_timer_start_once(syncTimer, 5 * discoveredPulseWidth);
            timerStarted = true;
            timerFired = false;
        }

        // Poll the flag set by the ISR.
        if (timerFired) {
            Serial.println("------------------------------------");
            
            // Reset state for the next run.
            timerStarted = false;
            timerFired = false;

            // Job done, transition sub-FSM to Idle.
            this->machine_->setState(SubStateIdType::Idle);
        }
        // If the timer is running, do nothing and let the main loop continue.
    }

    SubStateIdType getStateId() const override { return SubStateIdType::Synced; }
};

// Static member initialization.
template<typename SubStateIdType>
volatile bool SyncedSyncSubState<SubStateIdType>::timerFired = false;


// --- INITIATOR (Transmitter) Path States ---

// Sends the initial long pulse to wake up any listeners.
template<typename SubStateIdType>
class Initiate_SendInitialPulse : public State<SubStateIdType> {
public:
    void handle() override {
        digitalWrite(TX_PIN, HIGH);
        delayMicroseconds(17500); // 17.5ms pulse
        digitalWrite(TX_PIN, LOW);
        this->machine_->setState(SubStateIdType::Initiate_SendPreamble);
    }
    SubStateIdType getStateId() const override { return SubStateIdType::Initiate_SendInitialPulse; }
};

// Sends a burst of known-width pulses for the receiver to measure.
template<typename SubStateIdType>
class Initiate_SendPreamble : public State<SubStateIdType> {
public:
    void handle() override {
        // Transmit a fixed-rate preamble. The receiver will measure this
        // to determine the effective bit rate.
        for (unsigned int i = 0; i < PREAMBLE_PULSE_COUNT; ++i) {
            digitalWrite(TX_PIN, HIGH);
            delayMicroseconds(500);
            digitalWrite(TX_PIN, LOW);
            delayMicroseconds(500);
        }
        this->machine_->setState(SubStateIdType::Initiate_WaitForConfirmation);
    }
    SubStateIdType getStateId() const override { return SubStateIdType::Initiate_SendPreamble; }
};

// Waits for the receiver's confirmation pulse.
template<typename SubStateIdType>
class Initiate_WaitForConfirmation : public State<SubStateIdType> {
public:
    void handle() override {
        // Block and wait for a pulse within the expected time window.
        unsigned long duration = pulseIn(RX_PIN, HIGH, 500000);
        if (duration >= CONFIRMATION_PULSE_MIN_US && duration <= CONFIRMATION_PULSE_MAX_US) {
            this->machine_->setState(SubStateIdType::Initiate_SendFinalTrigger);
        } else {
            this->machine_->setState(SubStateIdType::Timeout);
        }
    }
    SubStateIdType getStateId() const override { return SubStateIdType::Initiate_WaitForConfirmation; }
};

/**
 * @brief Sends the final, short pulse that triggers the synchronized action
 * using a non-blocking hardware timer.
 */
template<typename SubStateIdType>
class Initiate_SendFinalTrigger : public State<SubStateIdType> {
private:
    esp_timer_handle_t triggerTimer;
    bool timerStarted = false;
    volatile static bool pulseSent;

    static void IRAM_ATTR onTriggerTimer(void* arg) {
        // This ISR is called after 1000us to end the pulse.
        digitalWrite(TX_PIN, LOW);
        pulseSent = true;
    }

public:
    Initiate_SendFinalTrigger() {
        const esp_timer_create_args_t timer_args = {
            .callback = &onTriggerTimer,
            .name = "trigger_timer"
        };
        esp_timer_create(&timer_args, &triggerTimer);
        pulseSent = false;
    }

    ~Initiate_SendFinalTrigger() {
        esp_timer_delete(triggerTimer);
    }
    
    void handle() override {
        if (!timerStarted) {
            Serial.println("  Sub-State: Sending final trigger pulse (non-blocking).");
            // Start the pulse
            digitalWrite(TX_PIN, HIGH);
            // Set a timer to end the pulse after 1000us
            esp_timer_start_once(triggerTimer, 1000);
            timerStarted = true;
            pulseSent = false;
        }

        if (pulseSent) {
            // Reset for next time
            timerStarted = false;
            pulseSent = false;
            // Move to the final state
            this->machine_->setState(SubStateIdType::Synced);
        }
    }
    SubStateIdType getStateId() const override { return SubStateIdType::Initiate_SendFinalTrigger; }
};
template<typename SubStateIdType>
volatile bool Initiate_SendFinalTrigger<SubStateIdType>::pulseSent = false;


// --- REQUEST (Receiver) Path States ---

// Listens for the initial long pulse from an initiator.
template<typename SubStateIdType>
class Request_WaitForInitialPulse : public State<SubStateIdType> {
public:
    void handle() override {
        unsigned long duration = pulseIn(RX_PIN, HIGH, 500000);
        if (duration >= INITIATION_PULSE_MIN_US && duration <= INITIATION_PULSE_MAX_US) {
            this->machine_->setState(SubStateIdType::Request_MeasurePreamble);
        } else {
            // Timed out, no one is initiating. Return to Idle.
            this->machine_->setState(SubStateIdType::Idle);
        }
    }
    SubStateIdType getStateId() const override { return SubStateIdType::Request_WaitForInitialPulse; }
};

// Measures the incoming preamble pulses to discover the clock rate.
template<typename SubStateIdType>
class Request_MeasurePreamble : public State<SubStateIdType> {
public:
    void handle() override {
        unsigned long totalDuration = 0;
        unsigned int measuredPulses = 0;
        for (unsigned int i = 0; i < PREAMBLE_PULSE_COUNT; ++i) {
            // Measure both high and low phases of the pulse.
            unsigned long highTime = pulseIn(RX_PIN, HIGH, PULSE_TIMEOUT_US);
            unsigned long lowTime = pulseIn(RX_PIN, LOW, PULSE_TIMEOUT_US);
            if (highTime > 0 && lowTime > 0) {
                totalDuration += highTime + lowTime;
                measuredPulses++;
            } else {
                break; // Pulse train ended or timeout occurred.
            }
        }

        // Check if enough pulses were measured for a reliable average.
        if (measuredPulses > PREAMBLE_PULSE_COUNT / 2) {
            discoveredPulseWidth = (totalDuration / measuredPulses) / 2;
            this->machine_->setState(SubStateIdType::Request_SendConfirmation);
        } else {
            this->machine_->setState(SubStateIdType::Timeout);
        }
    }
    SubStateIdType getStateId() const override { return SubStateIdType::Request_MeasurePreamble; }
};

// Sends the long confirmation pulse back to the initiator.
template<typename SubStateIdType>
class Request_SendConfirmation : public State<SubStateIdType> {
public:
    void handle() override {
        digitalWrite(TX_PIN, HIGH);
        delayMicroseconds(22500); // 22.5ms pulse.
        digitalWrite(TX_PIN, LOW);
        this->machine_->setState(SubStateIdType::Request_WaitForFinalTrigger);
    }
    SubStateIdType getStateId() const override { return SubStateIdType::Request_SendConfirmation; }
};

/**
 * @brief Listens for the final trigger pulse from the initiator
 * using a non-blocking polling and a timeout timer.
 */
template<typename SubStateIdType>
class Request_WaitForFinalTrigger : public State<SubStateIdType> {
private:
    esp_timer_handle_t timeoutTimer;
    bool timerStarted = false;
    volatile static bool timedOut;

    static void IRAM_ATTR onTimeout(void* arg) {
        timedOut = true;
    }
public:
    Request_WaitForFinalTrigger() {
        const esp_timer_create_args_t timer_args = {
            .callback = &onTimeout,
            .name = "timeout_timer"
        };
        esp_timer_create(&timer_args, &timeoutTimer);
        timedOut = false;
    }

    ~Request_WaitForFinalTrigger() {
        esp_timer_delete(timeoutTimer);
    }

    void handle() override {
        if (!timerStarted) {
            Serial.println("  Sub-State: Waiting for final trigger (non-blocking)...");
            // Start a timer for the maximum wait time.
            esp_timer_start_once(timeoutTimer, 500000); // 500ms timeout
            timerStarted = true;
            timedOut = false;
        }

        // Poll the pin for the trigger signal.
        if (digitalRead(RX_PIN) == HIGH) {
            Serial.println("  Final trigger received!");
            // Stop the timer, we don't need it anymore.
            esp_timer_stop(timeoutTimer);
            
            // Reset for next time and transition.
            timerStarted = false;
            this->machine_->setState(SubStateIdType::Synced);
            return; // Exit handle immediately.
        }

        // Check if we have timed out.
        if (timedOut) {
            timerStarted = false;
            timedOut = false;
            this->machine_->setState(SubStateIdType::Timeout);
        }
    }
    SubStateIdType getStateId() const override { return SubStateIdType::Request_WaitForFinalTrigger; }
};
template<typename SubStateIdType>
volatile bool Request_WaitForFinalTrigger<SubStateIdType>::timedOut = false;


// ============================================================================
// SyncState Main Implementation
// ============================================================================

template<typename StateIdType>
SyncState<StateIdType>::SyncState() {
    pinMode(LED_BUILTIN, OUTPUT);
    subMachine_ = new StateMachine<SyncStates>(
        SyncStates::Idle,
        // Register all sub-states with the sub-machine.
        IdleSyncSubState<SyncStates>(),
        SyncedSyncSubState<SyncStates>(),
        TimeoutSyncSubState<SyncStates>(),
        Initiate_SendInitialPulse<SyncStates>(),
        Initiate_SendPreamble<SyncStates>(),
        Initiate_WaitForConfirmation<SyncStates>(),
        Initiate_SendFinalTrigger<SyncStates>(),
        Request_WaitForInitialPulse<SyncStates>(),
        Request_MeasurePreamble<SyncStates>(),
        Request_SendConfirmation<SyncStates>(),
        Request_WaitForFinalTrigger<SyncStates>()
    );
}

template<typename StateIdType>
SyncState<StateIdType>::~SyncState() {
    delete subMachine_;
}

template<typename StateIdType>
void SyncState<StateIdType>::handle() {
    // Check for a new task passed from an ISR. This runs only once per transition.
    if (this->stateTask_.has_value()) {
        // --- CRITICAL SECTION START: Disable interrupt during sync process ---
        detachInterrupt(digitalPinToInterrupt(RX_PIN));
        Serial.println("SyncState: RX Interrupt detached.");

        try {
            auto task = std::any_cast<SyncStates>(this->stateTask_);
            // Set the initial state of the sub-machine based on the task.
            if (task == SyncStates::Initiate) {
                subMachine_->setState(SyncStates::Initiate_SendInitialPulse); 
            } else if (task == SyncStates::Request) {
                subMachine_->setState(SyncStates::Request_WaitForInitialPulse);
            }
        } catch (const std::bad_any_cast& e) {
            Serial.println("SyncState: Error while casting task payload.");
        }
        this->stateTask_.reset(); // Consume the task.
    }

    // Delegate execution to the sub-state machine.
    if (subMachine_) {
        subMachine_->update();
    }
    
    // Check if the sub-machine has completed its work (returned to Idle).
    if (subMachine_ && subMachine_->getCurrentStateId() == SyncStates::Idle) {
        // --- CRITICAL SECTION END: Re-enable interrupt after sync process ---
        attachInterrupt(digitalPinToInterrupt(RX_PIN), handleRadioPulse, CHANGE);
        Serial.println("SyncState: RX Interrupt re-attached.");

        // Transition the main FSM back to its Idle state.
        Serial.println("SyncState: Process finished. Returning to main Idle state.");
        this->machine_->setState(MasterStates::Idle);
    }
}

// Explicit template instantiation.
template class SyncState<MasterStates>;
