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

