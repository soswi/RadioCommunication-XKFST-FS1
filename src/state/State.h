#ifndef STATE_H
#define STATE_H

#include "CommState.h"

class StateMachine;

class State {
public:
    // Virtual destructor
    virtual ~State() = default;

    // Pure virtual method to handle state logic
    virtual void handle() = 0;

    // Each state must identify itself by returning its CommState enum
    virtual CommState getStateId() const = 0;

protected:
    StateMachine* machine_ = nullptr;

public:
    // Setter for StateMachine pointer (to allow transitions)
    void setStateMachine(StateMachine* machine) {
        machine_ = machine;
    }
};

#endif // STATE_H
