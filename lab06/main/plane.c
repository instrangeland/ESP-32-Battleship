#include "plane.h"
#include "config.h"
#include <stdlib.h>

// State machine states for plane control.
enum planeControl_st_t {
    init_st,    // Start here, transition out of this state on the first tick.
    flying_st,  // The plane is flying right to left.
    inactive_st // The plane is inactive (waiting for restart).
};

// Plane control variables. 
static enum planeControl_st_t currentState;
static missile_t *missile;
static coord_t x_origin;
static coord_t y_location;
static coord_t x_dest;
static coord_t x_current;
static bool destroy_plane;
static uint32_t plane_idle_time;
static coord_t missile_launch_x;

#define LCD_H_FIFTH (LCD_H / 5)

// Initialize the plane state machine. Pass a pointer to the missile
// that will be (re)launched by the plane. It will only have one missile.
void plane_init(missile_t *plane_missile){
    currentState = init_st;
    missile = plane_missile;
    x_origin = LCD_W;
    y_location = LCD_H_FIFTH;
    x_dest = 0;
    x_current = x_origin;
    destroy_plane = false;
    plane_idle_time = 0;
    missile_launch_x = rand() % LCD_W;
}

/******************** Plane Control & Tick Functions ********************/

// Trigger the plane to explode.
void plane_explode(void){
    destroy_plane = true;
}

// State machine tick function.
void plane_tick(void){
    // Transition switch statement for plane state machine.
    switch(currentState){
        // Transition out of the init state on the first tick.
        case init_st:
            currentState = flying_st;
            break;
        // If the plane is flying, check if the plane has been destroyed or has reached the left edge of the screen. If so, transition to the inactive state.
        // Includes a Mealy action to erase the plane from the display.
        case flying_st:
            // Check if the plane has been destroyed or has reached the left edge of the screen. If so, transition to the inactive state and
            // clear the plane from the display.
            if(destroy_plane || x_current <=0){
                currentState = inactive_st;
                lcd_fillTriangle(x_current, y_location, x_current+CONFIG_PLANE_WIDTH, y_location+(CONFIG_PLANE_HEIGHT/2), 
                    x_current+CONFIG_PLANE_WIDTH, y_location-(CONFIG_PLANE_HEIGHT/2), CONFIG_COLOR_BACKGROUND);
            } else {
                currentState = flying_st;
            }
            break;
        // If the plane is inactive, check if the plane has been inactive for a certain number of ticks. If so, transition back to the flying state.
        case inactive_st:
            // Check if the plane has been inactive for a certain number of ticks. If so, transition back to the flying state. 
            // Otherwise, stay in the inactive state.
            if(plane_idle_time >= CONFIG_PLANE_IDLE_TIME_TICKS){
                currentState = flying_st;
                plane_idle_time = 0;
            } else {    
                currentState = inactive_st;
            }
            break;
        // In case things go horribly wrong, restart the state.
        default:
            break;
    }

    // Action switch statemet for plane state machine.
    switch(currentState){
        // Do nothing.
        case init_st:
            break;
        // If the plane is flying, erase the plane from the display, update the plane's x-coordinate, and redraw the plane at the new x-coordinate.
        // Check if the plane missile is idle and if the plane is at the randomly selected launch x-coordinate. 
        // If it is, launch the missile and select a new launch x-coordinate.
        case flying_st:
            lcd_fillTriangle(x_current, y_location, x_current+CONFIG_PLANE_WIDTH, y_location+(CONFIG_PLANE_HEIGHT/2), 
                x_current+CONFIG_PLANE_WIDTH, y_location-(CONFIG_PLANE_HEIGHT/2), CONFIG_COLOR_BACKGROUND);
            x_current -= CONFIG_PLANE_DISTANCE_PER_TICK;
            lcd_fillTriangle(x_current, y_location, x_current+CONFIG_PLANE_WIDTH, y_location+(CONFIG_PLANE_HEIGHT/2), 
                x_current+CONFIG_PLANE_WIDTH, y_location-(CONFIG_PLANE_HEIGHT/2), CONFIG_COLOR_PLANE);
            // Check if the plane missile is idle and if the plane is at the randomly selected launch x-coordinate.
            // If it is, launch the missile and select a new launch x-coordinate.    
            if(missile_is_idle(missile) && x_current <= missile_launch_x){
                missile_init_plane(missile, x_current, y_location);
                missile_launch_x = rand() % LCD_W;
            }
            break;
        // If the plane is inactive, increment the plane idle time and set the plane's x-coordinate to the origin.
        case inactive_st:
            x_current = x_origin;
            plane_idle_time++;
            destroy_plane = false;
            break;
        // In case things go horribly wrong, restart the state.
        default:
            break;
    }
}

/******************** Plane Status Function ********************/

// Return the current plane position through the pointers *x,*y.
void plane_get_pos(coord_t *x, coord_t *y){
    *x = x_current;
    *y = y_location;
}

// Return whether the plane is flying.
bool plane_is_flying(void){
    // Check if the plane is in the flying state. If it is, return true.
    if(currentState == flying_st){
        return true;
    } else {
        return false;
    }   
}
