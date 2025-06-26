#ifndef SYNCSTATE_H
#define SYNCSTATE_H

#include "state/State.h"
#include "states/StateIds.h"
#include <any>


/**
 * @class SyncState
 * @brief Represents the state where the device is actively trying to
 * synchronize with another device before main communication begins.
 *
 * This state will contain the logic for the synchronization protocol,
 * which might involve its own sub-states (e.g., sending a request,
 * waiting for a reply).
 */
template<typename StateIdType>
class SyncState : public State<StateIdType> {
public:
    /**
     * @brief Default constructor.
     */

    SyncState();

    ~SyncState() override{
        delete subMachine_;
    };

    /**
     * @brief The main execution handler for this state.
     *
     * This method is called repeatedly by the StateMachine's update() loop
     * while SyncState is the current state.
     */
    void handle() override;

    /**
     * @brief Returns the unique identifier for this state.
     * @return The state's ID from the corresponding enum.
     */
    StateIdType getStateId() const override {
        return StateIdType::Sync;
    }

protected:
    friend class StateMachine<StateIdType>; // Allow StateMachine to access private members
    StateMachine<SyncStates>* subMachine_ = nullptr;
};

template<typename SubStateIdType>
class IdleSyncSubState;

template<typename SubStateIdType>
class RequestSyncSubState;

template<typename SubStateIdType>
class InitiateSyncSubState;

#endif // SYNCSTATE_H
