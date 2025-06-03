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
