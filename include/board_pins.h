#pragma once

// ESP-12E module LED (BUILTIN_LED). Lit when the pin is low.
// GPIO2 is a boot strap: idle high is the normal flash-boot level.
#define PIN_STATUS_LED 2
#define LED_ACTIVE_LEVEL LOW
#define LED_IDLE_LEVEL HIGH
