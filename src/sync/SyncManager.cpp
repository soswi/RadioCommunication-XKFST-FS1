// --- Plik: sync/SyncManager.cpp ---
#include "SyncManager.h"
#include "state/StateMachine.h"
#include <Arduino.h>

namespace {
  constexpr int RECIVER_PIN = 2;  // GPIO do synchronizacji
  volatile bool syncFlag = false;
}

void IRAM_ATTR onSyncPulse() {
  syncFlag = true;
}

void initSyncManager() {
  pinMode(RECIVER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SYNC_PIN), onSyncPulse, RISING);
}

bool getAndClearSyncFlag() {
  bool wasSet = syncFlag;
  syncFlag = false;
  return wasSet;
}
