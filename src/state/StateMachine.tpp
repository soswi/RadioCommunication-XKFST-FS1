// FILE: src/state/StateMachine.tpp

#ifndef STATEMACHINE_TPP
#define STATEMACHINE_TPP

#include "StateMachine.h"
// Note: Do NOT include StateMachine.h here to avoid circular inclusion.
// The header already includes this file.

// Template definition for the constructor
template <typename StateIdType>
template <typename... States>
StateMachine<StateIdType>::StateMachine(StateIdType initialState, States&&... states)
    : currentState_(initialState) // Use the provided initial state
{
    ( (void)states_.emplace(
        states.getStateId(),
        std::make_unique<std::remove_reference_t<decltype(states)>>(std::forward<States>(states))
    ).first->second->setStateMachine(this), ... );
}

// Template definition for findState
template <typename StateIdType>
State<StateIdType>* StateMachine<StateIdType>::findState(StateIdType id) {
    auto it = states_.find(id);
    if (it != states_.end()) {
        return it->second.get();
    }
    return nullptr;
}

// Template definition for setState
template <typename StateIdType>
void StateMachine<StateIdType>::setState(StateIdType newState) {
    currentState_ = newState;
}

// Template definition for update
template <typename StateIdType>
void StateMachine<StateIdType>::update() {
    State<StateIdType>* current = findState(currentState_);
    if (current) {
        current->handle();
    }
}

// Template definition for getCurrentState
template <typename StateIdType>
StateIdType StateMachine<StateIdType>::getCurrentState() const {
    return currentState_;
}

#endif // STATEMACHINE_TPP