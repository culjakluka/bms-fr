/**
 * BMS Master – konstante
 * Jedino mjesto za sve brojeve (pinovi, CAN, baterija, limiti).
 */

#ifndef CONFIG_H
#define CONFIG_H

/* Baterija (Feature 1) */
#define SOC_PERCENT_MIN           0
#define SOC_PERCENT_MAX           100
#define SOC_INITIAL_PERCENT       80
#define TOTAL_CAPACITY_WH         2000.0f
#define SECONDS_PER_HOUR          3600.0f
#define PERCENT_SCALE             100.0f

/* Stanja BMS-a */
#define BMS_STATE_IDLE            0
#define BMS_STATE_READY            1
#define BMS_STATE_ERROR            2

/* Power limiting (Feature 2) */
#define POWER_LIMIT_MAX_W         5000
#define POWER_LIMIT_MIN_W         500
#define MAX_DISCHARGE_TIME_S      3600   /* base limit = capacity_Wh * 3600 / this */

/* CAN (Feature 4) */
#define CAN_TX_ID                 0x100
#define CAN_RX_ID                 0x200
#define CAN_TX_INTERVAL_MS        100
#define CAN_HEARTBEAT_TIMEOUT_MS  50

/* Timing */
#define MS_PER_SECOND             1000

/* Senzori (Feature 5) – rasponi */
#define PACK_VOLTAGE_MIN_V        10.0f
#define PACK_VOLTAGE_MAX_V        15.0f
#define PACK_CURRENT_MIN_A        0.0f
#define PACK_CURRENT_MAX_A        50.0f
#define PACK_TEMP_MIN_C           0.0f
#define PACK_TEMP_MAX_C           60.0f
#define PACK_TEMP_DEFAULT_C       25
#define ADC_MAX                   1023
#define ADC_VREF_V                5.0f

/* Pinovi */
#define PIN_BUTTON                2
#define PIN_LED                   3
#define PIN_CAN_CS                10
#define PIN_CAN_INT               9

#endif
