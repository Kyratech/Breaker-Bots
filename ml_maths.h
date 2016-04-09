#include <stdlib.h>

#ifndef ML_MATHS_H
#define ML_MATHS_H

int16_t ml_sin(uint16_t x);
int16_t ml_cos(uint16_t x);
uint16_t ml_sqrt(uint16_t n);
uint16_t ml_max(uint16_t a, uint16_t b);
uint16_t ml_min(uint16_t a, uint16_t b);
int16_t ml_clamp(int16_t low, int16_t high, int16_t value);

#endif
