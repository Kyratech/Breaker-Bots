#ifndef ML_PRINT
#define ML_PRINT

#include <stdint.h>

void ml_printf(char *str, ...);
void ml_printf_at(char *str, uint16_t x, uint16_t y, ...);

#endif
