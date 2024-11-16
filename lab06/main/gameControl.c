
#include <stdio.h>
#include <stdlib.h> // rand

#include "hw.h"
#include "lcd.h"
#include "cursor.h"
#include "sound.h"
#include "pin.h"
#include "missile.h"
#include "plane.h"
#include "gameControl.h"
#include "config.h"

// sound support
#include "missileLaunch.h"

// M3: Define stats constants
#define STATS_BUFFER_SIZE 128
#define DISPLAY_STATS_X 50
#define DISPLAY_STATS_Y LCD_H - 10
#define VOLUME 50


// All missiles
missile_t missiles[CONFIG_MAX_TOTAL_MISSILES];

// Alias into missiles array
missile_t *enemy_missiles = missiles+0;
missile_t *player_missiles = missiles+CONFIG_MAX_ENEMY_MISSILES;
missile_t *plane_missile = missiles+CONFIG_MAX_ENEMY_MISSILES+
									CONFIG_MAX_PLAYER_MISSILES;

// Hardware Oneshot button storage
static bool pressed;
static coord_t init_x, init_y;
static uint64_t btns;
static uint32_t missile_control;

// M3: Declare stats variables
char stats[STATS_BUFFER_SIZE];
static uint32_t player_missiles_shot;
static uint32_t enemy_missiles_impacted;

// Initialize the game control logic.
// This function initializes all missiles, planes, stats, etc.
void gameControl_init(void)
{
	// Initialize enemy missiles
	for (uint32_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++)
		missile_init_enemy(enemy_missiles+i);
	// Initialize player and plane missiles
	for (uint32_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++)
		missile_init_idle(player_missiles+i);
	missile_init_idle(plane_missile);

	// Initialize oneshot driver variables
	pressed = false;
	init_x = 0;
	init_y = 0;
	btns = 0;
	missile_control = 0;


	// M3: Initialize plane
	plane_init(plane_missile);

	// M3: Initialize stats
	player_missiles_shot = 0;
	enemy_missiles_impacted = 0;

	// M3: Set sound volume
	sound_set_volume(VOLUME);

}

// Update the game control logic.
// This function calls the missile & plane tick functions, reinitializes
// idle enemy missiles, handles button presses, fires player missiles,
// detects collisions, and updates statistics.
void gameControl_tick(void)
{
	// Tick missiles in one batch
	for (uint32_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++)
		missile_tick(missiles+i);

	// Reinitialize idle enemy missiles
	for (uint32_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++){
		// Check if the enemy missile is idle. If it is, initialize the enemy missile.
		if (missile_is_idle(enemy_missiles+i)){
			missile_init_enemy(enemy_missiles+i);
		}
	}

	// M2: Check for button press. If so, launch a free player missile.
	btns = ~pin_get_in_reg() & HW_BTN_MASK;

	// Check for a button press and launch a missile if the button is pressed and the missile is idle. 
	// If the missile is not idle, launch the missile and increment the missile_control index variable.
	if (!pressed && btns) {
		pressed = true; // button pressed
		cursor_get_pos(&init_x, &init_y);
		// Check if the missile is idle and the button is pressed. If so, initialize the player missile and increment the shot counter
		if(missile_is_idle(player_missiles+missile_control) && pressed) {
			missile_init_player(player_missiles+missile_control, init_x, init_y);
			player_missiles_shot++;
			sound_start(&missileLaunch, sizeof(missileLaunch), false);
			// Check if the missile control index is less than the maximum number of player missiles. If it is, increment the missile control index.
			// Otherwise, set the missile control index to 0.
			if(missile_control < CONFIG_MAX_PLAYER_MISSILES-1) {
				missile_control++;
			} else {
				missile_control = 0;
			}
		} 
	} else if (pressed && !btns) {
		pressed = false; // all released
	}
	
	// Location variables for the plane and enemy missiles.
	coord_t loc_x, loc_y;

	// Loop through the player missiles, determine if they are exploding, and check for collisions.
	for(uint32_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) {
		// Check to see if the player missile is exploding before checking for a collision.
		// Otherwise, check for a collision with plane missiles.
		if(missile_is_exploding(player_missiles+i)) {
			// Loop through the enemy missiles and check for a collision with player missiles.
			for(uint32_t j = 0; j < CONFIG_MAX_ENEMY_MISSILES; j++) {
				// Make sure the enemy missile is moving before checking for a collision, and if it is, get the current location of the missile.
				// If the enemy missile is exploding, check for a collision with other enemy missiles.
				if(missile_is_moving(enemy_missiles+j)) {
					missile_get_pos(enemy_missiles+j, &loc_x, &loc_y);
					// Check is the enemy missile is colliding with the player missile. If it is, explode the enemy missile.
					if(missile_is_colliding(player_missiles+i, loc_x, loc_y)) {
						missile_explode(enemy_missiles+j);
					}
				} else if(missile_is_exploding(enemy_missiles+j)){
					// Loop through the enemy missiles again and check for a collision with other exploding enemy missiles.
					for(uint32_t k = 0; k < CONFIG_MAX_ENEMY_MISSILES; k++) {
						// Make sure the enemy missile is moving and not idle or exploding.
						if(missile_is_moving(enemy_missiles+k)) {
							missile_get_pos(enemy_missiles+k, &loc_x, &loc_y);
							// Check if an enemy missile is colliding with another enemy missile. If it is, explode the enemy missile.
							if(missile_is_colliding(enemy_missiles+j, loc_x, loc_y)) {
								missile_explode(enemy_missiles+k);
							}
						} 

						// Check if the enemy missile is moving.
						if(missile_is_moving(plane_missile)){
							missile_get_pos(plane_missile, &loc_x, &loc_y);
							// Check if the plane missile is moving and if it is colliding with an enemy missile. If it is, explode the enemy missile.
							if(missile_is_colliding(enemy_missiles+j, loc_x, loc_y)) {
								missile_explode(plane_missile);
							}
						}

						// Check if the plane is moving. If so, get its location.
						if(plane_is_flying()){
							plane_get_pos(&loc_x, &loc_y);
							// Check if the plane is flying and if it is colliding with an exploding enemy missile. If it is, destroy the plane.
							if(missile_is_colliding(enemy_missiles+j, loc_x, loc_y)) {
								plane_explode();
							}
						}
					}
				}
			} 
			// Check if the plane is moving. If so, get its location.
			if(plane_is_flying()){
				plane_get_pos(&loc_x, &loc_y);
				// Check if the plane is flying and if it is colliding with the exploding player missile. If it is, destroy the plane.
				if(missile_is_colliding(player_missiles+i, loc_x, loc_y)) {
					plane_explode();
				}
			} 

			// Check if the plane missile is moving. If so, get its position and check for collisions with player missiles.
			if(missile_is_moving(plane_missile)){
				missile_get_pos(plane_missile, &loc_x, &loc_y);
				// Check if the plane missile is moving and if it is colliding with an enemy missile. If it is, explode the enemy missile.
				if(missile_is_colliding(player_missiles+i, loc_x, loc_y)) {
					missile_explode(plane_missile);
				}
			}
		} 
	}

	// M3: Count non-player impacted missiles
	for(uint32_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++){
		// Check if the enemy missile is impacted. If it is, increment the enemy missile impacted counter.
		if(missile_is_impacted(enemy_missiles+i)){
			enemy_missiles_impacted++;
		}
	}
	// Check if the plane missile is impacted. If it is, increment the enemy missile impacted counter.
	if(missile_is_impacted(plane_missile)){
		enemy_missiles_impacted++;
	}

	// M3: Tick plane & draw stats
	plane_tick();
	sprintf(stats, "Shots taken: %ld.  Missiles Impacted: %ld", player_missiles_shot, enemy_missiles_impacted);
	lcd_drawString(DISPLAY_STATS_X, DISPLAY_STATS_Y, stats, CONFIG_COLOR_STATUS);
}
