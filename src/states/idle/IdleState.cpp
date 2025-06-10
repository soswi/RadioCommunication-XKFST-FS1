#include "IdleState.h"
#include "states/StateIds.h" // Include the enum definition
#include "state/StateMachine.h" // Needed for state transitions
#include <Arduino.h>

// This is an explicit instantiation of the template.
template class IdleState<MasterStates>;

// This global variable is declared in the .ino file
extern bool hasDataToSend;

template<typename StateIdType>
void IdleState<StateIdType>::handle() {
    Serial.println("Handling Idle State: Checking for data...");

    // In a real scenario, you would check a message queue.
    // Here, we just check the simulation flag.
    if (hasDataToSend) {
        Serial.println("Data detected! Changing state to Sync.");
        // Use the machine pointer to change the state
        if (this->machine_) {
            this->machine_->setState(StateIdType::Sync);
        }
    }
}