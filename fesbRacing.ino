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
#include "feature2_power.h"
#include "feature5_simulation.h"

/* Hardware */
MCP2515 can(PIN_CAN_CS);

/* Stanje (glavni file) */
uint8_t state = BMS_STATE_IDLE;
uint16_t power_request_w = 0;
uint16_t power_limit_w;
unsigned long last_ms = 0;
unsigned long last_tx_ms = 0;
bool button_prev = HIGH;

/* Feature 5 */
float pack_voltage_v, pack_current_a, pack_temp_c;

/* Konvencija VCU */
uint16_t decodeBytes(uint8_t lowByte, uint8_t highByte) {
  return ((uint16_t)highByte << 8) | lowByte;
}

void encodeBytes(uint16_t value, uint8_t &lowByte, uint8_t &highByte) {
  lowByte  = value & 0xFF;
  highByte = (value >> 8) & 0xFF;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_VOLTAGE_POT, INPUT);
  pinMode(PIN_CURRENT_POT, INPUT);
  pinMode(PIN_TEMP_POT, INPUT);
  button_prev = (digitalRead(PIN_BUTTON) == HIGH);

  battery_init();
  power_limit_init();

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

  /* CAN RX: power request (low byte, high byte) */
  struct can_frame rx;
  if (can.readMessage(&rx) == MCP2515::ERROR_OK && rx.can_id == CAN_RX_ID) {
    power_request_w = decodeBytes(rx.data[4], rx.data[5]);
  }

  /* Feature 1: baterija (samo u READY se prazni) */
  battery_update(dt_s, power_request_w, (state == BMS_STATE_READY));
  if (state == BMS_STATE_READY && battery_is_empty()) {
    state = BMS_STATE_ERROR;
  }

  /* Feature 2: power limit (capacity, request, rate check za 1 s) */
  power_limit_update(battery_get_current_wh(), power_request_w);
  power_limit_w = power_limit_get_w();

  /*Feature 5: sensor simulation*/
  pack_voltage_v = analogRead(PIN_VOLTAGE_POT) * (SENSOR_VOLTAGE_MAX / ADC_MAX);
  pack_current_a = analogRead(PIN_CURRENT_POT) * (SENSOR_CURRENT_MAX / ADC_MAX);
  pack_temp_c = (uint8_t)(analogRead(PIN_TEMP_POT) * (SENSOR_TEMP_MAX / ADC_MAX));

  if(!(check_ranges(pack_voltage_v,pack_current_a,pack_temp_c))){
    state = BMS_STATE_ERROR;
  }
  
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
  encodeBytes(soc, tx.data[1], tx.data[2]);
  encodeBytes(power_limit_w, tx.data[3], tx.data[4]);
  encodeBytes(volt, tx.data[5], tx.data[6]);
  tx.data[7] = pack_temp_c;

  can.sendMessage(&tx);
}
