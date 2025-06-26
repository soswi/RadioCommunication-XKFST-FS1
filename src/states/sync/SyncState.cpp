#include "SyncState.h"
#include "states/StateIds.h"
#include "state/StateMachine.h"
#include <Arduino.h>
#include <any>

// ============================================================================
// File: states/sync/substates.h (Sub-state definitions)
// ============================================================================

/**
 * @brief A sub-state where the sub-machine is idle, waiting for a task.
 */
template<typename SubStateIdType>
class IdleSyncSubState : public State<SubStateIdType> {
public:
    void handle() override { /* Wait for a task from the parent state */ }
    SubStateIdType getStateId() const override { return SubStateIdType::Idle; }
};

/**
 * @brief A sub-state for listening for a synchronization sequence.
 */
template<typename SubStateIdType>
class RequestSyncSubState : public State<SubStateIdType> {
public:
    void handle() override {
        // TODO: Logic for listening will be placed here
        Serial.println("  Sub-State: Handling Request (Listening)...");
    }
    SubStateIdType getStateId() const override { return SubStateIdType::Request; }
};

/**
 * @brief A sub-state for transmitting a synchronization sequence.
 */
template<typename SubStateIdType>
class InitiateSyncSubState : public State<SubStateIdType> {
public:
    void handle() override {
        // TODO: Logic for transmitting will be placed here
        Serial.println("  Sub-State: Handling Initiate (Transmitting)...");
    }
    SubStateIdType getStateId() const override { return SubStateIdType::Initiate; }
};


// ============================================================================
// File: states/sync/SyncState.cpp (Main implementation)
// ============================================================================

/**
 * @brief Constructor for the Sync state. Creates the nested state machine.
 */
template<typename StateIdType>
SyncState<StateIdType>::SyncState() {
    subMachine_ = new StateMachine<SyncStates>(
        SyncStates::Idle, // The sub-machine always starts in the Idle state
        IdleSyncSubState<SyncStates>(),
        RequestSyncSubState<SyncStates>(),
        InitiateSyncSubState<SyncStates>()
    );
}

/**
 * @brief The main execution handler for the Sync state.
 */
template<typename StateIdType>
void SyncState<StateIdType>::handle() {
    // STEP 1: Check if a new task has arrived (set by an interrupt).
    // This logic will execute only once per task.
    std::any stateTask_ = this->getStateTask();
    if (stateTask_.has_value()) {
        try {
            // We now use SyncStates as the task type
            SyncStates task = std::any_cast<SyncStates>(stateTask_);
            Serial.print("SyncState: Received new task -> ");

            // STEP 2: Set the appropriate state in the sub-machine.
            if (task == SyncStates::Initiate) {
                Serial.println("INITIATE");
                subMachine_->setState(SyncStates::Initiate);
            } else if (task == SyncStates::Request) {
                Serial.println("REQUEST");
                subMachine_->setState(SyncStates::Request);
            }
        } catch (const std::bad_any_cast& e) {
            Serial.println("SyncState: Error while casting task payload.");
        }

        // STEP 3: Clear the task after processing to prevent re-execution.
        stateTask_.reset();
    }

    // STEP 4: Always delegate execution to the sub-state machine.
    if (subMachine_) {
        subMachine_->update();
    }
}


// ============================================================================
// Explicit template instantiation
// ============================================================================
template class SyncState<MasterStates>;
