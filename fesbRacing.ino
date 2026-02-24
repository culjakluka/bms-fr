/**
 * BMS Master – glavni file
 * Arduino Uno + HW-184 (MCP2515)
 *
 * Ovdje je samo: init, petlja, CAN, state machine, pozivi u feature module.
 * Logika baterije -> feature1_battery.h
 * Konstante -> config.h
 */

#include <SPI.h>
#include <mcp2515.h>
#include "config.h"
#include "feature1_battery.h"

/* --- Hardware --- */
MCP2515 can(PIN_CAN_CS);

/* --- Stanje (glavni file) --- */
uint8_t state = BMS_STATE_IDLE;
uint16_t power_request_w = 0;
uint16_t power_limit_w = POWER_LIMIT_MAX_W;
unsigned long last_ms = 0;
unsigned long last_tx_ms = 0;
bool button_prev = HIGH;

/* Za CAN TX (kasnije Feature 5 će postavljati) */
float pack_voltage_v = 12.0f;
uint8_t pack_temp_c = PACK_TEMP_DEFAULT_C;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  button_prev = (digitalRead(PIN_BUTTON) == HIGH);

  battery_init();

  can.reset();
  can.setBitrate(CAN_500KBPS, MCP_16MHZ);
  can.setNormalMode();

  last_ms = millis();
  last_tx_ms = last_ms;
  Serial.println(F("BMS Master"));
}

void loop() {
  unsigned long now = millis();
  float dt_s = (float)(now - last_ms) / (float)MS_PER_SECOND;
  last_ms = now;

  /* State machine: gumb */
  bool btn = (digitalRead(PIN_BUTTON) == HIGH);
  if (!btn && button_prev) {
    if (state == BMS_STATE_IDLE) {
      state = BMS_STATE_READY;
    } else if (state == BMS_STATE_ERROR) {
      state = BMS_STATE_IDLE;
    }
  }
  button_prev = btn;

  /* CAN RX: power request */
  struct can_frame rx;
  if (can.readMessage(&rx) == MCP2515::ERROR_OK && rx.can_id == CAN_RX_ID) {
    power_request_w = (uint16_t)((rx.data[4] << 8) | rx.data[5]);
  }

  /* Feature 1: baterija (samo u READY se prazni) */
  battery_update(dt_s, power_request_w, (state == BMS_STATE_READY));
  if (state == BMS_STATE_READY && battery_is_empty()) {
    state = BMS_STATE_ERROR;
  }

  /* Feature 2: power limit (TODO – za sada fiksno) */
  power_limit_w = POWER_LIMIT_MAX_W;

  /* CAN TX @ 10 Hz */
  if (now - last_tx_ms >= CAN_TX_INTERVAL_MS) {
    send_can_bms();
    last_tx_ms = now;
  }

  digitalWrite(PIN_LED, (state == BMS_STATE_READY) ? HIGH : LOW);
}

void send_can_bms(void) {
  struct can_frame tx;
  tx.can_id = CAN_TX_ID;
  tx.can_dlc = 8;

  uint16_t soc = (uint16_t)(battery_get_soc_pct() * 10.0f);
  uint16_t volt = (uint16_t)(pack_voltage_v * 10.0f);

  tx.data[0] = state;
  tx.data[1] = (soc >> 8) & 0xFF;
  tx.data[2] = soc & 0xFF;
  tx.data[3] = (power_limit_w >> 8) & 0xFF;
  tx.data[4] = power_limit_w & 0xFF;
  tx.data[5] = (volt >> 8) & 0xFF;
  tx.data[6] = volt & 0xFF;
  tx.data[7] = pack_temp_c;

  can.sendMessage(&tx);
}
