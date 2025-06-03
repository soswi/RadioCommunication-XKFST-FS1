#ifndef SYNCSTATE_H
#define SYNCSTATE_H

#include "state/State.h"

class SyncState : public State {
public:
    SyncState() = default;

    void handle() override;
};

#endif // SYNCSTATE_H
