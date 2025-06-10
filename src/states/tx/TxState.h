#ifndef IDLESTATE_H
#define IDLESTATE_H

#include "state/State.h"

class IdleState : public State {
public:
    IdleState() = default;

    void handle() override;

    CommState getStateId() const override {
        return CommState::Idle;
    }

    
};

#endif // IDLESTATE_H
