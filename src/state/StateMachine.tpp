// FILE: src/state/StateMachine.tpp

#ifndef STATEMACHINE_TPP
#define STATEMACHINE_TPP

#include "StateMachine.h"
#include <utility>
#include <any>

template <typename StateIdType>
template <typename... States>
StateMachine<StateIdType>::StateMachine(StateIdType initialState, States&&... states)
    : currentStateId_(initialState), previousStateId_(initialState) {
    
    // C++17 fold expression to iterate through the passed state objects.
    // For each state, its ID is retrieved via getStateId() and used as a key
    // to store a unique_ptr to the object in the `states_` map.
    ( (states_[states.getStateId()] = std::make_unique<std::decay_t<States>>(std::forward<States>(states))), ... );

    // Establish a back-reference from each state to this FSM instance.
    // This allows states to call back into the FSM (e.g., to trigger transitions).
    for (auto const& [id, state_ptr] : states_) {
        if (state_ptr) {
            state_ptr->setStateMachine(this);
        }
    }
    // Set the initial currentState_ pointer.
    findAndSetCurrentState(currentStateId_);
}

// Simple state transition, sets only the ID. The actual transition logic
// is deferred to the update() loop. Designed to be ISR-safe.
template <typename StateIdType>
void StateMachine<StateIdType>::setState(StateIdType newState) {
    previousStateId_ = currentStateId_;
    currentStateId_ = newState;
}

// Overloaded setState to include a task payload. Also ISR-safe.
// It moves the task payload into a member variable for later processing.
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

// Finds the state object pointer from the map by its ID.
// This operation is wrapped in a critical section to prevent race conditions
// if an ISR were to modify the state map concurrently (not our current design, but good practice).
template <typename StateIdType>
void StateMachine<StateIdType>::findAndSetCurrentState(StateIdType stateId) {
    noInterrupts();
    auto it = states_.find(stateId);
    interrupts();

    if (it != states_.end()) {
        currentState_ = it->second.get();
    } else {
        currentState_ = nullptr; // Safety: nullify pointer if state not found.
    }
}

// Delivers the pending task from the FSM to the state object.
template <typename StateIdType>
void StateMachine<StateIdType>::setCurrentStateTask() {
    if (currentState_) {
        currentState_->setTask(currentStateTask_);
    }
}

template <typename StateIdType>
void StateMachine<StateIdType>::update() {
    // A transition is pending if the current and previous state IDs differ.
    if (currentStateId_ != previousStateId_) {
        // Update the raw pointer to the new state object.
        findAndSetCurrentState(currentStateId_);
        // Deliver the task payload (if any) to the new state.
        setCurrentStateTask();
    }

    // Delegate execution to the current state's logic.
    if (currentState_) {
        currentState_->handle();
    }
}

#endif // STATEMACHINE_TPP
