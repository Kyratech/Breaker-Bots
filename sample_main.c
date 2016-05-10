/* Ballistic Breaker Bots */

#include "os.h"
#include "ml_filesystem.h"
#include "ml_maths.h"
#include "ballistics_UI.h"
#include "ballistics_levelgen.h"
#include "ballistics_titlescreen.h"
#include <stdlib.h>

#define VERSION "v1.0.2"

#define HEIGHT 240
#define WIDTH 320
#define HEIGHT_NO_UI 225
#define PLAYER_WIDTH 4		//Actually width / 2 to save division later
#define PLAYER_HEIGHT 8		//Ditto, is height / 2

#define MAX_PLAYER_HP 100
#define RETICULE_DISTANCE 3	//Distance from player centre
#define MAX_PROJECTILE_POWER 20	
#define EXPLOSION_RADIUS 20	//Size of crater
#define BLAST_RADIUS 30		//Area of effect (in which players lose HP)
#define MAX_EXPLOSION_DAMAGE 50
#define MAX_CLIMB_HEIGHT 20	//Max pixels the player can climb

uint8_t *level_map;

int run_game();
int update_dial(int);
int collect_delta(int);
int check_switches(int);
void start_turn();
void draw_HP_UI();

sprite *reticule_SPR;
sprite *player_SPR;

uint8_t current_player;
uint8_t players = 2;
uint8_t game_state;				
volatile int16_t *playersX;
volatile int16_t *playersY;
uint8_t *players_HP;
int16_t reticuleX, reticuleY;

int8_t direction;
int projectileX, projectileY;
int proVelX, proVelY;
int launch_speed;

int8_t wind_velocity;					/* 0-9 LEFT, 10 NONE, 11-20 RIGHT */

FIL File;  						/* FAT File */

int position = 0;

/*
 * Excuse lousy commenting, but I've already lost nearly all of it to a git cockup.
 * 
 * Actually, no commenting because life's too short to do it all again and I just want to cry myself to sleep.
 */
void start_menu()
{
	game_state = 4;

	clear_screen();	
	draw_titlescreen(&FatFs);
	display_color(CRIMSON, BLACK);
	ml_printf_at("%s", 135, 85, VERSION);
	display_color(WHITE, BLACK);
    	ml_printf_at("CENTRE to start", 5, 105);
    	ml_printf_at("< %u Players >", 5, 115, players);
}

void end_game()
{
	game_state = 5;

	free((void*)playersX);
	free((void*)playersY);
	free((void*)players_HP);
	
	ml_printf_at("CENTRE to end game", 120, 120);
}

void start_game()
{
	/* Seed the RNG */
	srand(4);

	/* Load in the sprites */
	reticule_SPR = reticule(0);

	/* Generate and draw a level */
	clear_screen();
	free((void*) level_map);
	level_map = generate_flat(WIDTH, 129);	
	if(level_map != NULL)
		draw_level(level_map, SILVER, 0, WIDTH - 1);
	
	/* Set up the players in the field */
	playersX = malloc(players * sizeof(int16_t));
	playersY = malloc(players * sizeof(int16_t));
	players_HP = malloc(players * sizeof(uint8_t));
	if(playersX == NULL || playersY == NULL || players_HP == NULL)
	{
		display_string("Out of memory error!");
		return;
	}
	uint8_t i;
	for(i = 0; i < players; i++)
	{
		playersX[i] = (i + 1) * WIDTH / (players + 1);
		playersY[i] = 120;
		players_HP[i] = MAX_PLAYER_HP;

		free_sprite(player_SPR);
		player_SPR = botleft(i);
		fill_sprite(player_SPR, playersX[i], playersY[i]);
	}
	current_player = 0;
	reticuleX = (playersX[current_player] + (RETICULE_DISTANCE * ml_cos(position))/100);
	reticuleY = (playersY[current_player] + (RETICULE_DISTANCE * ml_sin(position))/100);



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
	/* If only one player remains, declare them winner. */
	uint8_t i;
	uint8_t player_left; //Record last player left (meaningless if more than 1 survivor)
	uint8_t players_alive = 0;
	for(i = 0; i < players; i++)
	{
		if(players_HP[i] != 0)
		{
			players_alive++;
			player_left = i;
		}
	}
	if(players_alive == 1)
	{
		switch(player_left)
		{
			case 0:
				display_color(BLUE, BLACK);
				ml_printf_at("To the victor, the spoils! BLUE WINS!", 5, 20);
				break;
			case 1:
				display_color(RED, BLACK);
				ml_printf_at("And the win goes to RED! Congratulations!", 5, 20);
				break;
			case 2:
				display_color(YELLOW, BLACK);
				ml_printf_at("Who's the best? YELLOW's the best!", 5, 20);
				break;
			default:
				display_color(GREEN, BLACK);
				ml_printf_at("Looks like the others are GREEN with envy! (GREEN WINS)", 5, 20);
				break;
		}

		end_game();
		return;
	}
	else if(players_alive == 0)
	{
		display_color(WHITE, BLACK);
		ml_printf_at("Oh dear, it looks like a DRAW!", 5, 20);

		end_game();
		return;
	}

	/* Rotate through the players */
	current_player = (current_player + 1) % players;
	while(players_HP[current_player] == 0)
	{
		current_player = (current_player + 1) % players;
	}

	free_sprite(reticule_SPR);
	reticule_SPR = reticule(current_player);

	/* Change to movement phase */
	game_state = 1;

	/* Update the HP UI */
	draw_HP_UI();
	
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

void draw_HP_UI()
{
	rectangle UI_rec = {0, WIDTH, 0, 15};
	fill_rectangle(UI_rec, BLACK);

	display_color(BLUE, BLACK);
	ml_printf_at("BLUE: %u", 5, 5, players_HP[0]);
	display_color(RED, BLACK);
	ml_printf_at("RED: %u", 85, 5, players_HP[1]);
	if(players >= 3)
	{
		display_color(YELLOW, BLACK);
	        ml_printf_at("YELLOW: %u", 165, 5, players_HP[2]);
		if(players == 4)
		{
			display_color(GREEN, BLACK);
		        ml_printf_at("GREEN: %u", 245, 5, players_HP[3]);
		}
	}
}

void main(void) {
    os_init();

    os_add_task( run_game,            30, 1);
    os_add_task( collect_delta,   60, 1);
    //os_add_task( check_switches,  60, 1);

    start_menu();

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
	
	projectileX = playersX[current_player];
	projectileY = playersY[current_player];

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
		int16_t newRX = (playersX[current_player] + (RETICULE_DISTANCE * ml_cos(position))/100);
		int16_t newRY = (playersY[current_player] + (RETICULE_DISTANCE * ml_sin(position))/100);
		
		rectangle reticuleOld = {reticuleX - reticule_SPR->width / 2, reticuleX + reticule_SPR->width / 2 - 1, reticuleY - reticule_SPR->height / 2, reticuleY + reticule_SPR->height / 2 - 1};
		draw_background(level_map, SILVER, reticuleOld);

		//fill_rectangle(reticuleOld, BLACK);
		if(newRX >= 0 && newRX < WIDTH && newRY >= 0 && newRY < HEIGHT)
			fill_sprite(reticule_SPR, newRX, newRY);
	
		reticuleX = newRX;
		reticuleY = newRY;

		if (get_switch_long(_BV(SWC)))
		{
			game_state = 3;
			return 0;
		}		

		/* Read directional input. Gonna move this later. */
		if (get_switch_rpt(_BV(SWE))) 
		{
			direction = 4;
			free_sprite(player_SPR);
			player_SPR = botright(current_player);
		}
		else if (get_switch_rpt(_BV(SWW)))
		{
			direction = -4;
			free_sprite(player_SPR);
			player_SPR = botleft(current_player);
		}
		else
		{
			direction = 0;
			return 0;
		}

		/* Move the player */
		int16_t newX = playersX[current_player] + direction;
		int16_t newY = ml_min(level_map[newX - PLAYER_WIDTH], level_map[newX + PLAYER_WIDTH - 1]) - PLAYER_HEIGHT;

		int8_t i;
		int8_t player_collision = 0;
		for(i = 0; i < players; i++)
		{
			if(players_HP[i] != 0 && i != current_player && newX >= playersX[i] - PLAYER_WIDTH && newX <= playersX[i] + PLAYER_WIDTH - 1)
			{
				player_collision = 1;
				break;
			}				
		}

		/* Cancel movement if trying to climb to high or off the sides of the screen */
		if(player_collision || playersY[current_player] - newY > MAX_CLIMB_HEIGHT || newX <= PLAYER_WIDTH || newX + PLAYER_WIDTH > WIDTH)
		{
			//display_string_xy("Error\n", 10, 10);
			//ml_printf("Cannot move there... (%u,%u) to (%u,%u)", playersX[0], playersY[0], newX, newY);
			return 0;
		}
		
		rectangle playerOld = {playersX[current_player] - PLAYER_WIDTH, playersX[current_player] + PLAYER_WIDTH - 1, playersY[current_player] - PLAYER_HEIGHT, playersY[current_player] + PLAYER_HEIGHT - 1};
		//rectangle playerNew = {newX - PLAYER_WIDTH, newX + PLAYER_WIDTH - 1, newY - PLAYER_HEIGHT, newY + PLAYER_HEIGHT - 1};

		fill_rectangle(playerOld, BLACK);
		fill_sprite(player_SPR, newX, newY);
		//fill_rectangle(playerNew, BLUE);
		
		playersX[current_player] = newX;
		playersY[current_player] = newY;
	}
	/* Projectile phase (missile in the air) */
	else if(game_state == 2)
	{
		/* Find the new position of the projectile */
		int16_t newX = projectileX + proVelX/1000;
		int16_t newY = projectileY + proVelY/1000;
		
		rectangle projectileOld = {projectileX, projectileX, projectileY, projectileY};
		fill_rectangle(projectileOld, BLACK);

		/* End turn if projectile goes off the sides of the level */
		if(newX < 0 || newX > WIDTH)
		{
			start_turn();
			return 0;
		}
		
		/* If the new position is in the ground, EXPLODE!
		 * (Also, if off the bottom of the level) */
		if(newY >= level_map[newX] || newY > HEIGHT_NO_UI)
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
			//rectangle player = {blueX - PLAYER_WIDTH, blueX + PLAYER_WIDTH - 1, blueY - PLAYER_HEIGHT, blueY + PLAYER_HEIGHT - 1};
			//fill_rectangle(player, BLUE);

			/* Check players for damage + redraw them */
			for(i = 0; i < players; i++)
			{
				int16_t deltaX = playersX[i] - newX;
				int16_t deltaY = playersY[i] - newY;
				int16_t distance = ml_sqrt(deltaX * deltaX + deltaY * deltaY);
				if(distance < BLAST_RADIUS)
				{
					int16_t damage = ml_clamp(0, players_HP[i], MAX_EXPLOSION_DAMAGE - (distance * MAX_EXPLOSION_DAMAGE / BLAST_RADIUS));
					players_HP[i] -= damage;
				}

				rectangle playerOld = {playersX[i] - PLAYER_WIDTH, playersX[i] + PLAYER_WIDTH - 1, playersY[i] - PLAYER_HEIGHT, playersY[i] + PLAYER_HEIGHT - 1};
				fill_rectangle(playerOld, BLACK);

				/* If the player is still alive, redraw them */
				if(players_HP[i] != 0)
				{
					free_sprite(player_SPR);
					player_SPR = botleft(i);
					playersY[i] = ml_min(level_map[playersX[i] - PLAYER_WIDTH], level_map[playersX[i] + PLAYER_WIDTH - 1]) - PLAYER_HEIGHT;
					fill_sprite(player_SPR, playersX[i], playersY[i]);
				}
			}

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
	/* Menu phase */
	else if(game_state == 4)
	{
		if (get_switch_press(_BV(SWE)))
		{
			if(players == 4)
				players = 2;
			else
				players++;
		}
		else if (get_switch_press(_BV(SWW)))
		{
			if(players == 2)
				players = 4;
			else
				players--;
		}

		ml_printf_at("< %u Players >", 5, 115, players);
		
		if (get_switch_long(_BV(SWC)))
		{
			start_game();
		}
	}
	/* End game phase */
	else if(game_state == 5)
	{
		if (get_switch_long(_BV(SWC)))
		{
			start_menu();
		}
	}

	sei();
	
	return 0;
}

/*
int check_switches(int state) {
	
	if (get_switch_press(_BV(SWN))) {
	}
		
	if (get_switch_press(_BV(SWE))) {
	}
		
	if (get_switch_press(_BV(SWS))) {
	}
		
	if (get_switch_press(_BV(SWW))) {
	}
		
	if (get_switch_long(_BV(SWC))){
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
*/

