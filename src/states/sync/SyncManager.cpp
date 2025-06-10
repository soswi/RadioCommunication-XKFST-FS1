// FILE: src/sync/SyncManager.cpp

#include "SyncManager.h"

// A global pointer to our SyncManager instance.
// This is needed so the global C-style ISR can find our C++ class instance.
static SyncManager* globalSyncManagerInstance = nullptr;

// The actual ISR function. It must have C-style linkage for attachInterrupt.
// IRAM_ATTR ensures it's placed in fast RAM.
void IRAM_ATTR onSyncPulse() {
    // ISRs should be as short and fast as possible.
    // We just call a method on our manager instance.
    if (globalSyncManagerInstance) {
        globalSyncManagerInstance->handlePulse();
    }
}

SyncManager::SyncManager(int syncPin) : syncPin_(syncPin) {
    // The constructor just initializes the pin number.
}

void SyncManager::begin() {
    // Set the global pointer to this instance.
    globalSyncManagerInstance = this;

    // Configure the pin and attach the interrupt.
    pinMode(syncPin_, INPUT);
    attachInterrupt(digitalPinToInterrupt(syncPin_), onSyncPulse, RISING);
}

void SyncManager::handlePulse() {
    // This is called from the ISR.
    syncFlag_ = true;
}

bool SyncManager::getAndClearSyncFlag() {
    // This is an atomic way to check and clear the flag.
    // We disable interrupts briefly to prevent a race condition where an interrupt
    // could occur between reading and writing the flag.
    noInterrupts();
    bool flagState = syncFlag_;
    syncFlag_ = false;
    interrupts();
    return flagState;
}