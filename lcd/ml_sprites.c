#include "ml_sprites.h"

sprite *reticule(uint8_t team)
{
	/* Allocate sprite */
	sprite *new_sprite = malloc(sizeof(sprite));
	if(new_sprite == NULL)
		return NULL;

	uint16_t dark, light; 
	/* Set the light and dark colour  */
	switch(team)
	{
		case 0:
			dark = ROYAL_BLUE;
			light = CORNFLOWER_BLUE;
			break;
		case 1:
			dark = RED;
			light = ORANGE;
			break;
		case 2:
			dark = YELLOW;
			light = LIGHT_GOLDENROD;
			break;
		case 3:
			dark = LIME_GREEN;
			light = GREEN_YELLOW;
			break;
		default:
			dark = DARK_GRAY;
			light = SILVER;
			break;
	}

	/* Allocate pixel data */
	new_sprite->pixels = malloc(64 * sizeof(uint16_t));
	if(new_sprite->pixels == NULL)
	{
		free(new_sprite);
		return NULL;
	}

	uint16_t temp[] = {
	BLACK,	BLACK,	BLACK,	dark,	BLACK,	BLACK,	BLACK,	BLACK,
	BLACK,	BLACK,	dark,	dark,	dark,	BLACK,	BLACK,	BLACK,
	BLACK,	dark,	BLACK,	light,	BLACK,	dark,	BLACK,	BLACK,
	dark,	dark,	light,	BLACK,	light,	dark,	dark,	BLACK,
	BLACK,	dark,	BLACK,	light,	BLACK,	dark,	BLACK,	BLACK,
	BLACK,	BLACK,	dark,	dark,	dark,	BLACK,	BLACK,	BLACK,
	BLACK,	BLACK,	BLACK,	dark,	BLACK,	BLACK,	BLACK,	BLACK,
	BLACK, 	BLACK,	BLACK,	BLACK,	BLACK,	BLACK,	BLACK,	BLACK
	};
	memcpy(new_sprite->pixels, temp, sizeof(temp));

	new_sprite->trans_colour = BLACK;
	
	new_sprite->width = 8;
	new_sprite->height = 8;

	return new_sprite;
}
