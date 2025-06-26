#ifndef STATEIDS_H
#define STATEIDS_H

enum class MasterStates {
    Idle,
    Sync,
    Tx,
    Rx
};

enum class SyncStates {
    // Common states
    Idle,
    Synced,
    Timeout,
    Request,
    Initiate,

    // Initiator (Transmitter) path states
    Initiate_SendInitialPulse,
    Initiate_SendPreamble,
    Initiate_WaitForConfirmation,
    Initiate_SendFinalTrigger,

    // Request (Receiver) path states
    Request_WaitForInitialPulse,
    Request_MeasurePreamble,
    Request_SendConfirmation,
    Request_WaitForFinalTrigger
};

#endif // COMMSTATE_H
