#ifndef BB_SPRITES
#define BB_SPRITES

#include "svgrgb565.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	uint16_t *pixels;
	uint16_t trans_colour;
	uint8_t width, height;
} sprite;

void free_sprite(sprite *spr);
sprite *reticule(uint8_t team);
sprite *botleft(uint8_t team);
sprite *botright(uint8_t team);

#endif
