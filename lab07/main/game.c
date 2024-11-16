#include "config.h"
#include "board.h"
#include "graphics.h"
#include "nav.h"
#include "com.h"
#include "game.h"
#include "joy.h"
#include "lcd.h"
#include "pin.h"
#include "hw.h"

// Define enums for the game state machine and to manage place ships
enum GAME_CNTRL_STATE {INIT_STATE, NEW_GAME_STATE, PLACE_SHIPS_STATE, READY_STATE, PLAY_STATE, END_STATE}; 


// Define ship sizes
#define CARRIER_SIZE 5
#define BATTLESHIP_SIZE 4
#define SUBMARINE_SIZE 3
#define CRUISER_SIZE 3
#define DESTROYER_SIZE 2

// Define bit shift and bit mask options for UART data manipulation.
#define BIT_SHIFT 4
#define BIT_MASK 0x0F

// Define global static variables to manage the game state machine and game logic.
static enum GAME_CNTRL_STATE currentState;
static int8_t playerTurn;
static bool shipsPlaced;
static bool setMark, dataCheck;

// Initialize the game variables and logic.
// @param void: None
// @return void: None
void game_init(void){
    currentState = INIT_STATE;
}

void game_tick(void){
    switch(currentState){
        case INIT_STATE:
            currentState = NEW_GAME_STATE;
            break;
        case NEW_GAME_STATE:
            break;
        case PLACE_SHIPS_STATE: 
            break;
        case READY_STATE:
            break;
        case PLAY_STATE:
            break;
        case END_STATE:
            break;
        default:
            break;
    }

    switch(currentState){
        case INIT_STATE:
            break;
        case NEW_GAME_STATE:
            lcd_fillScreen(CONFIG_BACK_CLR);
            graphics_drawGrid(CONFIG_GRID_CLR);
            board_clear();
            nav_set_loc(0,0);
            break;
        case PLACE_SHIPS_STATE: 
            break;
        case READY_STATE:
            break;
        case PLAY_STATE:
            break;
        case END_STATE:
            break;
        default:
            break;
    }
}