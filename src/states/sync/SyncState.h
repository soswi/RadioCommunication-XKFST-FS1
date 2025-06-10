#ifndef SYNCSTATE_H
#define SYNCSTATE_H

#include "state/State.h"
#include "states/StateIds.h"

template<typename StateIdType>
class SyncState : public State<StateIdType> {
public:
    SyncState() = default;

    void handle() override;

    StateIdType getStateId() const override {
        return StateIdType::Sync;
    }
};

#endif // SYNCSTATE_H
