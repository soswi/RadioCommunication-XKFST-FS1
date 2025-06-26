// FILE: src/state/StateMachine.tpp

#ifndef STATEMACHINE_TPP
#define STATEMACHINE_TPP

#include "StateMachine.h"
#include "State.h"
#include <utility>
#include <any>

template <typename StateIdType>
template <typename... States>
StateMachine<StateIdType>::StateMachine(StateIdType initialState, States&&... states)
    : currentStateId_(initialState), previousStateId_(initialState) {
    
    ( (states_[states.getStateId()] = std::make_unique<std::decay_t<States>>(std::forward<States>(states))), ... );

    for (auto const& [id, state_ptr] : states_) {
        if (state_ptr) {
            state_ptr->setStateMachine(this);
        }
    }
    // Set the initial state pointer right away
    findAndSetCurrentState(currentStateId_);
}

template <typename StateIdType>
void StateMachine<StateIdType>::setState(StateIdType newState) {
    // This is ISR-safe. It only changes the ID.
    previousStateId_ = currentStateId_;
    currentStateId_ = newState;
}

// Overload to pass a task to a state (specifically SyncState)
template <typename StateIdType>
void StateMachine<StateIdType>::setState(StateIdType newState, std::any newTask) {
    previousStateId_ = currentStateId_;
    currentStateId_ = newState;
    currentStateTask_ = std::move(newTask);
}


template <typename StateIdType>
StateIdType StateMachine<StateIdType>::getCurrentStateId() const {
    return currentStateId_;
}

template <typename StateIdType>
StateIdType StateMachine<StateIdType>::getPreviousStateId() const {
    return previousStateId_;
}

template <typename StateIdType>
void StateMachine<StateIdType>::findAndSetCurrentState(StateIdType stateId) {
    noInterrupts();
    auto it = states_.find(stateId);
    interrupts();

    if (it != states_.end()) {
        currentState_ = it->second.get();
    } else {
        currentState_ = nullptr;
    }
}

template <typename StateIdType>
void StateMachine<StateIdType>::setCurrentStateTask() {
    currentState_->setStateTask(currentStateTask_);
}

template <typename StateIdType>
void StateMachine<StateIdType>::update() {
    // If the state ID has changed, find and update the pointer to the state object.
    if (currentStateId_ != previousStateId_) {
        findAndSetCurrentState(currentStateId_);
        setCurrentStateTask();
    }

    // Execute the handle method of the current state.
    if (currentState_) {
        currentState_->handle();
    }
}

#endif // STATEMACHINE_TPP
