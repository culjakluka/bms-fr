#include "Arduino.h"
#include "feature5_simulation.h"
#include "config.h"

bool in_range(int value, int minVal, int maxVal) {
  return (value >= minVal && value <= maxVal);
}

bool check_ranges(float voltage,float current, float temp){
    bool voltageOK = in_range(voltage, PACK_VOLTAGE_MIN_V, PACK_VOLTAGE_MAX_V);
    bool currentOK = in_range(current, PACK_CURRENT_MIN_A, PACK_CURRENT_MAX_A);
    bool temperatureOK = in_range(temp, PACK_TEMP_MIN_C, PACK_TEMP_MAX_C);

    bool systemOK = voltageOK && currentOK && temperatureOK ? true : false;

    return systemOK;
}

