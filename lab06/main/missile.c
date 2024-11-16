#include "missile.h"
#include "config.h"
#include <stdlib.h>
#include <math.h>

// State machine states for missile control.
enum missileControl_st_t {
	init_st,                // Start here, transition out of this state on the first tick.
	moving_st,              // The missile is moving.
    exploding_growing_st,   // The missile is exploding and growing.
    exploding_shrinking_st, // The missile is exploding and shrinking.
    impacted_st,            // The missile has hit the ground.
    idle_st                 // Wait for next state.
};

// Determine measurement positions for the missile origin based on the screen size.
#define LCD_H_FIFTH (LCD_H / 5)
#define LCD_W_THREE_EIGTH (3 * LCD_W) / 8
#define LCD_W_QUARTER (LCD_W / 4)
#define LCD_W_HALF (LCD_W / 2)
#define LCD_W_FIVE_EIGTH (5 * LCD_W) / 8
#define LCD_W_THREE_QUARTER (3 * LCD_W / 4)

// Initialize the missile as an idle missile. If initialized to the idle
// state, a missile doesn't appear nor does it move.
void missile_init_idle(missile_t *missile){
    missile->currentState = idle_st;
}

// Initialize the missile as a player missile. This function takes an (x, y)
// destination of the missile (as specified by the user). The origin is the
// closest "firing location" to the destination (there are three firing
// locations evenly spaced along the bottom of the screen).
void missile_init_player(missile_t *missile, coord_t x_dest, coord_t y_dest){
    missile->type = MISSILE_TYPE_PLAYER;
    missile->x_dest = x_dest;
    missile->y_dest = y_dest;

    // Determine where the destination is relative to the bottom of the screen.
    // Depending on the destination, the origin will be one of three launch locations.
    if(missile->x_dest >= 0 && missile->x_dest < LCD_W_THREE_EIGTH){
        missile->x_origin = LCD_W_QUARTER;
    } else if(missile->x_dest >= LCD_W_THREE_EIGTH && missile->x_dest < LCD_W_FIVE_EIGTH){
        missile->x_origin = LCD_W_HALF;
    } else if(missile->x_dest >= LCD_W_FIVE_EIGTH && missile->x_dest < LCD_W){
        missile->x_origin = LCD_W_THREE_QUARTER;
    }
    missile->y_origin = LCD_H;
    missile_init_helper(missile);
}

// Initialize the missile as an enemy missile. This will randomly choose the
// origin and destination of the missile. The origin is somewhere near the
// top of the screen, and the destination is the very bottom of the screen.
void missile_init_enemy(missile_t *missile){
    missile->type = MISSILE_TYPE_ENEMY;
    missile->x_origin = rand() % LCD_W;
    missile->y_origin = rand() % LCD_H_FIFTH;
    missile->x_dest = rand() % LCD_W;
    missile->y_dest = LCD_H;
    missile_init_helper(missile);
}

// Initialize the missile as a plane missile. This function takes the (x, y)
// location of the plane as an argument and uses it as the missile origin.
// The destination is randomly chosen along the bottom of the screen.
void missile_init_plane(missile_t *missile, coord_t x_orig, coord_t y_orig){
    missile->type = MISSILE_TYPE_PLANE;
    missile->x_origin = x_orig;
    missile->y_origin = y_orig;
    missile->x_dest = rand() % LCD_W;
    missile->y_dest = LCD_H;
    missile_init_helper(missile);
}

// Initialize the missile with the generic missile structures. This function
// is called by the other missile initialization functions to set generic initial
// values each type of missile uses.
void missile_init_helper(missile_t *missile){
    missile->currentState = init_st;
    missile->length = 0;
    missile->radius = 0;
    missile->explode_me = false;
    missile->total_length = sqrtf((missile->x_dest - missile->x_origin) * (missile->x_dest - missile->x_origin) + 
            (missile->y_dest - missile->y_origin) * (missile->y_dest - missile->y_origin));
    missile->x_current = missile->x_origin;
    missile->y_current = missile->y_origin;
}

/******************** Missile Control & Tick Functions ********************/

// Used to indicate that a moving missile should be detonated. This occurs
// when an enemy or a plane missile is located within an explosion zone.
void missile_explode(missile_t *missile){
    missile->explode_me = true;
}

// Tick the state machine for a single missile.
void missile_tick(missile_t *missile){
    // Switch statement to control the transitions for each state of the missile.
    switch(missile->currentState){
        // Transition out of the init state on the first tick to the moving state.
        case init_st:
            missile->currentState = moving_st;
            break;
        // If the missile is exploding, transition to the growing state. If the missile
        // has reached its destination, transition to the impacted state.
        case moving_st:
            // Determine if the missile length has reached the total length of the missile and if the missile is a player missile. If so, transition to the exploding growing state.
            // If not a player missile, check if the missile has an explode me flag set. If so, transition to the exploding growing state.
            if(((missile->length >= missile->total_length) && missile->type == MISSILE_TYPE_PLAYER) || missile->explode_me){
                missile->currentState = exploding_growing_st;
                lcd_drawLine(missile->x_origin, missile->y_origin, missile->x_dest, missile->y_dest, CONFIG_COLOR_BACKGROUND);
            } else if((missile->length >= missile->total_length)){
                missile->currentState = impacted_st;
            } else {
                missile->currentState = moving_st;
            }
            break;
        // If the missile is exploding and growing, and hits the radius maximum, transition
        // to the shrinking state. Otherwise, stay in the growing state.
        case exploding_growing_st:
            // Check if the missile radius has reached the maximum explosion radius. If so, transition to the exploding shrinking state.
            if(missile->radius >= CONFIG_EXPLOSION_MAX_RADIUS){
                missile->currentState = exploding_shrinking_st;
            } else {
                missile->currentState = exploding_growing_st;
            }
            break;
        // If the missile is exploding and shrinking, and the radius is zero, transition
        // to the idle state. Otherwise, stay in the shrinking stateErase the missile line and wait..
        case exploding_shrinking_st:
            // Check if the missile radius has reached zero. If so, transition to the idle state.
            if(missile->radius <= 0){
                missile->currentState = idle_st;
            } else {
                missile->currentState = exploding_shrinking_st;
            }
            break;
        // The missile has reached the ground. Transition to the idle state.
        case impacted_st:
            missile->currentState = idle_st;
            lcd_drawLine(missile->x_origin, missile->y_origin, missile->x_dest, missile->y_dest, CONFIG_COLOR_BACKGROUND);
            break;
        // The missile is idle (it is no longer moving). Stay in the idle state.
        case idle_st:
            missile->currentState = idle_st;
            break;
        // In case things go horribly wrong, restart the state.
        default:
            break;
    }

    // Switch statement to control the actions for each state of the missile.
    switch(missile->currentState){
        // No actions are taken in the init state.
        case init_st:
            break;
        // Update the missile length, current location, and draw a new line to represent the missile.
        case moving_st:
            // Clear the missile line before redrawing it.
            lcd_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, CONFIG_COLOR_BACKGROUND);
            
            // Store the color of the missile based on the missile type.
            color_t color;

            // Determine which type of missile is moving and update the length according to
            // the missile type's speed. Update a color value based on the missile type.
            if(missile->type == MISSILE_TYPE_PLAYER){
                missile->length += CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK;
                color = CONFIG_COLOR_PLAYER_MISSILE;
            } else if(missile->type == MISSILE_TYPE_ENEMY){
                missile->length += CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
                color = CONFIG_COLOR_ENEMY_MISSILE;
            } else {
                missile->length += CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
                color = CONFIG_COLOR_PLANE_MISSILE;
            }

            float fraction = 0;
            fraction = missile->length / missile->total_length;
            missile->x_current = missile->x_origin + fraction * (missile->x_dest - missile->x_origin);
            missile->y_current = missile->y_origin + fraction * (missile->y_dest - missile->y_origin);
            lcd_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, color);

            break;
        // Incrase the missile's explosion radius and draw a new circle to represent the explosion.
        case exploding_growing_st:
            // Erase the previous circle representing the missile explosion.
            lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, CONFIG_COLOR_BACKGROUND);

            // Increase the radius of the missile explosion 
            missile->radius += CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;

            // Draw a circle to represent the updated radius based on the color of the missile type exploding.
            if(missile->type == MISSILE_TYPE_PLAYER){
                lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, CONFIG_COLOR_PLAYER_MISSILE);
            } else if(missile->type == MISSILE_TYPE_ENEMY){
                lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, CONFIG_COLOR_ENEMY_MISSILE);
            } else if(missile->type == MISSILE_TYPE_PLANE){
                lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, CONFIG_COLOR_PLANE_MISSILE);
            }
            break;
        // Decrease the missile's explosion radius and draw a new circle to represent the explosion.
        case exploding_shrinking_st:
            // Erase the previous circle representing the missile explosion.
            lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, CONFIG_COLOR_BACKGROUND);

            // Decrease the radius of the missile explosion
            missile->radius -= CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;

            // Draw a circle to represent the updated radius based on the color of the missile type exploding.
            if(missile->type == MISSILE_TYPE_PLAYER){
                lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, CONFIG_COLOR_PLAYER_MISSILE);
            } else if(missile->type == MISSILE_TYPE_ENEMY){
                lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, CONFIG_COLOR_ENEMY_MISSILE);
            } else {
                lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, CONFIG_COLOR_PLANE_MISSILE);
            }
            break;
        // Do nothing (for now).
        case impacted_st:
            break;
        // Do nothing, just vibe.
        case idle_st:
            break;
        // In case things go horribly wrong, restart the state.
        default:
            break;
    }
}

/******************** Missile Status Functions ********************/

// Return the current missile position through the pointers *x,*y.
void missile_get_pos(missile_t *missile, coord_t *x, coord_t *y){
    *x = missile->x_current;
    *y = missile->y_current;
}

// Return the missile type.
missile_type_t missile_get_type(missile_t *missile){
    return missile->type;
}

// Return whether the given missile is moving.
bool missile_is_moving(missile_t *missile){
    // Check if the missile is in the moving state. If it is, return true.
    if(missile->currentState == moving_st){
        return true;
    } else {
        return false;
    }
}

// Return whether the given missile is exploding. If this missile
// is exploding, it can explode another intersecting missile.
bool missile_is_exploding(missile_t *missile){
    // Check if the missile is in the exploding growing or shrinking state. If it is, return true.
    if(missile->currentState == exploding_growing_st || missile->currentState == exploding_shrinking_st){
        return true;
    } else {
        return false;
    }
}

// Return whether the given missile is idle.
bool missile_is_idle(missile_t *missile){
    // Check if the missile is in the idle state. If it is, return true.
    if(missile->currentState == idle_st){
        return true;
    } else {
        return false;
    }
}

// Return whether the given missile is impacted.
bool missile_is_impacted(missile_t *missile){
    // Check if the missile is in the impacted state. If it is, return true.
    if(missile->currentState == impacted_st){
        return true;
    } else {
        return false;
    }
}


// Return whether an object (e.g., missile or plane) at the specified
// (x,y) position is colliding with the given missile. For a collision
// to occur, the missile needs to be exploding and the specified
// position needs to be within the explosion radius.
bool missile_is_colliding(missile_t *missile, coord_t x, coord_t y){
    // Check if the missile is in the exploding growing or shrinking state. If it is, check if the
    // specified position is within the explosion radius. If it is, return true.
    if(missile->currentState == exploding_growing_st || missile->currentState == exploding_shrinking_st){
        // Check if the specified position is within the explosion radius of the missile.
        if((((missile->y_current-y)*(missile->y_current-y)) + ((missile->x_current-x)*(missile->x_current-x))) <= ((missile->radius) * (missile->radius))){
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}