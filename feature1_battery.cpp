/**
 * Feature 1: Battery simulation
 */

#include "Arduino.h"
#include "feature1_battery.h"
#include "config.h"

static float capacity_wh;
static float current_wh;
static float soc_pct;

void battery_init(void) {
  capacity_wh = TOTAL_CAPACITY_WH;
  current_wh = capacity_wh * ((float)SOC_INITIAL_PERCENT / PERCENT_SCALE);
  soc_pct = (float)SOC_INITIAL_PERCENT;
}

void battery_update(float dt_s, uint16_t power_request_w, bool is_discharging) {
  if (is_discharging) {
    float drain_wh = (power_request_w * dt_s) / SECONDS_PER_HOUR;
    current_wh -= drain_wh;
    if (current_wh < 0.0f) {
      current_wh = 0.0f;
    }
  }
  soc_pct = (current_wh / capacity_wh) * PERCENT_SCALE;
}

float battery_get_soc_pct(void) {
  return soc_pct;
}

float battery_get_current_wh(void) {
  return current_wh;
}

bool battery_is_empty(void) {
  return soc_pct <= (float)SOC_PERCENT_MIN;
}
