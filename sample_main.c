/* COMP2215 15/16 Task 5---SKELETON */

#include "os.h"
#include "ml_filesystem.h"
#include "ml_maths.h"
#include "ballistics_UI.h"
#include "ballistics_levelgen.h"
#include <stdlib.h>

#define HEIGHT 240
#define WIDTH 320
#define HEIGHT_NO_UI 225
#define PLAYER_WIDTH 4		//Actually width / 2 to save division later
#define PLAYER_HEIGHT 8		//Ditto, is height / 2

#define MAX_PLAYER_HP 200
#define RETICULE_DISTANCE 3	//Distance from player centre
#define MAX_PROJECTILE_POWER 20
#define EXPLOSION_RADIUS 20
#define MAX_CLIMB_HEIGHT 20	//Max pixels the player can climb

uint8_t *level_map;

int blink(int);
int run_game();
int update_dial(int);
int collect_delta(int);
int check_switches(int);
void start_turn();

sprite *reticule_SPR;

int game_state;				
int blueX, blueY;
int reticuleX, reticuleY;

int direction;
int projectileX, projectileY;
int proVelX, proVelY;
int launch_speed;

int8_t wind_velocity;					/* 0-9 LEFT, 10 NONE, 11-20 RIGHT */

FIL File;  						/* FAT File */

int position = 0;

void start_game()
{
	/* Seed the RNG */
	srand(4);

	/* Load in the sprites */
	reticule_SPR = reticule(0);

	/* Generate and draw a level */
	level_map = generate_flat(WIDTH, 129);	
	if(level_map != NULL)
		draw_level(level_map, SILVER, 0, WIDTH - 1);
	
	/* Set up the players in the field */
	blueX = 180;
	blueY = 120;
	reticuleX = (blueX + (RETICULE_DISTANCE * ml_cos(position))/100) - reticule_SPR->width/2;
	reticuleY = (blueY + (RETICULE_DISTANCE * ml_sin(position))/100) - reticule_SPR->height/2;

	/* Initialise movement variables */
	direction = 0;
	launch_speed = 0;

	/* Draw the UI */
	display_string_xy("POWER:", POWER_LABEL_X, POWER_LABEL_Y);
	fill_rectangle(power_outline, WHITE);
	fill_rectangle(power_empty, BLACK);
	display_string_xy("WIND:", WIND_LABEL_X, WIND_LABEL_Y);
	fill_rectangle(wind_outline, WHITE);
	fill_rectangle(wind_empty, BLACK);

	/* Start the first player's turn */
	start_turn();
}

void start_turn()
{
	/* Change to movement phase */
	game_state = 1;
	
	/* Reset the power bar */
	launch_speed = 0;
	fill_rectangle(power_empty, BLACK);

	/* Generate a random(ish) wind for the turn */
	wind_velocity = rand() % 21;
	
	rectangle *wind_bar = malloc(sizeof(rectangle));
	uint16_t wind_colour;
	fill_rectangle(wind_empty, BLACK);
	
	if(wind_velocity < 10)
	{
		wind_bar->left = WIND_BAR_START + wind_velocity * 10;
		wind_bar->right = WIND_BAR_END;
		wind_bar->top = 231;
		wind_bar->bottom = 236;
		wind_colour = RED;
	}
	else if (wind_velocity > 10)
	{
		wind_bar->left = WIND_BAR_START;
		wind_bar->right = WIND_BAR_START + (wind_velocity - 10) * 10;
		wind_bar->top = 231;
		wind_bar->bottom = 236;
		wind_colour = BLUE;
	}
	else
	{
		free(wind_bar);
		return;
	}
	fill_rectangle(*wind_bar, wind_colour);

	free(wind_bar);
}

void main(void) {
    os_init();

    os_add_task( run_game,            30, 1);
    os_add_task( collect_delta,   60, 1);
    os_add_task( check_switches,  60, 1);
     
    start_game();

    sei();
    for(;;){}
    
}


int collect_delta(int state) {
	position += os_enc_delta();
	if(position >= 360)
		position -= 360;
	else if(position < 0)
		position += 360;

	return state;
}

void fire_projectile()
{
	game_state = 2;
	
	projectileX = blueX;
	projectileY = blueY;

	int corrected_launch_speed = launch_speed * MAX_PROJECTILE_POWER / 100;

	proVelX = (corrected_launch_speed * ml_cos(position));
	proVelY = (corrected_launch_speed * ml_sin(position));
}


int run_game()
{
	cli();

	/* Movement phase */
	if(game_state == 1)
	{
		/* Move the reticule based on readings from the rotary encoder */
		int newRX = (blueX + (RETICULE_DISTANCE * ml_cos(position))/100);
		int newRY = (blueY + (RETICULE_DISTANCE * ml_sin(position))/100);

		rectangle reticuleOld = {reticuleX - reticule_SPR->width / 2, reticuleX + reticule_SPR->width / 2 - 1, reticuleY - reticule_SPR->height / 2, reticuleY + reticule_SPR->height / 2 - 1};
		draw_background(level_map, SILVER, reticuleOld);

		//fill_rectangle(reticuleOld, BLACK);
		fill_sprite(reticule_SPR, newRX, newRY);
	
		reticuleX = newRX;
		reticuleY = newRY;

		/* Read directional input. Gonna move this later. */
		if (get_switch_rpt(_BV(SWE))) 
		{
			direction = 4;
		}
		else if (get_switch_rpt(_BV(SWW)))
		{
			direction = -4;
		}
		else
		{
			direction = 0;
			return 0;
		}

		/* Move the player */
		int newX = blueX + direction;
		int newY = ml_min(level_map[newX - PLAYER_WIDTH], level_map[newX + PLAYER_WIDTH - 1]) - PLAYER_HEIGHT;

		/* Cancel movement if trying to climb to high */
		if(blueY - newY > MAX_CLIMB_HEIGHT)
			return 0;
		
		rectangle playerOld = {blueX - PLAYER_WIDTH, blueX + PLAYER_WIDTH - 1, blueY - PLAYER_HEIGHT, blueY + PLAYER_HEIGHT - 1};
		rectangle playerNew = {newX - PLAYER_WIDTH, newX + PLAYER_WIDTH - 1, newY - PLAYER_HEIGHT, newY + PLAYER_HEIGHT - 1};

		fill_rectangle(playerOld, BLACK);
		sprite *spr = botleft(0);
		fill_sprite(spr, newX, newY);
		free_sprite(spr);
		//fill_rectangle(playerNew, BLUE);
		
		blueX = newX;
		blueY = newY;
	}
	/* Projectile phase (missile in the air) */
	else if(game_state == 2)
	{
		/* Find the new position of the projectile */
		int newX = projectileX + proVelX/1000;
		int newY = projectileY + proVelY/1000;
		
		rectangle projectileOld = {projectileX, projectileX, projectileY, projectileY};
		fill_rectangle(projectileOld, BLACK);

		/* End turn if projectile goes off sides or bottom of level */
		if(newX < 0 || newX > WIDTH || newY > HEIGHT_NO_UI)
		{
			start_turn();
			return 0;
		}
		
		/* If the new position is in the ground, EXPLODE! */
		if(newY >= level_map[newX])
		{
			int i;
			for(i = -EXPLOSION_RADIUS; i <= EXPLOSION_RADIUS; i++)
			{
				/* Make sure that terrain is on screen */
				if(newX + i >= 0 && newX + i < WIDTH)
				{
					uint16_t new_ground_level = newY + ml_sqrt(EXPLOSION_RADIUS * EXPLOSION_RADIUS - i * i);
					if(new_ground_level >= HEIGHT_NO_UI)
						level_map[newX + i] = HEIGHT_NO_UI - 1; //Clamp to bottom of level
					else if(new_ground_level > level_map[newX + i]) //Don't raise the ground level!
						level_map[newX + i] = new_ground_level; 
				}
			}

			draw_level(level_map, SILVER, newX - EXPLOSION_RADIUS, newX + EXPLOSION_RADIUS);
			rectangle player = {blueX - PLAYER_WIDTH, blueX + PLAYER_WIDTH - 1, blueY - PLAYER_HEIGHT, blueY + PLAYER_HEIGHT - 1};
			fill_rectangle(player, BLUE);

			start_turn();
			return 0;
		}	

		rectangle projectileNew = {newX, newX, newY, newY};
		
		/* Continue turn if off top of level, but only draw if projectile onscreen */
		if(newY >= 0)
		{
			fill_rectangle(projectileNew, RED);
		}

		projectileX = newX;
		projectileY = newY;		

		/* Apply gravity and wind */
		proVelY += 500;
		proVelX += (wind_velocity - 10) * 50;
	}
	/* Charging phase */
	else if(game_state == 3)
	{
		/* Increase the launch speed while the fire button is held */
		if (get_switch_state(_BV(SWC))) {
			launch_speed++;
			rectangle bar = {POWER_BAR_START, POWER_BAR_START + launch_speed, 231, 236};
			fill_rectangle(bar, WHITE);

			/* Fire once max power is reached */
			if(launch_speed == 100)
				fire_projectile();
		}
		else
		{
			fire_projectile();
		}
	}	

	sei();
	
	return 0;
}

int check_switches(int state) {
	
	if (get_switch_press(_BV(SWN))) {
	}
		
	if (get_switch_press(_BV(SWE))) {
	}
		
	if (get_switch_press(_BV(SWS))) {
	}
		
	if (get_switch_press(_BV(SWW))) {
	}
		
	if (get_switch_long(_BV(SWC)) && game_state == 1) {
		game_state = 3;
	}

	if (get_switch_short(_BV(SWC))) {
	}

	if (get_switch_rpt(_BV(SWN))) {
	}
		
	
		
	if (get_switch_rpt(_BV(SWS))) {
	}
		


	if (get_switch_rpt(SWN)) {
	}


	if (get_switch_long(_BV(OS_CD))) {
	}

	return state;	
}



/*
int blink(int state) {
	static int light = 0;
	uint8_t level;
	
	if (light < -120) {
		state = 1;
	} else if (light > 254) {
		state = -20;
	}


	Compensate somewhat for nonlinear LED 
	output and eye sensitivity:
    
	if (state > 0) {
		if (light > 40) {
			state = 2;
		}
		if (light > 100) {
			state = 5;
		}
	} else {
		if (light < 180) {
			state = -10;
		}
		if (light < 30) {
			state = -5;
		}
	}
	light += state;

	if (light < 0) {
		level = 0;
	} else if (light > 255) {
		level = 255;
	} else {
		level = light;
	}
	
	os_led_brightness(level);
	return state;
}*/

