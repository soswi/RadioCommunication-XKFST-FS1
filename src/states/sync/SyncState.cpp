#include "SyncState.h"
#include "states/StateIds.h"
#include "state/StateMachine.h"
#include <Arduino.h>

// Explicitly instantiate the template for MasterStates.
// This helps the linker and can improve compilation times and error messages.
template class SyncState<MasterStates>;

/**
 * @brief Handles the logic for the synchronization state.
 */
template<typename StateIdType>
void SyncState<StateIdType>::handle() {
    Serial.println("Handling Sync State: Synchronization logic will run here.");

    // TODO: Implement the actual synchronization mechanism.
    // This could involve:
    // 1. Starting a timer or counter.
    // 2. Sending a synchronization pattern or request packet.
    // 3. Listening for a response from the other device.
    // 4. Once synchronization is confirmed, transition to the next state.

    // Example of a future state transition:
    // if (isSynced) {
    //     this->machine_->setState(StateIdType::Tx); // or Rx, or a new Handshake state
    // }
}
