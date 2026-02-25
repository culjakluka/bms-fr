/**
 * Feature 2: Power limiting
 * API – main proslijedi current_wh i power_request_w, čita limit za CAN.
 *
 * Spec: Input capacity + VCU request; rate check (predikt 1 s);
 * limit = capacity_Wh × 3600 / max_discharge_time_s; smanji ako bi za 1 s palo < 0%.
 */

#ifndef FEATURE2_POWER_H
#define FEATURE2_POWER_H

#include <stdint.h>

void power_limit_init(void);

/**
 * Ažurira limit: koristi capacity iz config.h, current_wh i power_request_w.
 * Rate check: predvida Wh nakon 1 s; ako bi < 0, smanjuje limit.
 */
void power_limit_update(float current_wh, uint16_t power_request_w);

uint16_t power_limit_get_w(void);

#endif
