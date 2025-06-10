#ifndef IDLESTATE_H
#define IDLESTATE_H

#include "state/State.h"
#include "states/StateIds.h" // Include the enum definition

template<typename StateIdType>
class IdleState : public State<StateIdType> { // Inherit from State<StateIdType>
public:
    IdleState() = default;

    void handle() override;

    StateIdType getStateId() const override {
        return StateIdType::Idle;
    }
};

#endif // IDLESTATE_H