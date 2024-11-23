#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>
#include <stdbool.h>
#include "coord.h"
#include <stdio.h>
#include "game.h"

#define BOARD_R CONFIG_BOARD_R // Rows
#define BOARD_C CONFIG_BOARD_C // Columns
#define BOARD_N CONFIG_BOARD_N // Number of contiguous marks
#define BOARD_SPACES CONFIG_BOARD_SPACES
#define EMPTY_SPACE -1
// Clear the board
void board_clear(void);

// Get mark at board location
int8_t board_get_vars(int8_t r, int8_t c);

int8_t board_get_coord(coord check_coord);

// Check if mark type is a winner.
// Get a count of marks in the board.
// Use to determine a draw condition if count is equal to CONFIG_BOARD_SPACES
uint16_t board_mark_count(void);

void write_coords(coord *coord_to_write, uint8_t num_coords, int8_t ship_num);

bool check_coords_free(coord *coord_to_write, uint8_t num_coords);
bool check_coords_within_board(coord *coord_to_write, uint8_t num_coords);

uint8_t find_invalid_coord(coord *coord_to_write, uint8_t num_coords);

bool all_coords_valid(coord *coord_to_write, uint8_t num_coords);

void print_board();

#endif // BOARD_H_
