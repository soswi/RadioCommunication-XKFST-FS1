#ifndef IDLESTATE_H
#define IDLESTATE_H

#include "state/State.h"
#include "states/StateIds.h"

template<typename StateIdType>
class TxState : public State<StateIdType> {
public:
    /**
     * @brief Default constructor.
     */
    TxState() = default;

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
        return StateIdType::Tx;
    }
};

#endif // IDLESTATE_H
