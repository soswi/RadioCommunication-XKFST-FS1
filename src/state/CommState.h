#ifndef COMMSTATE_H
#define COMMSTATE_H

enum class CommState {
    Idle,
    Sync,
    Tx,
    Rx
    // Add more states here as needed
};

#endif // COMMSTATE_H
