#include <Arduino.h>
#include "state/StateMachine.h"
#include "states/StateIds.h"
#include "states/idle/IdleState.h"
#include "states/sync/SyncState.h"

// A flag to simulate that data has become available for sending.
bool hasDataToSend = false;

// A global pointer to the state machine instance.
// Using a pointer allows for its proper initialization within the setup() function,
// which is crucial for handling complex objects in the Arduino environment.
StateMachine<MasterStates>* stateMachine;

void setup() {
    // Initialize serial communication for debugging.
    Serial.begin(115200);
    while (!Serial); // Wait for the serial port to connect. Needed for native USB.

    Serial.println("System initialized. Creating State Machine...");

    // Dynamically allocate and instantiate the state machine with its states.
    // This is the correct way to initialize the templated StateMachine and its
    // corresponding templated State objects.
    stateMachine = new StateMachine<MasterStates>(
        MasterStates::Idle,         // The initial state of the machine.
        IdleState<MasterStates>(),  // An instance of the Idle state.
        SyncState<MasterStates>()   // An instance of the Sync state.
    );

    Serial.println("State Machine created. Initial state: Idle");
}

void loop() {
    // This block simulates an external event (e.g., data arriving in a queue)
    // that triggers the need for a state transition.
    if (millis() > 5000 && !hasDataToSend) {
        Serial.println("\n[SIMULATION] Data is now available to send. Triggering state change...");
        hasDataToSend = true;
    }

    // The main workhorse of the application.
    // This calls the handle() method of the current state.
    stateMachine->update();

    // A small delay to slow down the loop's execution, making the serial
    // output easier to read and follow during debugging.
    delay(500);
}
