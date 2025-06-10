// FILE: src/sync/SyncManager.h

#ifndef SYNC_MANAGER_H
#define SYNC_MANAGER_H

#include <Arduino.h>

class SyncManager {
public:
    // Constructor
    SyncManager(int syncPin);

    // Call this from setup()
    void begin();

    // Check this in your main loop or in a state
    bool getAndClearSyncFlag();

    // This method will be called by the ISR. It must be public.
    void handlePulse();

private:
    const int syncPin_;
    // The flag is now a private member of the class.
    // It must be volatile because it's modified in an ISR and read in the main loop.
    volatile bool syncFlag_ = false;
};

#endif // SYNC_MANAGER_H