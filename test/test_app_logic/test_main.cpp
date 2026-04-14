#include <unity.h>
#include "app_logic.h"

using namespace app_logic;

void setUp(void) {}
void tearDown(void) {}

void test_on_off_payload_is_clean() {
  TEST_ASSERT_EQUAL_STRING("ON", onOff(true));
  TEST_ASSERT_EQUAL_STRING("OFF", onOff(false));
}

void test_cloud_services_allowed_only_with_mains() {
  TEST_ASSERT_TRUE(canUseCloudServices(true));
  TEST_ASSERT_FALSE(canUseCloudServices(false));
}

void test_wifi_reconnect_when_interval_elapsed() {
  TEST_ASSERT_TRUE(shouldAttemptWifiReconnect(true, false, 5000UL, 0UL, 5000UL));
}

void test_wifi_reconnect_not_triggered_before_interval() {
  TEST_ASSERT_FALSE(shouldAttemptWifiReconnect(true, false, 4999UL, 0UL, 5000UL));
}

void test_wifi_reconnect_blocked_when_cloud_not_allowed() {
  TEST_ASSERT_FALSE(shouldAttemptWifiReconnect(false, false, 10000UL, 0UL, 5000UL));
}

void test_wifi_reconnect_blocked_when_already_connected() {
  TEST_ASSERT_FALSE(shouldAttemptWifiReconnect(true, true, 10000UL, 0UL, 5000UL));
}

void test_mqtt_reconnect_when_interval_elapsed() {
  TEST_ASSERT_TRUE(shouldAttemptMqttReconnect(true, true, false, 5000UL, 0UL, 5000UL));
}

void test_mqtt_reconnect_blocked_without_wifi() {
  TEST_ASSERT_FALSE(shouldAttemptMqttReconnect(true, false, false, 10000UL, 0UL, 5000UL));
}

void test_mqtt_reconnect_blocked_when_mains_off() {
  TEST_ASSERT_FALSE(shouldAttemptMqttReconnect(false, true, false, 10000UL, 0UL, 5000UL));
}

void test_mqtt_reconnect_blocked_when_already_connected() {
  TEST_ASSERT_FALSE(shouldAttemptMqttReconnect(true, true, true, 10000UL, 0UL, 5000UL));
}

void test_keypad_opens_gate_offline_or_online() {
  TEST_ASSERT_TRUE(shouldOpenGateFromKeypad(true));
  TEST_ASSERT_FALSE(shouldOpenGateFromKeypad(false));
}

void test_status_recurrence_validation_edges() {
  TEST_ASSERT_FALSE(isConfigIntervalValid(1000, 1001, 80000));
  TEST_ASSERT_TRUE(isConfigIntervalValid(1001, 1001, 80000));
  TEST_ASSERT_TRUE(isConfigIntervalValid(80000, 1001, 80000));
  TEST_ASSERT_FALSE(isConfigIntervalValid(80001, 1001, 80000));
}

void test_generic_interval_validation_edges() {
  TEST_ASSERT_FALSE(isConfigIntervalValid(999, 1000, 80000));
  TEST_ASSERT_TRUE(isConfigIntervalValid(1000, 1000, 80000));
  TEST_ASSERT_TRUE(isConfigIntervalValid(80000, 1000, 80000));
  TEST_ASSERT_FALSE(isConfigIntervalValid(90000, 1000, 80000));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_on_off_payload_is_clean);
  RUN_TEST(test_cloud_services_allowed_only_with_mains);
  RUN_TEST(test_wifi_reconnect_when_interval_elapsed);
  RUN_TEST(test_wifi_reconnect_not_triggered_before_interval);
  RUN_TEST(test_wifi_reconnect_blocked_when_cloud_not_allowed);
  RUN_TEST(test_wifi_reconnect_blocked_when_already_connected);
  RUN_TEST(test_mqtt_reconnect_when_interval_elapsed);
  RUN_TEST(test_mqtt_reconnect_blocked_without_wifi);
  RUN_TEST(test_mqtt_reconnect_blocked_when_mains_off);
  RUN_TEST(test_mqtt_reconnect_blocked_when_already_connected);
  RUN_TEST(test_keypad_opens_gate_offline_or_online);
  RUN_TEST(test_status_recurrence_validation_edges);
  RUN_TEST(test_generic_interval_validation_edges);
  return UNITY_END();
}
