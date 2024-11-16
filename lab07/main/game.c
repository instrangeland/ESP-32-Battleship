#include "config.h"
#include "graphics.h"
#include "game.h"
#include "nav.h"
#include "board.h"
#include "com.h"
#include "pin.h"
#include "hw.h"
#include "lcd.h"
#include <stdio.h>

#define HW_BTN_A 32
#define HW_BTN_B 33
#define HW_BTN_MENU 13
#define HW_BTN_OPTION 0
#define HW_BTN_SELECT 27
#define HW_BTN_START 39
#define MESSAGE_MAX_LENGTH 100
#define COL_BITS 0b1111
#define ROW_BITS 0b11110000
#define ROW_SHIFT 4
// is this a scope??
typedef enum Mark
{
    cross,
    naut,
    empty,
    draw,
} Mark;
// typedef
typedef enum GameState
{
    init,
    new_game,
    main,
    mark,
    wait_button_up,
    game_won,
} GameState;

// shift the buffer to remove sent data

#define ROW_BOUNDED(row) ((row < CONFIG_BOARD_R))
#define COL_BOUNDED(col) ((col < CONFIG_BOARD_C))

static GameState current_state;

static Mark game_board[CONFIG_BOARD_R][CONFIG_BOARD_C];
static Mark current_player = naut;

char message[MESSAGE_MAX_LENGTH];

// clear all board to empty
void clear_game_board()
{
    // col i think
    for (int8_t i = 0; i < CONFIG_BOARD_R; i++)
    {
        // row prob
        for (int8_t j = 0; j < CONFIG_BOARD_C; j++)
        {
            game_board[i][j] = empty;
        }
    }
}
// is this position valid to place
bool mark_valid(int8_t row, int8_t col)
{
    // what if instead of having scope i just used ifs with no brackets and gotos and labels then no points lost lol and no comments
    if (row_bounded(row) && col_bounded(col))
    {
        return (game_board[row][col] == empty);
    }
    else
    {
        return false;
    }
}

// display whos turn it is
void display_current_turn(Mark who_turn)
{
    // no scope get pranked
    if (who_turn == naut)
        snprintf(message, MESSAGE_MAX_LENGTH, "Naut's turn!");
    else
        snprintf(message, MESSAGE_MAX_LENGTH, "Cross' turn!");
    graphics_drawMessage(message, WHITE, BLACK);
}
// display win message
void display_victory(Mark who_won)
{
    // check win
    if (who_won == naut)
    {
        snprintf(message, MESSAGE_MAX_LENGTH, "Naut wins! Press start to reset.");
    }
    // idk i don;t wanna lose points
    else if (who_won == cross)
    {
        snprintf(message, MESSAGE_MAX_LENGTH, "Cross wins! Press start to reset.");
    }
    else
    {
        snprintf(message, MESSAGE_MAX_LENGTH, "Draw! Press start to reset.");
    }

    graphics_drawMessage(message, WHITE, BLACK);
}
// make a mark
void do_mark(int8_t row, int8_t col, Mark player)
{
    game_board[row][col] = player;
    // which play
    if (player == naut)
    {
        graphics_drawO(row, col, WHITE);
    }
    else
    {
        graphics_drawX(row, col, WHITE);
    }
    uint8_t send_byte = 0;
    send_byte |= col;
    send_byte |= (row << ROW_SHIFT);
    com_write((void *)&send_byte, sizeof(send_byte));
}
// print over uart
void print_board()
{
    // it row
    for (int8_t row = 0; row < CONFIG_BOARD_R; row++)
    {
        // it col
        for (int8_t col = 0; col < CONFIG_BOARD_C; col++)
        {
            printf("%d", game_board[row][col]);
        }
        printf("\n");
    }
}
// reset, either after cold restart or game won
void reset_game()
{
    nav_set_loc(1, 1);
    lcd_fillScreen(CONFIG_BACK_CLR);
    clear_game_board();
    graphics_drawGrid(RED);
    current_player = cross;
}
// returns a mark of who won, or empty if no win yet.
Mark check_win()
{
    // print_board();
    for (Mark player = cross; player < empty; player++)
    {
        // check rows
        for (int8_t row = 0; row < CONFIG_BOARD_R; row++)
        {
            bool won = true;
            // col
            for (int8_t col = 0; col < CONFIG_BOARD_C; col++)
            {
                // is unwin
                if (game_board[row][col] != player)
                {
                    won = false;
                    break;
                }
            }
            // is won
            if (won)
            {
                return player;
            }
        }
        // check columns
        for (int8_t col = 0; col < CONFIG_BOARD_R; col++)
        {
            bool won = true;
            // iterate row
            for (int8_t row = 0; row < CONFIG_BOARD_C; row++)
            {
                // is not win
                if (game_board[row][col] != player)
                {
                    won = false;
                    break;
                }
            }
            // is win
            if (won)
            {
                return player;
            }
        }

        bool won = true;
        // check linear diag
        for (int8_t combined_coord = 0; combined_coord < CONFIG_BOARD_R; combined_coord++)
        {
            // check if not win
            if (game_board[combined_coord][combined_coord] != player)
            {
                won = false;
                break;
            }
        }
        // wow what this do idk
        if (won)
        {
            return player;
        }
        won = true;
        // iterate the diag
        for (int8_t combined_coord = 0; combined_coord < CONFIG_BOARD_R; combined_coord++)
        {
            // check if not win
            if (game_board[CONFIG_BOARD_R - 1 - combined_coord][combined_coord] != player)
            {
                won = false;
                break;
            }
        }
        // if you can't tell what this does, you're dumb
        if (won)
        {
            return player;
        }
    }
    bool is_draw = true;
    // row iterate
    for (int8_t row = 0; row < CONFIG_BOARD_R; row++)
    {
        // col iterate
        for (int8_t col = 0; col < CONFIG_BOARD_C; col++)
        {
            // is empty
            if (game_board[row][col] == empty)
            {
                is_draw = false;
            }
        }
    }
    // is it a draw
    if (is_draw)
    {
        return draw;
    }
    return empty;
}

// Initialize the game logic.
void game_init(void)
{
    reset_game();
}

// Update the game logic.
int8_t row = 0;
int8_t col = 0;
void game_tick(void)
{
    // also a switch
    switch (current_state)
    {
    case init:
        current_state = new_game;
        break;
    case new_game:
        current_state = main;
        display_current_turn(current_player);
        break;
    case main:
        // did click?
        uint8_t val;
        com_read((void *)&val, 1);
        // click or got byte
        // printf("val: %d", val);
        if (val > 0)
        {
            row = (ROW_BITS & val) >> ROW_SHIFT;
            col = COL_BITS & val;
            printf("%d, %d", row, col);
            // did we desync or did it work?
            if (mark_valid(row, col))
            {
                current_state = mark;
                printf("NON_DESYNC: %d, %d\n", row, col);
            }
            else
            {
                printf("DESYNC: %d, %d\n", row, col);
            }
        }
        else if (!pin_get_level(HW_BTN_A))
        {
            nav_get_loc(&row, &col);
            if (mark_valid(row, col))
            {
                current_state = mark;
                printf("NORMAL: %d, %d\n", row, col);
            }
        }
        break;
    case mark:
        Mark who_won = check_win();
        // did who won not empty
        if (who_won != empty)
        {
            current_state = game_won;
            display_victory(who_won);
        }
        else
        {
            current_state = wait_button_up;
        }
        break;
    case wait_button_up:
        // is button not up
        if (pin_get_level(HW_BTN_A))
        {
            current_state = main;
        }
        break;
    case game_won:
        // if statement
        if (!pin_get_level(HW_BTN_START))
        {
            current_state = new_game;
        }
    default:
        break;
    }
    // this is a switch
    switch (current_state)
    {
    case new_game:
        reset_game();
        break;
    case main:
        break;
    case mark:
        do_mark(row, col, current_player);
        // wow much comment
        if (current_player == cross)
        {
            current_player = naut;
        }
        else
        {
            current_player = cross;
        }
        display_current_turn(current_player);
        break;
    case game_won:
        break;
    default:
        break;
    }
}