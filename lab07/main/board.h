#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>
#include <stdbool.h>
#include "coord.h"
#include <stdio.h>
#include "game.h"
#include "player.h"
#include "config.h"

#define BOARD_SPACES CONFIG_BOARD_SPACES
// Clear the board
void board_clear(PLAYER *player);

// Get mark at board location
int8_t board_get_vars(PLAYER *player, int8_t r, int8_t c);

int8_t board_get_coord(PLAYER *player, coord check_coord);

void write_coords(PLAYER *player, coord *coord_to_write, uint8_t num_coords, int8_t ship_num);

bool check_coords_free(PLAYER *player, coord *coord_to_write, uint8_t num_coords);
bool check_coords_within_board(coord *coord_to_write, uint8_t num_coords);

uint8_t find_invalid_coord(PLAYER *player, coord *coord_to_write, uint8_t num_coords);

bool all_coords_valid(PLAYER *player, coord *coord_to_write, uint8_t num_coords);

void print_board(PLAYER *player);

#endif // BOARD_H_
