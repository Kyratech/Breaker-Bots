#include "ballistics_levelgen.h"
#include "lcd.h"

uint8_t *generate_flat(uint16_t width, uint8_t height)
{
	uint8_t *level_map = malloc(width * sizeof(uint8_t));
	if(level_map == NULL)
	{
		display_string("Not enough memory");
		return NULL;
	}

	uint16_t i;
	for(i = 0; i < width; i++)
	{
		level_map[i] = height;
	}
	return level_map;
}
