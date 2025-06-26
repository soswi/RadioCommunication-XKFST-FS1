#include "IdleState.h"
#include "states/StateIds.h" // Include the enum definition
#include "state/StateMachine.h" // Needed for state transitions
#include <Arduino.h>

// This is an explicit instantiation of the template.
template class IdleState<MasterStates>;

template<typename StateIdType>
void IdleState<StateIdType>::handle() {}