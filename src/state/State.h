// FILE: State.h

#ifndef STATE_H
#define STATE_H

#include <any>

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

    void setStateTask(std::any task) {
        stateTask_ = task;
    }

    void setStateMachine(StateMachine<StateIdType>* machine) {
        machine_ = machine;
    }

    std::any getStateTask() const {
        return stateTask_;
    }

protected:
    StateMachine<StateIdType>* machine_ = nullptr;
    std::any stateTask_; // Placeholder for any job or task related to this state

};

#endif // STATE_H