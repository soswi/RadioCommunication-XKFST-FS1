#ifndef STATEMACHINE_TPP
#define STATEMACHINE_TPP

#include "StateMachine.h"
#include <iostream>

template <typename... States>
StateMachine::StateMachine(States&&... states)
    : currentState_(CommState::Idle)
{
    // Use a fold expression to insert all states into the map
    // We move each state into a unique_ptr and store keyed by getStateId()
    (void)std::initializer_list<int>{
        ( 
            states_.emplace(
                states.getStateId(), 
                std::make_unique<std::remove_reference_t<decltype(states)>>(std::forward<States>(states))
            )->second->setStateMachine(this),
        0)...
    };
}

State* StateMachine::findState(CommState id) {
    auto it = states_.find(id);
    if (it != states_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void StateMachine::setState(CommState newState) {
    currentState_ = newState;
}

void StateMachine::update() {
    State* current = findState(currentState_);
    if (current) {
        current->handle();
    } else {
        std::cerr << "Error: State not found!\n";
    }
}

CommState StateMachine::getCurrentState() const {
    return currentState_;
}

#endif // STATEMACHINE_TPP
