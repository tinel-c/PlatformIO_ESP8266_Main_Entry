#pragma once

#include <PubSubClient.h>
#include <stdio.h>

// SMS gateway device watchdog (RAM list on PlatformIO_esp32-sms-gateway).
// Copy this header into each new device repo and call on MQTT connect plus
// the status publish cadence (~30 s). Name must match the fleet id.
// Gateway timeout = 2 × interval; default interval 60 → alert after 120 s silence.
// Cap: MAX_WATCHDOG_DEVICES (10). Full table drops new enrolls silently.
// Re-enroll is required after an SMS gateway reboot (list is not persisted).

#ifndef SMS_WATCHDOG_ENROLL_TOPIC
#define SMS_WATCHDOG_ENROLL_TOPIC "sms/gateway/watchdog/enroll"
#endif
#ifndef SMS_WATCHDOG_HEARTBEAT_TOPIC
#define SMS_WATCHDOG_HEARTBEAT_TOPIC "sms/gateway/watchdog/heartbeat"
#endif
#ifndef SMS_WATCHDOG_DEFAULT_INTERVAL_S
#define SMS_WATCHDOG_DEFAULT_INTERVAL_S 60u
#endif

inline void smsWatchdogRegister(PubSubClient& mqtt, const char* deviceName,
                                unsigned intervalSec = SMS_WATCHDOG_DEFAULT_INTERVAL_S) {
    if (!mqtt.connected() || deviceName == nullptr || deviceName[0] == '\0') {
        return;
    }
    char enroll[192];
    snprintf(enroll, sizeof(enroll), "{\"name\":\"%s\",\"interval\":%u}", deviceName,
             intervalSec);
    mqtt.publish(SMS_WATCHDOG_ENROLL_TOPIC, enroll, false);

    char heartbeat[128];
    snprintf(heartbeat, sizeof(heartbeat), "{\"name\":\"%s\"}", deviceName);
    mqtt.publish(SMS_WATCHDOG_HEARTBEAT_TOPIC, heartbeat, false);
}
