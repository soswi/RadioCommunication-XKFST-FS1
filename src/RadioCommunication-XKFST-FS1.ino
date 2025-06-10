#include <Arduino.h>
#include "state/StateMachine.h"
#include "states/StateIds.h"
#include "states/idle/IdleState.h"
#include "states/sync/SyncState.h"

// Let's assume you have a flag to trigger sending
bool hasDataToSend = false;

// The state machine needs a pointer to be accessed globally
// We use a pointer to allow for proper initialization in setup()
StateMachine<MasterStates>* stateMachine;

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for Serial to be available

    Serial.println("System initialized. Creating State Machine...");

    // Correctly instantiate the state machine and its states
    stateMachine = new StateMachine<MasterStates>(
        MasterStates::Idle,
        IdleState<MasterStates>(),     
        SyncState<MasterStates>()       
    );

    Serial.println("State Machine created. Initial state: Idle");
}

void loop() {
    // Simulate a trigger to start the communication
    if (millis() > 5000 && !hasDataToSend) {
        Serial.println("\n[SIMULATION] Data is now available to send. Triggering state change...");
        hasDataToSend = true;
    }

    stateMachine->update();
    delay(500); // Slow down execution for clarity
}