// FILE: src/state/StateMachine.h

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "State.h"
#include <memory>
#include <unordered_map>
#include <Arduino.h>
#include <any>

/**
 * @brief A generic, template-based Finite State Machine (FSM).
 * @tparam StateIdType An enum class that defines the set of possible states.
 */
template <typename StateIdType>
class StateMachine {
public:
    /**
     * @brief Constructs the FSM and populates it with states.
     * @tparam States A parameter pack of concrete state types.
     * @param initialState The ID of the state to start in.
     * @param states R-value references to the state objects.
     */
    template <typename... States>
    explicit StateMachine(StateIdType initialState, States&&... states);

    // --- Public API for state transitions ---

    /**
     * @brief Requests a simple state transition. ISR-safe.
     * @param newState The ID of the target state.
     */
    void setState(StateIdType newState);

    /**
     * @brief Requests a state transition with a task payload. ISR-safe.
     * @param newState The ID of the target state.
     * @param newTask A std::any payload to be passed to the new state.
     */
    void setState(StateIdType newState, std::any newTask);

    // --- Getters for current status ---
    
    StateIdType getCurrentStateId() const;
    StateIdType getPreviousStateId() const;

    /**
     * @brief Main update loop for the FSM. Should be called repeatedly.
     * Handles transitions and delegates execution to the current state's handle().
     */
    void update();

private:
    // --- Internal helper methods ---

    /**
     * @brief Finds the state object corresponding to an ID and updates the currentState_ pointer.
     * @param id The ID of the state to find.
     */
    void findAndSetCurrentState(StateIdType id);

    /**
     * @brief Pushes the pending task from the FSM into the current state object.
     */
    void setCurrentStateTask();

    // --- Member Variables ---

    // A map from a state ID (enum) to a unique pointer holding the state object.
    std::unordered_map<StateIdType, std::unique_ptr<State<StateIdType>>> states_;
    
    // Raw pointer to the currently active state object for fast access.
    State<StateIdType>* currentState_ = nullptr;
    
    // State tracking IDs. The difference between them signals a transition request.
    StateIdType currentStateId_;
    StateIdType previousStateId_;

    // A temporary container for a task payload during a state transition.
    std::any currentStateTask_;
};

// Implementation is sourced from the .tpp file.
#include "StateMachine.tpp"

#endif // STATEMACHINE_H
