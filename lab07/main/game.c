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
    P2_PLACE_SHIPS_STATE,
    READY_STATE,
    BOT_DECIDE_STATE,
    PLAYER_MARK_STATE,
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

PLAYER player1;
PLAYER player2;

// Define bit shift and bit mask options for UART data manipulation.
#define BIT_SHIFT 4
#define BIT_MASK 0x0F

// Define global static variables to manage the game state machine and game logic.
uint8_t bot_ship_lengths[] = {3, 4, 5, 2}; /* we start with 3 as that way we can just run it once and double the probabilities,
                                            instead of calculating for both 3 long ships*/
static bool placedMark;
static bool shipsPlaced;
static enum GAME_CNTRL_STATE currentState;
static bool setMark, dataCheck;
static uint8_t placing_ship = 0;
static bool rotateShip, pressed;
static uint64_t btns;
uint8_t bot_current_row = 0;
uint8_t bot_current_ship_index = 5;
coord temp_coordinates[5];
char temp_char[50];

// Initialize the game variables and logic.
// @param void: None
// @return void: None
void game_init(void)
{
    init_player(&player1, false);
    init_player(&player2, true);
    board_clear(&player1);
    board_clear(&player2);

    currentState = INIT_STATE;
    rotateShip = false;
    pressed = false;
    btns = 0;
    placedMark = false;
    shipsPlaced = false;
    setMark = false;
    dataCheck = false;
    srand(esp_timer_get_time());
}

void print_ship(uint8_t ship_num)
{
    snprintf(temp_char, 50, "Place %s (length %d)", player1.ships[ship_num].name, player1.ships[ship_num].length);
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

void redraw_all_ships(PLAYER *player)
{
    for (uint8_t cur_ship = 0; cur_ship < 5; cur_ship++)
    {
        if (player->ships[cur_ship].placed)
            redraw_ship(player->ships[cur_ship].coordinates, player->ships[cur_ship].length, CONFIG_BTTLESHIP_CLR, true);
    }
}

void draw_invalid_ship(coord *coords_to_draw, uint8_t invalid_starting_at, uint8_t ship_length)
{
    printf("invalid starting at %d\n", invalid_starting_at);
    redraw_ship(coords_to_draw, invalid_starting_at, GREEN, false);
    for (uint8_t i = invalid_starting_at; i < ship_length; i++)
    {
        graphics_drawX(coords_to_draw[i].row, coords_to_draw[i].col, RED);
    }
}

void draw_hit_board(PLAYER *player_perspective)
{
    lcd_fillScreen(CONFIG_BACK_CLR);
    graphics_drawGrid(CONFIG_GRID_CLR);
    for (int8_t r = 0; r < BOARD_R; r++)
    {
        for (int8_t c = 0; c < BOARD_C; c++)
        {
            if (player_perspective->shot_board[r][c] == HIT)
            {
                draw_shot(r, c, RED);
            }
            if (player_perspective->shot_board[r][c] == MISS)
            {
                draw_shot(r, c, WHITE);
            }
        }
    }
}

void print_hit_board(PLAYER *player_perspective)
{
    printf("shot_board\n");
    for (int8_t r = 0; r < BOARD_R; r++)
    {
        for (int8_t c = 0; c < BOARD_C; c++)
        {
            printf("%d ", player_perspective->shot_board[r][c]);
        }
        printf("\n");
    }
    printf("\n");
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
            redraw_all_ships(&player1);
            graphics_drawMessage("All ships placed!!", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
            currentState = P2_PLACE_SHIPS_STATE;
        }
        break;
    case P2_PLACE_SHIPS_STATE:
        if (placing_ship > 4)
        {
            lcd_fillScreen(CONFIG_BACK_CLR);
            graphics_drawGrid(CONFIG_GRID_CLR);
            currentState = READY_STATE;
            redraw_all_ships(&player2);
            graphics_drawMessage("All ships placed!!", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
        }
        break;
    case READY_STATE:
        prev_column = -1;
        prev_row = -1;
        bot_current_row = 0;
        bot_current_ship_index = 0;
        init_shot_board(&player1);
        currentState = BOT_DECIDE_STATE;
        break;
    case BOT_DECIDE_STATE:
        if (bot_current_ship_index > 3)
        {
            // bot_print_probability_board();
            bot_emphasize_previous_hits(&player2);
            coord bot_choice = bot_decide_shot(&player2);
            snprintf(temp_char, 50, "Bot chose %d, %d\n", bot_choice.row + 1, bot_choice.col + 1);
            printf(temp_char);
            graphics_drawMessage(temp_char, CONFIG_MESS_CLR, CONFIG_BACK_CLR);
            attempt_shot(&player2, &player1, bot_choice);
            //draw_hit_board(&player2);
            //print_hit_board(&player2);
            currentState = PLAYER_MARK_STATE;
            draw_hit_board(&player1);
            graphics_drawMessage("Your turn: mark a spot", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
        }
        break;
    case PLAYER_MARK_STATE:
        bot_current_row = 0;
        bot_current_ship_index = 0;
        if(placedMark && !pin_get_level(HW_BTN_A)){
            placedMark = false;
            graphics_drawMessage("Bot's turn: thinking...", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
            currentState = BOT_DECIDE_STATE;
        }
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
        nav_set_loc(0, 0);
        break;
    case PLACE_SHIPS_STATE:
        int8_t column, row;
        nav_get_loc(&row, &column);
        coord start_coords = {row, column};
        bool checkPlace = false;
        bool dirty = false;
        btns = ~pin_get_in_reg() & HW_BTN_MASK;
        // this is dirty
        if ((prev_column != column) || (prev_row != row))
        {
            coord prev_coords = {prev_row, prev_column};
            get_coordinates(temp_coordinates, prev_coords, player1.ships[placing_ship].length, !rotateShip);
            redraw_ship(temp_coordinates, player1.ships[placing_ship].length, CONFIG_BACK_CLR, true);
            start_coords.row = row;
            start_coords.col = column;
            get_coordinates(player1.ships[placing_ship].coordinates, start_coords, player1.ships[placing_ship].length, !rotateShip);
            redraw_all_ships(&player1);
            if (check_coords_free(&player1, player1.ships[placing_ship].coordinates, player1.ships[placing_ship].length))
            {
                redraw_ship(player1.ships[placing_ship].coordinates, player1.ships[placing_ship].length, CONFIG_BTTLESHIP_CLR, false);
            }
            else
            {
                draw_invalid_ship(player1.ships[placing_ship].coordinates, find_invalid_coord(&player1, player1.ships[placing_ship].coordinates, player1.ships[placing_ship].length), player1.ships[placing_ship].length);
            }
            print_ship(placing_ship);
            prev_column = column;
            prev_row = row;
        }
        else if (!pin_get_level(HW_BTN_B) && !pressed && btns)
        {
            pressed = true;

            get_coordinates(temp_coordinates, start_coords, player1.ships[placing_ship].length, !rotateShip);
            redraw_ship(temp_coordinates, player1.ships[placing_ship].length, CONFIG_BACK_CLR, true);
            rotateShip = !rotateShip;

            get_coordinates(player1.ships[placing_ship].coordinates, start_coords, player1.ships[placing_ship].length, !rotateShip);
            redraw_ship(player1.ships[placing_ship].coordinates, player1.ships[placing_ship].length, CONFIG_BTTLESHIP_CLR, false);
            print_ship(placing_ship);
        }
        else if (!pin_get_level(HW_BTN_A) && !pressed && btns)
        {
            pressed = true;
            if (all_coords_valid(&player1, player1.ships[placing_ship].coordinates, player1.ships[placing_ship].length))
            {
                player1.ships[placing_ship].placed = true;
                placing_ship++;
                write_coords(&player1, player1.ships[placing_ship - 1].coordinates, player1.ships[placing_ship - 1].length, placing_ship - 1);
                redraw_ship(player1.ships[placing_ship - 1].coordinates, player1.ships[placing_ship - 1].length, CONFIG_BTTLESHIP_CLR, true);
                print_ship(placing_ship - 1);

                if (rotateShip)
                {
                    nav_set_loc(row, column + 1);
                }
                else
                {
                    nav_set_loc(row + 1, column);
                }
            }
            print_board(&player1);
        }
        else if (pressed && !btns)
        {
            pressed = false; // all released
        }
        break;
    case P2_PLACE_SHIPS_STATE:
        printf("in p2 state\n");
        if (true)
        {
            print_board(&player2);
            for (placing_ship = 0; placing_ship < 5; placing_ship++)
            {
                bool placed = false;
                while (!placed)
                {
                    coord ship_possible = random_coord();
                    bool direction = RAND_INT(0, 2);
                    printf("coord: r: %d c: %d dir: %d\n", ship_possible.row, ship_possible.col, direction);
                    get_coordinates(player2.ships[placing_ship].coordinates, ship_possible, player2.ships[placing_ship].length, !rotateShip);
                    if (all_coords_valid(&player2, player2.ships[placing_ship].coordinates, player2.ships[placing_ship].length))
                    {
                        write_coords(&player2, player2.ships[placing_ship].coordinates, player2.ships[placing_ship].length, placing_ship);
                        placed = true;
                        player2.ships[placing_ship].placed = true;
                    }
                }
                print_board(&player2);
            }
            printf("done\n");
            redraw_all_ships(&player2);
        }

        else if (false)
        { // 2 people same handheld
        }
        else if (false)
        { // 2 people diff handheld over uart
        }
        lcd_fillScreen(CONFIG_BACK_CLR);
        graphics_drawGrid(CONFIG_GRID_CLR);
        break;
    case READY_STATE:

        break;
    case BOT_DECIDE_STATE:
        if (bot_current_row >= BOARD_R)
        {
            bot_current_row = 0;
            bot_current_ship_index++;
        }
        if (bot_current_ship_index > 3)
            break;
        if (bot_current_ship_index == 0)
            bot_calculate_probability(&player2, bot_current_row, 2, bot_ship_lengths[bot_current_ship_index], 2);
        else
            bot_calculate_probability(&player2, bot_current_row, 2, bot_ship_lengths[bot_current_ship_index], 1);
        bot_current_row += 2;
        break;
    case PLAYER_MARK_STATE:
        int8_t nav_row, nav_col;
        nav_get_loc(&nav_row, &nav_col);
        if((prev_column != nav_col) || (prev_row != nav_row)){
            graphics_drawHighlight(prev_row, prev_column, CONFIG_BACK_CLR);
            graphics_drawHighlight(nav_row, nav_col, CONFIG_MESS_CLR);
            prev_column = nav_col;
            prev_row = nav_row;
        }
        if(!placedMark && !pin_get_level(HW_BTN_A)){
            coord mark = {nav_row, nav_col};
            if(get_shot_location(&player1, mark) == NOT_TRIED){
                attempt_shot(&player1, &player2, mark);
                draw_hit_board(&player1);
                print_hit_board(&player1);
                placedMark = true;
            }
        }
        break;
    case PLAY_STATE:
        break;
    case END_STATE:
        break;
    default:
        break;
    }
}
