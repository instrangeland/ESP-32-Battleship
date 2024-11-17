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
#include "ship.h"
#include <stdlib.h>

// Define enums for the game state machine and to manage place ships
enum GAME_CNTRL_STATE {INIT_STATE, NEW_GAME_STATE, PLACE_SHIPS_STATE, READY_STATE, PLAY_STATE, END_STATE}; 
enum SHIP_TYPE {CARRIER, BATTLESHIP, SUBMARINE, CRUISER, DESTROYER};

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
static uint8_t placing_ship;
static bool rotateShip;

// Initialize the game variables and logic.
// @param void: None
// @return void: None
void game_init(void){
    currentState = INIT_STATE;
    placing_ship = CARRIER;
    rotateShip = false;
}

void game_tick(void){
    switch(currentState){
        case INIT_STATE:
            currentState = NEW_GAME_STATE;
            break;
        case NEW_GAME_STATE:
            currentState = PLACE_SHIPS_STATE;
            break;
        case PLACE_SHIPS_STATE: 
            if(shipsPlaced){
                currentState = READY_STATE;
            } else{
                currentState = PLACE_SHIPS_STATE;
            }
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
            int8_t row, column;
            nav_get_loc(&row, &column);
            coord start_coord = {row, column};
            bool checkPlace = false;

            if(!pin_get_level(HW_BTN_B) && !rotateShip){
                rotateShip = true;
            } else if(!pin_get_level(HW_BTN_B) && rotateShip){
                rotateShip = false;
            }

            switch(placing_ship){
                case CARRIER:
                    coord* carrier_coords;
                    graphics_drawMessage("Place Carrier", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                    if(!rotateShip){
                        carrier_coords = get_coordinates(start_coord, CARRIER_SIZE, HORIZONTAL);
                        for(int i = 0; i < CARRIER_SIZE; i++){
                            graphics_drawHighlight(carrier_coords[i].row, carrier_coords[i].col, CONFIG_HIGH_CLR);
                        }
                    }
                    else{
                        carrier_coords = get_coordinates(start_coord, CARRIER_SIZE, VERTICAL);
                        for(int i = 0; i < CARRIER_SIZE; i++){
                            graphics_drawHighlight(carrier_coords[i].row, carrier_coords[i].col, CONFIG_HIGH_CLR);
                        }
                    }

                    if(!pin_get_level(HW_BTN_A)){
                        placing_ship = BATTLESHIP;
                        checkPlace = true;
                    }

                    break;
                case BATTLESHIP:
                    coord* battleship_coords;
                    graphics_drawMessage("Place Battleship", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                    if(!rotateShip){
                        battleship_coords = get_coordinates(start_coord, BATTLESHIP_SIZE, HORIZONTAL);
                        for(int i = 0; i < sizeof(battleship_coords); i++){
                            graphics_drawHighlight(battleship_coords[i].row, battleship_coords[i].col, CONFIG_HIGH_CLR);
                        }
                    }
                    else{
                        battleship_coords = get_coordinates(start_coord, BATTLESHIP_SIZE, VERTICAL);
                        for(int i = 0; i < sizeof(battleship_coords); i++){
                            graphics_drawHighlight(battleship_coords[i].row, battleship_coords[i].col, CONFIG_HIGH_CLR);
                        }
                    }

                    if(!pin_get_level(HW_BTN_A)){
                        placing_ship = SUBMARINE;
                    }

                    break;
                case SUBMARINE:
                    graphics_drawMessage("Place Submarine", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                    
                    break;
                case CRUISER:
                    graphics_drawMessage("Place Cruiser", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                    placing_ship++;
                    break;
                case DESTROYER:
                    graphics_drawMessage("Place Destroyer", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                    shipsPlaced = true;
                    break;
                default:
                    break;
            }
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