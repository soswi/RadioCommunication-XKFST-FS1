// FILE: src/state/State.h

#ifndef STATE_H
#define STATE_H

#include <any>

// Forward declaration to break circular dependency with StateMachine.h.
template <typename StateIdType>
class StateMachine;

/**
 * @brief An abstract base class (interface) for all concrete states.
 * Defines the contract that every state must fulfill.
 * @tparam StateIdType The enum class used for state identification.
 */
template <typename StateIdType>
class State {
public:
    virtual ~State() = default;

    // --- Pure virtual functions (must be implemented by derived states) ---

    /**
     * @brief The primary logic function for the state. Called repeatedly by the FSM.
     */
    virtual void handle() = 0;

    /**
     * @brief Returns the unique ID of this state. Used to populate the FSM's state map.
     * @return StateIdType The enum value corresponding to this state.
     */
    virtual StateIdType getStateId() const = 0;

    // --- Public methods for FSM interaction ---

    /**
     * @brief Receives a task payload from the FSM.
     * @param task A std::any object containing the task context.
     */
    void setTask(std::any task) {
        stateTask_ = task;
    }

    /**
     * @brief Sets the back-pointer to the parent FSM.
     * @param machine A pointer to the StateMachine that owns this state.
     */
    void setStateMachine(StateMachine<StateIdType>* machine) {
        machine_ = machine;
    }

protected:
    // A pointer to the parent FSM, allowing states to trigger transitions.
    StateMachine<StateIdType>* machine_ = nullptr;
    
    // A generic container for a task payload, delivered upon state entry.
    std::any stateTask_;
};

#endif // STATE_H
