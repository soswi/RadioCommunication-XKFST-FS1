#ifndef SYNCSTATE_H
#define SYNCSTATE_H

#include "state/State.h"
#include "states/StateIds.h"

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
    SyncState() = default;

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
};

#endif // SYNCSTATE_H
