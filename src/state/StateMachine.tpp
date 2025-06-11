// FILE: src/state/StateMachine.tpp

#ifndef STATEMACHINE_TPP
#define STATEMACHINE_TPP

#include "StateMachine.h"
#include <utility>

/**
 * @brief Construct a new State Machine object
 * @tparam StateIdType The enum type for state identifiers.
 * @tparam States The types of the state objects.
 * @param initialState The initial state for the machine.
 * @param states The state objects to be managed by the machine.
 */
template <typename StateIdType>
template <typename... States>
StateMachine<StateIdType>::StateMachine(StateIdType initialState, States&&... states)
    : currentState_(initialState) {
    
    // Use a C++17 fold expression to populate the map from the provided states.
    ( (states_[states.getStateId()] = std::make_unique<std::decay_t<States>>(std::forward<States>(states))), ... );

    // Set the back-pointer to this state machine instance for each state.
    for (auto const& [id, state_ptr] : states_) {
        if (state_ptr) {
            state_ptr->setStateMachine(this);
        }
    }
}

/**
 * @brief Sets the current state of the machine.
 * This method is designed to be lean enough to be called from an ISR.
 * @param newState The new state to switch to.
 */
template <typename StateIdType>
void StateMachine<StateIdType>::setState(StateIdType newState) {
    currentState_ = newState;
}

/**
 * @brief Gets the ID of the current state.
 * @return StateIdType The current state's ID.
 */
template <typename StateIdType>
StateIdType StateMachine<StateIdType>::getCurrentState() const {
    return currentState_;
}

/**
 * @brief Finds a state pointer in the map based on its ID.
 * This is a critical section, as it accesses the state map which could be
 * in contention if an ISR calls setState(). Interrupts are disabled
 * briefly to ensure atomic-like access relative to state changes.
 * @param id The ID of the state to find.
 * @return State<StateIdType>* A pointer to the state object, or nullptr if not found.
 */
template <typename StateIdType>
State<StateIdType>* StateMachine<StateIdType>::findState(StateIdType id) {
    // --- CRITICAL SECTION START ---
    // Interrupts are disabled here as per our design decision to protect
    // the read operation on `currentState_` and the subsequent map lookup
    // from a concurrent write by an ISR calling `setState()`.
    noInterrupts();

    // Find the state in the map.
    auto it = states_.find(id);
    
    // Re-enable interrupts immediately after the critical operations are complete.
    interrupts();
    // --- CRITICAL SECTION END ---

    if (it != states_.end()) {
        return it->second.get();
    }
    return nullptr;
}

/**
 * @brief Updates the state machine by executing the handle() method of the current state.
 */
template <typename StateIdType>
void StateMachine<StateIdType>::update() {
    // The findState method is now protected against interrupts.
    State<StateIdType>* state = findState(currentState_);
    if (state) {
        state->handle();
    }
}


#endif // STATEMACHINE_TPP