#include <Arduino.h>
#include "state/StateMachine.h"
#include "states/StateIds.h"
#include "states/idle/IdleState.h"
#include "states/sync/SyncState.h"

// --- Pin Configuration ---
const int RX_PIN = 4;       // Pin for the RF receiver module
const int TX_PIN = 5;       // Pin for the RF transmitter module
const int BUTTON_PIN = 3;   // Pin for the manual trigger button

// A global pointer to the state machine instance.
StateMachine<MasterStates>* stateMachine;


/**
 * @brief Interrupt Service Routine (ISR) for the radio signal.
 * Called on any state change on the RX_PIN.
 */
void IRAM_ATTR handleRadioPulse() {
    if (stateMachine) {
        // Request the state machine to switch to Sync state with the "Listen" task.
        stateMachine->setState(MasterStates::Sync, SyncStates::Request);
    }
}

/**
 * @brief Interrupt Service Routine (ISR) for the button press.
 * Called when the button is pressed (falling edge).
 */
void IRAM_ATTR handleButtonPress() {
    if (stateMachine) {
        // Request the state machine to switch to Sync state with the "Initiate" task.
        stateMachine->setState(MasterStates::Sync, SyncStates::Initiate);
    }
}


void setup() {
    // Initialize serial communication for debugging.
    Serial.begin(115200);
    while (!Serial); // Wait for the serial port to connect. Needed for native USB.

    Serial.println("System initialized. Configuring pins and interrupts...");
    
    // Configure I/O pins
    pinMode(RX_PIN, INPUT_PULLUP);
    pinMode(TX_PIN, OUTPUT);
    digitalWrite(TX_PIN, LOW);
    pinMode(BUTTON_PIN, INPUT_PULLUP); // Configure button pin with internal pull-up

    // Dynamically allocate and instantiate the state machine with its states.
    stateMachine = new StateMachine<MasterStates>(
        MasterStates::Idle,         // The initial state of the machine.
        IdleState<MasterStates>(),  // An instance of the Idle state.
        SyncState<MasterStates>()   // An instance of the Sync state.
    );

    // Attach interrupts
    attachInterrupt(digitalPinToInterrupt(RX_PIN), handleRadioPulse, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);

    Serial.println("State Machine created. Waiting for events via interrupts...");
}

void loop() {
    // The main workhorse of the application.
    // This calls the handle() method of the current state.
    stateMachine->update();

    // The loop only needs to call update(). State transitions are
    // initiated by events (interrupts).
}
