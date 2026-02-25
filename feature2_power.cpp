/**
 * Feature 2: Power limiting – implementacija
 *
 * Input: Battery capacity (config), power request from VCU (argument).
 * Rate check: predikt Wh nakon 1 s pri trenutnom power_request.
 * Limit: base = capacity_Wh × 3600 / max_discharge_time_s;
 *        ako bi za 1 s bilo < 0%, limit = min(base, current_wh × 3600).
 * Output: limit u W za CAN.
 */

#include "Arduino.h"
#include "feature2_power.h"
#include "config.h"

static uint16_t limit_w;

void power_limit_init(void) {
  float base = (TOTAL_CAPACITY_WH * SECONDS_PER_HOUR) / (float)MAX_DISCHARGE_TIME_S;
  limit_w = (uint16_t)base;
  if (limit_w > POWER_LIMIT_MAX_W) {
    limit_w = POWER_LIMIT_MAX_W;
  }
}

void power_limit_update(float current_wh, uint16_t power_request_w) {
  float capacity_wh = TOTAL_CAPACITY_WH;
  float base_limit_w = (capacity_wh * SECONDS_PER_HOUR) / (float)MAX_DISCHARGE_TIME_S;

  /* Rate check: energija koju bi izvukli za 1 s (1 s = 1/3600 h) */
  float drain_wh_in_1s = (float)power_request_w / SECONDS_PER_HOUR;
  float wh_after_1s = current_wh - drain_wh_in_1s;

  if (wh_after_1s >= 0.0f) {
    /* Nema opasnosti da padne ispod 0% u sljedećoj sekundi */
    float w = base_limit_w;
    if (w > (float)POWER_LIMIT_MAX_W) w = (float)POWER_LIMIT_MAX_W;
    limit_w = (uint16_t)w;
    return;
  }

  /* Smanji limit: najviše onoliko W da za 1 s ne padnemo ispod 0% */
  float max_safe_w = current_wh * SECONDS_PER_HOUR;
  float w = (base_limit_w < max_safe_w) ? base_limit_w : max_safe_w;
  if (w < (float)POWER_LIMIT_MIN_W) w = (float)POWER_LIMIT_MIN_W;
  limit_w = (uint16_t)w;
}

uint16_t power_limit_get_w(void) {
  return limit_w;
}
