#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <unordered_map>
#include <memory>
#include <utility> // for std::forward
#include "State.h"

class StateMachine {
public:
    // Variadic template constructor accepting any number of State-derived objects
    template <typename... States>
    explicit StateMachine(States&&... states);

    void setState(CommState newState);

    void update();

    CommState getCurrentState() const;

private:
    std::unordered_map<CommState, std::unique_ptr<State>> states_;
    CommState currentState_;

    State* findState(CommState id);
};

#include "StateMachine.tpp"  // Implementation of template constructor

#endif // STATEMACHINE_H
