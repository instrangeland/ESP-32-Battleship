#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>
#include <stdbool.h>
#include "coord.h"
#include <stdio.h>

// Clear the board
void board_clear(void);

// Get mark at board location
bool board_get_vars(int8_t r, int8_t c);

bool board_get_coord(coord check_coord);

// Check if mark type is a winner.
bool board_winner();

// Get a count of marks in the board.
// Use to determine a draw condition if count is equal to CONFIG_BOARD_SPACES
uint16_t board_mark_count(void);

void write_coords(coord *coord_to_write, uint8_t num_coords);

bool check_coords_free(coord *coord_to_write, uint8_t num_coords);

uint8_t find_full_coord(coord *coord_to_write, uint8_t num_coords);

bool all_coords_valid(coord *coord_to_write, uint8_t num_coords);

#endif // BOARD_H_
