#include "app_logic.h"

namespace app_logic {

const char* onOff(bool value) {
  return value ? "ON" : "OFF";
}

bool canUseCloudServices(bool mainsPowerAvailable) {
  return mainsPowerAvailable;
}

bool shouldAttemptWifiReconnect(
    bool cloudAllowed,
    bool wifiConnected,
    unsigned long nowMs,
    unsigned long lastAttemptMs,
    unsigned long intervalMs) {
  if (!cloudAllowed || wifiConnected) {
    return false;
  }

  return (nowMs - lastAttemptMs) >= intervalMs;
}

bool shouldAttemptMqttReconnect(
    bool cloudAllowed,
    bool wifiConnected,
    bool mqttConnected,
    unsigned long nowMs,
    unsigned long lastAttemptMs,
    unsigned long intervalMs) {
  if (!cloudAllowed || !wifiConnected || mqttConnected) {
    return false;
  }

  return (nowMs - lastAttemptMs) >= intervalMs;
}

bool shouldOpenGateFromKeypad(bool keypadActivated) {
  return keypadActivated;
}

bool isConfigIntervalValid(int candidate, int minInclusive, int maxInclusive) {
  return candidate >= minInclusive && candidate <= maxInclusive;
}

}  // namespace app_logic
