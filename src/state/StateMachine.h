// FILE: src/state/StateMachine.h

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "State.h"
#include <memory>
#include <unordered_map>
#include <Arduino.h>

template <typename StateIdType>
class StateMachine {
public:
    template <typename... States>
    explicit StateMachine(StateIdType initialState, States&&... states);

    void setState(StateIdType newState);
    void setState(StateIdType newState, std::any newTask);

    StateIdType getCurrentStateId() const;
    StateIdType getPreviousStateId() const;

    void update();

private:
    // Finds a state pointer in the map and sets currentState_ pointer.
    void findAndSetCurrentState(StateIdType id);

    void setCurrentStateTask();

    // Map storing all available states.
    std::unordered_map<StateIdType, std::unique_ptr<State<StateIdType>>> states_;
    
    // Pointer to the currently active state object.
    State<StateIdType>* currentState_ = nullptr;
    
    // IDs for tracking state transitions.
    StateIdType currentStateId_;
    StateIdType previousStateId_;

    std::any currentStateTask_; // Placeholder for any job or task related to the current state


};

// Include the implementation file at the end
#include "StateMachine.tpp"

#endif // STATEMACHINE_H
