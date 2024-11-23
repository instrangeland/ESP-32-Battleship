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

// Define bit shift and bit mask options for UART data manipulation.
#define BIT_SHIFT 4
#define BIT_MASK 0x0F

// Define global static variables to manage the game state machine and game logic.
static enum GAME_CNTRL_STATE currentState;
static int8_t playerTurn;
static bool shipsPlaced;
static bool setMark, dataCheck;
static uint8_t placing_ship = 0;
static bool rotateShip, pressed;
static uint64_t btns;

char temp_char[50];

// Initialize the game variables and logic.
// @param void: None
// @return void: None
void game_init(void)
{
    currentState = INIT_STATE;
    rotateShip = false;
    pressed = false;
    btns = 0;
}

void print_ship(uint8_t ship_num)
{
    snprintf(temp_char, 50, "Place %s", ships[ship_num]->name);
    graphics_drawMessage(temp_char, CONFIG_MESS_CLR, CONFIG_BACK_CLR);
}

void redraw_ship(coord *coord_to_draw, uint8_t num_coords, color_t color, bool filled)
{
    for (int i = 0; i < num_coords; i++)
    {
        if (!filled)
        {
            graphics_drawHighlight(coord_to_draw[i].row, coord_to_draw[i].col, color);
        }
        else
        {
            graphics_filldrawHighlight(coord_to_draw[i].row, coord_to_draw[i].col, color);
        }
    }
}

void redraw_all_ships()
{
    for (uint8_t cur_ship = 0; cur_ship < 5; cur_ship++)
    {
        if (ships[cur_ship]->placed)
            redraw_ship(ships[cur_ship]->coordinates, ships[cur_ship]->length, GREEN, true);
    }
}

void draw_invalid_ship(coord *coords_to_draw, uint8_t invalid_starting_at)
{
    printf("invalid starting at %d\n", invalid_starting_at);
    redraw_ship(coords_to_draw, invalid_starting_at, GREEN, false);
    graphics_drawX(coords_to_draw[invalid_starting_at].row, coords_to_draw[invalid_starting_at].col, RED);
}

int8_t prev_row,
    prev_column = 5;

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
        if (placing_ship > 4)
        {
            lcd_fillScreen(CONFIG_BACK_CLR);
            graphics_drawGrid(CONFIG_GRID_CLR);
            currentState = READY_STATE;
            redraw_all_ships();
            graphics_drawMessage("All ships placed!!", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
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
        coord start_coords = {row, column};
        bool checkPlace = false;
        bool dirty = false;
        btns = ~pin_get_in_reg() & HW_BTN_MASK;
        // this is dirty
        if ((prev_column != column) || (prev_row != row))
        {
            coord prev_coords = {prev_row, prev_column};
            redraw_ship(get_coordinates(prev_coords, ships[placing_ship]->length, !rotateShip), ships[placing_ship]->length, CONFIG_BACK_CLR, true);
            start_coords.row = row;
            start_coords.col = column;
            
            ships[placing_ship]->coordinates = get_coordinates(start_coords, ships[placing_ship]->length, !rotateShip);
            redraw_all_ships();
            if (check_coords_free(ships[placing_ship]->coordinates, ships[placing_ship]->length))
            {
                redraw_ship(ships[placing_ship]->coordinates, ships[placing_ship]->length, GREEN, false);
            }
            else
            {
                draw_invalid_ship(ships[placing_ship]->coordinates, find_full_coord(ships[placing_ship]->coordinates, ships[placing_ship]->length));
            }
            print_ship(placing_ship);
            prev_column = column;
            prev_row = row;
        }
        else if (!pin_get_level(HW_BTN_B) && !pressed && btns)
        {
            pressed = true;
            redraw_ship(get_coordinates(start_coords, ships[placing_ship]->length, !rotateShip), ships[placing_ship]->length, CONFIG_BACK_CLR, true);
            rotateShip = !rotateShip;

            ships[placing_ship]->coordinates = get_coordinates(start_coords, ships[placing_ship]->length, !rotateShip);
            redraw_ship(ships[placing_ship]->coordinates, ships[placing_ship]->length, GREEN, false);
            print_ship(placing_ship);
        }
        else if (!pin_get_level(HW_BTN_A) && !pressed && btns)
        {
            pressed = true;
            if (all_coords_valid(ships[placing_ship]->coordinates, ships[placing_ship]->length))
            {
                ships[placing_ship]->placed = true;
                placing_ship++;
                write_coords(ships[placing_ship - 1]->coordinates, ships[placing_ship]->length, placing_ship - 1);
                redraw_ship(ships[placing_ship - 1]->coordinates, ships[placing_ship]->length, GREEN, true);
                print_ship(placing_ship - 1);

                if(rotateShip){
                    nav_set_loc(row, column + 1);
                } else{
                    nav_set_loc(row + 1, column);
                }
            }
        }
        else if (pressed && !btns)
        {
            pressed = false; // all released
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
