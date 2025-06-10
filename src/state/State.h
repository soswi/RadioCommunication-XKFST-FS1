// FILE: State.h

#ifndef STATE_H
#define STATE_H

// Forward declare the StateMachine template
template <typename StateIdType>
class StateMachine;

template <typename StateIdType>
class State {
public:
    virtual ~State() = default;
    virtual void handle() = 0;

    // This is now a regular virtual function that returns the template type.
    // Each state machine instance will have its own specific IdType.
    virtual StateIdType getStateId() const = 0;

    void setStateMachine(StateMachine<StateIdType>* machine) {
        machine_ = machine;
    }

protected:
    StateMachine<StateIdType>* machine_ = nullptr;
};

#endif // STATE_H