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
    StateIdType getCurrentState() const;
    void update();

private:
    State<StateIdType>* findState(StateIdType id);

    std::unordered_map<StateIdType, std::unique_ptr<State<StateIdType>>> states_;
    StateIdType currentState_;
};

// Include the implementation file at the end
#include "StateMachine.tpp"

#endif // STATEMACHINE_H