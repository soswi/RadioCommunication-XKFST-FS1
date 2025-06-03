// --- Plik: sync/SyncManager.h ---
#ifndef SYNC_MANAGER_H
#define SYNC_MANAGER_H

void initSyncManager();
void IRAM_ATTR onSyncPulse();
bool getAndClearSyncFlag();

#endif