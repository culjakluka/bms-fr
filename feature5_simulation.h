#ifndef FEATURE5_SIMULATION_H
#define FEATURE5_SIMULATION_H

#include <stdint.h>
#include <stdbool.h>

//Provjeri je li očitana vrijednost unutar granica
bool in_range(int value, int minVal, int maxVal);

//Provjeri sve 3 vrijednosti i pošalji potvrdu ako su sve tri unutar granica
bool check_ranges(float voltage,float current, float temp);

#endif