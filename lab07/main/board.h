#ifndef CE99D8A3_18D8_45AC_BDB1_3709003ECA37
#define CE99D8A3_18D8_45AC_BDB1_3709003ECA37

#include <stdint.h>
#include <stdbool.h>
#include "coord.h"
#include <stdio.h>
#include "game.h"
#include "player.h"
#include "config.h"
#include "ship.h"

#define BOARD_SPACES CONFIG_BOARD_SPACES

// clear bot's probability board
void bot_probability_clear();

void bot_calculate_probability(PLAYER *player, uint8_t starting_row, uint8_t num_rows, uint8_t ship_length, uint8_t weight);
// this goes thru the board and offers incentives to hit cells next to existing hits
void bot_emphasize_previous_hits(PLAYER *player);
// this actually does the incentivizing
void bot_emphasize_coordinate(PLAYER *player, coord coordinate);

coord bot_decide_shot(PLAYER *player);
// add the current coords to probability matrix (if possible)
void bot_increment_probability_matrix(coord *coords, uint8_t num_coords, uint8_t weight);
// checks if a set of coords are possible given the current information
bool bot_is_ship_possible(PLAYER *player, coord *coords, uint8_t num_coords);

// Clear the board
void board_clear(PLAYER *player);
void bot_print_probability_board();
// Get mark at board location
int8_t board_get_vars(PLAYER *player, int8_t r, int8_t c);

int8_t board_get_coord(PLAYER *player, coord check_coord);

void write_coords(PLAYER *player, coord *coord_to_write, uint8_t num_coords, int8_t ship_num);

bool check_coords_free(PLAYER *player, coord *coord_to_write, uint8_t num_coords);
bool check_coords_within_board(coord *coord_to_write, uint8_t num_coords);

uint8_t find_invalid_coord(PLAYER *player, coord *coord_to_write, uint8_t num_coords);

bool all_coords_valid(PLAYER *player, coord *coord_to_write, uint8_t num_coords);

void print_board(PLAYER *player);

#endif /* CE99D8A3_18D8_45AC_BDB1_3709003ECA37 */
