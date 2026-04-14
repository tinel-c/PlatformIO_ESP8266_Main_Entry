#pragma once

#include <stdint.h>

namespace app_logic {

const char* onOff(bool value);

bool canUseCloudServices(bool mainsPowerAvailable);

bool shouldAttemptWifiReconnect(
    bool cloudAllowed,
    bool wifiConnected,
    unsigned long nowMs,
    unsigned long lastAttemptMs,
    unsigned long intervalMs);

bool shouldAttemptMqttReconnect(
    bool cloudAllowed,
    bool wifiConnected,
    bool mqttConnected,
    unsigned long nowMs,
    unsigned long lastAttemptMs,
    unsigned long intervalMs);

bool shouldOpenGateFromKeypad(bool keypadActivated);

bool isConfigIntervalValid(int candidate, int minInclusive, int maxInclusive);

}  // namespace app_logic
