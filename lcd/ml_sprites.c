#include "ml_sprites.h"

void free_sprite(sprite *spr)
{
	free(spr->pixels);
	free(spr);
}

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

sprite *botleft(uint8_t team)
{
	/* Allocate sprite */
	sprite *new_sprite = malloc(sizeof(sprite));
	if(new_sprite == NULL)
		return NULL;

	uint16_t dark, med, light; 
	/* Set the light and dark colour  */
	switch(team)
	{
		case 0:
			dark = MIDNIGHT_BLUE;
			med = ROYAL_BLUE;
			light = CORNFLOWER_BLUE;
			break;
		case 1:
			dark = DARK_RED;
			med = RED;
			light = ORANGE;
			break;
		case 2:
			dark = GOLDENROD;
			med = YELLOW;
			light = LIGHT_GOLDENROD;
			break;
		case 3:
			dark = FOREST_GREEN;
			med = LIME_GREEN;
			light = GREEN_YELLOW;
			break;
		default:
			dark = DIM_GRAY;
			med = DARK_GRAY;
			light = SILVER;
			break;
	}

	/* Allocate pixel data */
	new_sprite->pixels = malloc(128 * sizeof(uint16_t));
	if(new_sprite->pixels == NULL)
	{
		free(new_sprite);
		return NULL;
	}

	uint16_t temp[] = {
	BLACK,	BLACK,		light,		light,		light,		BLACK,		BLACK,	BLACK,
	BLACK,	light,		med,		med,		light,		BLACK,		BLACK,	BLACK,
	BLACK,	WHITE,		med,		WHITE,		LIGHT_GREY,	LIGHT_GREY,	BLACK,	BLACK,
	BLACK,	WHITE,		med,		WHITE,		LIGHT_GREY,	LIGHT_GREY,	BLACK,	BLACK,
	BLACK,	light,		med,		med,		DIM_GREY,	BLACK,		BLACK,	BLACK,
	BLACK,	BLACK,		LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	BLACK,	BLACK,
	BLACK,	LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	DIM_GREY,	DARK_GREY,	BLACK,	BLACK,
	BLACK,	LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	DIM_GREY,	DARK_GREY,	BLACK,	BLACK,
	BLACK,	LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	DARK_GREY,	DARK_GREY,	BLACK,	BLACK,
	dark,	LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	DARK_GREY,	BLACK,		BLACK,	dark,
	dark,	LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	DARK_GREY,	BLACK,		dark,	dark,
	dark,	dark,		LIGHT_GREY,	LIGHT_GREY,	dark,		BLACK,		SILVER,	dark,
	SILVER,	SILVER,		dark,		dark,		dark,		BLACK,		BLACK,	BLACK,
	BLACK,	BLACK,		dark,		dark,		BLACK,		BLACK,		BLACK,	BLACK,
	BLACK, 	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,	BLACK,
	BLACK,	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,	BLACK
	};
	memcpy(new_sprite->pixels, temp, sizeof(temp));

	new_sprite->trans_colour = BLACK;
	
	new_sprite->width = 8;
	new_sprite->height = 16;

	return new_sprite;

}

sprite *botright(uint8_t team)
{
	/* Allocate sprite */
	sprite *new_sprite = malloc(sizeof(sprite));
	if(new_sprite == NULL)
		return NULL;

	uint16_t dark, med, light; 
	/* Set the light and dark colour  */
	switch(team)
	{
		case 0:
			dark = MIDNIGHT_BLUE;
			med = ROYAL_BLUE;
			light = CORNFLOWER_BLUE;
			break;
		case 1:
			dark = DARK_RED;
			med = RED;
			light = ORANGE;
			break;
		case 2:
			dark = GOLDENROD;
			med = YELLOW;
			light = LIGHT_GOLDENROD;
			break;
		case 3:
			dark = FOREST_GREEN;
			med = LIME_GREEN;
			light = GREEN_YELLOW;
			break;
		default:
			dark = DIM_GRAY;
			med = DARK_GRAY;
			light = SILVER;
			break;
	}

	/* Allocate pixel data */
	new_sprite->pixels = malloc(128 * sizeof(uint16_t));
	if(new_sprite->pixels == NULL)
	{
		free(new_sprite);
		return NULL;
	}

	uint16_t temp[] = {
	BLACK,	BLACK,		BLACK,		light,		light,		light,		BLACK,	BLACK,
	BLACK,	BLACK,		BLACK,		light,		med,		med,		light,	BLACK,
	BLACK,	BLACK,		LIGHT_GREY,	LIGHT_GREY,	WHITE,		med,		WHITE,	BLACK,
	BLACK,	BLACK,		LIGHT_GREY,	LIGHT_GREY,	WHITE,		med,		WHITE,	BLACK,
	BLACK,	BLACK,		BLACK,		DIM_GREY,	med,		med,		light,	BLACK,
	BLACK,	dark,		dark,		LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	BLACK,	BLACK,
	BLACK,	dark,		dark,		DIM_GREY,	LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	BLACK,
	SILVER,	dark,		DARK_GREY,	DIM_GREY,	LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	BLACK,
	dark,	dark,		DARK_GREY,	DARK_GREY,	LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	BLACK,
	dark,	dark,		BLACK,		DARK_GREY,	LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	dark,
	dark,	dark,		BLACK,		DARK_GREY,	LIGHT_GREY,	LIGHT_GREY,	LIGHT_GREY,	dark,
	SILVER,	dark,		BLACK,		dark,		LIGHT_GREY,	LIGHT_GREY,	BLACK,	BLACK,
	SILVER,	SILVER,		BLACK,		dark,		dark,		dark,		BLACK,	BLACK,
	BLACK,	BLACK,		BLACK,		BLACK,		dark,		dark,		BLACK,	BLACK,
	BLACK, 	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,	BLACK,
	BLACK,	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,	BLACK
	};
	memcpy(new_sprite->pixels, temp, sizeof(temp));

	new_sprite->trans_colour = BLACK;
	
	new_sprite->width = 8;
	new_sprite->height = 16;

	return new_sprite;

}
