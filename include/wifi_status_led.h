#pragma once

#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

// Wi-Fi status LED for every new ESP device that has a single indicator LED.
// Copy this header into the device repo. Include board_pins.h first.
// board_pins.h defines PIN_STATUS_LED, LED_ACTIVE_LEVEL, and LED_IDLE_LEVEL.
// PIN_STATUS_LED < 0 disables the indicator.
//
// "Lit" means LED_ACTIVE_LEVEL. Confirm the polarity on the board: the
// thyristor blue LED on IO5 is lit when the pin is low.
//
// Call updateWifiStatusLed(lastWifiBeginMs) from loop(). Pass millis() of the
// latest WiFi.begin(), or 0 before the first attempt.
//
//   associated: lit, with an 80 ms dark wink every 2 s
//   joining:    100 ms lit / 100 ms dark for 3 s after WiFi.begin
//   down:       500 ms lit / 500 ms dark while waiting to retry
//   failed:     two 80 ms flashes, then dark, repeating every 1.2 s
//               (WL_NO_SSID_AVAIL, WL_CONNECT_FAILED, WL_CONNECTION_LOST)

inline void updateWifiStatusLed(uint32_t lastWifiBeginMs) {
    if (PIN_STATUS_LED < 0) {
        return;
    }
    const uint32_t now = millis();
    const wl_status_t status = WiFi.status();
    bool lit = false;
    if (status == WL_CONNECTED) {
        lit = (now % 2000) >= 80;
    } else if (status == WL_NO_SSID_AVAIL || status == WL_CONNECT_FAILED ||
               status == WL_CONNECTION_LOST) {
        const uint32_t phase = now % 1200;
        lit = phase < 80 || (phase >= 160 && phase < 240);
    } else if (lastWifiBeginMs != 0 && now - lastWifiBeginMs < 3000) {
        lit = (now % 200) < 100;
    } else {
        lit = (now % 1000) < 500;
    }
    digitalWrite(PIN_STATUS_LED, lit ? LED_ACTIVE_LEVEL : LED_IDLE_LEVEL);
}
