#include "SyncState.h"
#include "states/StateIds.h"
#include <Arduino.h>

template class SyncState<MasterStates>;

template<typename StateIdType>
void SyncState<StateIdType>::handle() {
    Serial.println("Handling Sync State: Synchronization logic would run here.");
    // TODO: Implement the actual synchronization mechanism.
    // After syncing, you would transition to the next state (e.g., Tx or Rx).
    // For now, it does nothing more.
}