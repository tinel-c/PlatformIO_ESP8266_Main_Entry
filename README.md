# PlatformIO ESP8266 Main Gate Entry

ESP8266 firmware for a main gate controller with local keypad triggering, MQTT remote control, automatic WiFi/MQTT recovery, and offline-safe behavior.

## Features

- Controls 4 relays on an ESP8266 relay board.
- Opens the gate from keypad input without requiring internet/MQTT.
- Monitors mains availability and switches to offline self-management when mains is OFF.
- Automatically re-enables WiFi and MQTT when mains returns.
- Publishes clean `ON` / `OFF` status messages on `STAT` topics.
- Includes a relay coil protection failsafe so gate relay (`Relay3`) cannot remain ON indefinitely.

## Hardware Context

- ESP8266 4-relay board (`esp12e` target).
- Main gate trigger relay (`Relay3` in topics).
- Keypad input.
- Mains availability input.
- 12V supply with backup power path (recommended).

## Project Structure

- `src/main.cpp`: firmware logic (interrupts, debounce, relay control, connectivity, MQTT callback loop).
- `src/app_logic.cpp` and `include/app_logic.h`: testable pure logic for reconnect and validation rules.
- `test/test_app_logic/test_main.cpp`: Unity unit tests for edge cases and offline behavior.

## Configuration

1. Create `password.h` from your template (SSID, WiFi password, MQTT server).
2. Build and upload to board:

```bash
platformio run -e esp12e
platformio run -e esp12e -t upload
```

3. Open serial monitor:

```bash
platformio device monitor -b 115200
```

## Runtime Behavior

- **Mains ON**
  - WiFi and MQTT are used.
  - Reconnect attempts are automatic and non-blocking.
- **Mains OFF**
  - Cloud services are disabled.
  - Keypad can still trigger gate opening locally.
  - System keeps running without reset or external intervention.
- **Gate relay protection**
  - Normal close timer turns relay OFF after configured open window.
  - Loop-level failsafe force-closes gate relay if ON beyond safety max duration.

## MQTT Quick Reference

Use this section as the main copy/paste reference for MQTT integration.

### Commands (publish to device)

```text
MainGate/CMD/Relay1                ON|OFF
MainGate/CMD/Relay2                ON|OFF
MainGate/CMD/Relay3                ON|OFF
MainGate/CMD/Relay4                ON|OFF
MainGate/CMD/statusReccurence      1001..80000
MainGate/CMD/debounceSampleTime    1000..80000
MainGate/CMD/relayProcessTimer     1000..80000
```

### Status and events (subscribe from device)

```text
MainGate/STAT/Relay1                    ON|OFF
MainGate/STAT/Relay2                    ON|OFF
MainGate/STAT/Relay3                    ON|OFF
MainGate/STAT/Relay4                    ON|OFF
MainGate/STAT/eventRelay                ON|OFF
MainGate/STAT/eventKeypad               ON|OFF
MainGate/STAT/eventPower                ON|OFF
MainGate/STAT/reccurentStatusRelay1     ON|OFF
MainGate/STAT/reccurentStatusRelay2     ON|OFF
MainGate/STAT/reccurentStatusRelay3     ON|OFF
MainGate/STAT/reccurentStatusRelay4     ON|OFF
MainGate/STAT/reccurentStatusKeypad     ON|OFF
MainGate/STAT/reccurentStatusMains      ON|OFF
MainGate/STAT/heartbeat                 <seconds-online>
MainGate/STAT/message                   <debug-text>
```

## MQTT Topics (Detailed)

### Commands (`MainGate/CMD/...`)

- `Relay1`, `Relay2`, `Relay3`, `Relay4`: `ON` / `OFF`
- `statusReccurence`: integer, valid range `1001..80000`
- `debounceSampleTime`: integer, valid range `1000..80000`
- `relayProcessTimer`: integer, valid range `1000..80000`

### Status (`MainGate/STAT/...`)

- Direct status:
  - `Relay1`, `Relay2`, `Relay3`, `Relay4` -> `ON` / `OFF`
- Event status:
  - `eventRelay`, `eventKeypad`, `eventPower` -> `ON` / `OFF`
- Recurrent status:
  - `reccurentStatusRelay1`
  - `reccurentStatusRelay2`
  - `reccurentStatusRelay3`
  - `reccurentStatusRelay4`
  - `reccurentStatusKeypad`
  - `reccurentStatusMains`
  - all recurrent payloads are `ON` / `OFF`
- Debug:
  - `message`

## Testing

This project includes Unity unit tests for reconnect logic, offline operation, validation boundaries, and status mapping.

Run native tests:

```bash
platformio test -e native
```

If `gcc/g++` is missing on Windows, install a toolchain (for example WinLibs) and ensure compiler binaries are in `PATH`.