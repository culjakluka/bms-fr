# Mockup BMS Master Specification (Arduino Uno + HW-184)

## Overview

Mock BMS node that simulates a battery, receives power requests from VCU, computes drain, applies power limiting logic, and broadcasts state/limits over CAN. Also manages its own state machine.

## Hardware

- Arduino Uno
- HW-184 CAN module (MCP2515 + TJA1050)
- 1x button (state machine), 1x LED (state indicator)
- 3x 10k potentiometers (simulating pack voltage, current, temperature sensors)
- 2x 120 ohm termination resistors, breadboard, wires, USB power

## Wiring Notes

- HW-184 uses SPI: SCK/MISO/MOSI plus CS and INT pins.
- CANH/CANL go to the bus; terminate only at the two ends.

---

## Feature 1: Battery Simulation

- **State:** Battery capacity (0–100 %).
- **Drain:** Each loop, battery drains by (power_request_W × dt_ms / capacity_Wh).
- **Logic:** Capacity never goes below 0; at 0 %, force state to ERROR.
- **Initialization:** Starts at 80 % for testing.

---

## Feature 2: Power Limiting Algorithm

- **Input:** Battery capacity, power request from VCU (via CAN).
- **Rate Check:** Compute drain rate (W) and predict what happens next second.
- **Limit Logic:**
  - If drain rate would drop battery < 0 % in next second, reduce power limit.
  - Start limit at battery_capacity_Wh × 3600 / max_discharge_time_s.
  - Adjust down if rate is too high.
- **Output:** Power limit (watts) sent back to VCU via CAN.

---

## Feature 3: State Machine (Button-Cycled)

- **States:** IDLE → READY_TO_DRIVE → ERROR (back to IDLE on button press)
- **Behavior:**
  - IDLE: battery can charge (simulate as offset per loop).
  - READY_TO_DRIVE: active state; battery drains per power request.
  - ERROR: VCU must wait for BMS to allow READY again.

---

## Feature 4: CAN Communication

- **TX ID 0x100** at 10 Hz
  - Byte0: BMS state (0=IDLE, 1=READY, 2=ERROR)
  - Byte1-2: battery SoC % (uint16, 0.1 % units)
  - Byte3-4: power limit (uint16, 1 W units)
  - Byte5-6: pack voltage (uint16, 0.1 V units) — from sensor simulation
  - Byte7: pack temperature (uint8, 1 °C units) — from sensor simulation
- **RX ID 0x200** at 20 Hz (from VCU) — BMS monitors for heartbeat
  - Byte4-5: power request (uint16, 1 W)

---

## Feature 5: Sensor Simulation & Fault Detection

- **Inputs:** 3x analog pots representing pack voltage, current, and temperature.
- **Validation Ranges:**
  - Pack voltage: 10–15 V (map 0–5V ADC to 0–20V).
  - Pack current: 0–50 A (map 0–5V ADC to 0–100A).
  - Pack temperature: 0–60 °C (map 0–5V ADC to 0–100°C).
- **Logic:** If any sensor out of range, force BMS into ERROR state immediately.
- **Output:** Sensor values sent in CAN frame for logging.

---

## Feature 6: CAN Watchdog (Heartbeat Timeout)

- **Monitor:** Expect CAN frame from VCU (ID 0x200) at least every 50 ms.
- **Logic:** If no 0x200 frame received for > 50 ms, assume VCU link lost.
- **Action:** Force BMS into ERROR state (loss of comms = fault).
- **Recovery:** BMS stays in ERROR until button pressed and VCU resumes sending.

---
