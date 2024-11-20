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
#include <stdio.h>

// Define enums for the game state machine and to manage place ships
enum GAME_CNTRL_STATE
{
    INIT_STATE,
    NEW_GAME_STATE,
    PLACE_SHIPS_STATE,
    READY_STATE,
    PLAY_STATE,
    END_STATE
};
enum SHIP_TYPE
{
    CARRIER = 0,
    BATTLESHIP = 1,
    SUBMARINE = 2,
    CRUISER = 3,
    DESTROYER = 4,
};
SHIP carrier = {"Carrier", false, {0, 0}, NULL, 5};
SHIP battleship = {"Battleship", false, {0, 0}, NULL, 4};
SHIP submarine = {"Submarine", false, {0, 0}, NULL, 3};
SHIP cruiser = {"Cruiser", false, {0, 0}, NULL, 3};
SHIP destroyer = {"Destroyer", false, {0, 0}, NULL, 2};

SHIP *ships[] = {&carrier, &battleship, &submarine, &cruiser, &destroyer};

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

char temp_char[50];

// Initialize the game variables and logic.
// @param void: None
// @return void: None
void game_init(void)
{
    currentState = INIT_STATE;
    rotateShip = false;
}

void print_ship(uint8_t ship_num)
{
    snprintf(temp_char, 50, "Place %s", ships[ship_num]->name);
    graphics_drawMessage(temp_char, CONFIG_MESS_CLR, CONFIG_BACK_CLR);
}

void game_tick(void)
{
    switch (currentState)
    {
    case INIT_STATE:
        currentState = NEW_GAME_STATE;
        break;
    case NEW_GAME_STATE:
        currentState = PLACE_SHIPS_STATE;
        print_ship(0);
        break;
    case PLACE_SHIPS_STATE:
        if (placing_ship >= DESTROYER) {
            currentState = READY_STATE;
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

    switch (currentState)
    {
    case INIT_STATE:
        break;
    case NEW_GAME_STATE:
        lcd_fillScreen(CONFIG_BACK_CLR);
        graphics_drawGrid(CONFIG_GRID_CLR);
        board_clear();
        nav_set_loc(0, 0);
        break;
    case PLACE_SHIPS_STATE:
        int8_t row, column;
        nav_get_loc(&row, &column);
        coord start_coord = {row, column};
        bool checkPlace = false;

        if (!pin_get_level(HW_BTN_B))
        {
            rotateShip = !rotateShip;
            while (!pin_get_level(HW_BTN_A)) {}
        }

        if (!rotateShip)
        {
            ships[placing_ship]->coordinates = get_coordinates(start_coord, ships[placing_ship]->length, HORIZONTAL);
            for (int i = 0; i < CARRIER_SIZE; i++)
            {
                graphics_drawHighlight(ships[placing_ship]->coordinates[i].row, ships[placing_ship]->coordinates[i].col, CONFIG_HIGH_CLR);
            }
        }
        else
        {
            ships[placing_ship]->coordinates = get_coordinates(start_coord, ships[placing_ship]->length, VERTICAL);
            for (int i = 0; i < CARRIER_SIZE; i++)
            {
                graphics_drawHighlight(ships[placing_ship]->coordinates[i].row, ships[placing_ship]->coordinates[i].col, CONFIG_HIGH_CLR);
            }
        }
        if (!pin_get_level(HW_BTN_A))
        {
            placing_ship++;
            checkPlace = true;
            print_ship(placing_ship);
            while (!pin_get_level(HW_BTN_A)) {}
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
