#include "state/StateMachine.h"
#include "state/States.h"
#include <iostream>

int main() {
    // Pass constructed states directly to StateMachine constructor
    StateMachine sm(
        IdleState{},
        SyncState{}
    );

    for (int i = 0; i < 5; ++i) {
        sm.update();
    }

    return 0;
}

DataReceiver receiver;

void setup() {
    Serial.begin(115200);
    receiver.begin();
    Serial.println("Start systemu...");
}

void loop() {
    receiver.update();
    sm.update();
    delay(100);
}
