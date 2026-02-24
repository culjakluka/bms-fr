/**
 * Feature 1: Battery simulation
 * API – main samo poziva ove funkcije.
 */

#ifndef FEATURE1_BATTERY_H
#define FEATURE1_BATTERY_H

#include <stdint.h>
#include <stdbool.h>

/** Jednom u setup(). */
void battery_init(void);

/** Svaki loop: dt_s, power_request_w, prazni li se (npr. state == READY). */
void battery_update(float dt_s, uint16_t power_request_w, bool is_discharging);

/** SoC 0..100 %. */
float battery_get_soc_pct(void);

/** E u Wh. */
float battery_get_current_wh(void);

/** true ako je 0% – main postavlja ERROR. */
bool battery_is_empty(void);

#endif
