#ifndef STATEIDS_H
#define STATEIDS_H

enum class MasterStates {
    Idle,
    Sync,
    Tx,
    Rx
};

enum class SyncStates {
    Request,
    Initiate
};

#endif // COMMSTATE_H
