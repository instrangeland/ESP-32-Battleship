#include "board.h"
#include "config.h"
#include "stdio.h"

static uint16_t mark_count;
uint32_t bot_probability_board[BOARD_R][BOARD_C];

void bot_probability_clear()
{
	for (int8_t r = 0; r < BOARD_R; r++)
		for (int8_t c = 0; c < BOARD_C; c++)
			bot_probability_board[r][c] = 0;
}

// weight variable is (for now) only used for the 3 wide ships to avoid duplicating work. it may be used later to make the ai more likely to shoot at certain types of ships
// also: the starting row and num_rows are used to distribute this accross several frames. So it will only run a few rows at a time
void bot_calculate_probability(PLAYER *player, uint8_t starting_row, uint8_t num_rows, uint8_t ship_length, uint8_t weight)
{
	coord test_coords[5];
	for (int8_t r = starting_row; r < starting_row + num_rows; r++)
	{
		for (int8_t c = 0; c < BOARD_C; c++)
		{
			coord start_coord = {r, c};
			if (check_coords_within_board(&start_coord, 1))
			{
				get_coordinates(test_coords, start_coord, ship_length, VERTICAL);
				if (check_coords_within_board(test_coords, ship_length))
				{
					if (bot_is_ship_possible(player, test_coords, ship_length))
					{
						bot_increment_probability_matrix(test_coords, ship_length, weight);
					}
				}
				get_coordinates(test_coords, start_coord, ship_length, HORIZONTAL);
				if (check_coords_within_board(test_coords, ship_length))
				{
					if (bot_is_ship_possible(player, test_coords, ship_length))
					{
						bot_increment_probability_matrix(test_coords, ship_length, weight);
					}
				}
			}
		}
	}
}

// I found it helpful to have it add extra weight to locations around places it already succeeded in hitting something...
void bot_emphasize_previous_hits(PLAYER *player)
{
	for (int8_t r = 0; r < BOARD_R; r++)
	{
		for (int8_t c = 0; c < BOARD_C; c++)
		{
			if (player->shot_board[r][c] == HIT)
			{
				coord coord_to_emphasize = {r, c};
				bot_emphasize_coordinate(player, coord_to_emphasize);
			}
		}
	}
}

void bot_emphasize_coordinate(PLAYER *player, coord coordinate)
{
	if (coordinate.col > 0)
	{
		if (coordinate.col < 9 && player->shot_board[coordinate.row][coordinate.col + 1] == HIT)
		{
			bot_probability_board[coordinate.row][coordinate.col - 1] *= HIT_BONUS * LINE_HIT_BONUS;
		}
		else
		{
			bot_probability_board[coordinate.row][coordinate.col - 1] *= HIT_BONUS;
		}
	}
	if (coordinate.col < 9)
	{
		if (coordinate.col > 0 && player->shot_board[coordinate.row][coordinate.col - 1] == HIT)
		{
			bot_probability_board[coordinate.row][coordinate.col + 1] *= HIT_BONUS * LINE_HIT_BONUS;
		}
		else
		{
			bot_probability_board[coordinate.row][coordinate.col + 1] *= HIT_BONUS;
		}
	}
	if (coordinate.row > 0)
	{
		if (coordinate.col < 9 && player->shot_board[coordinate.row + 1][coordinate.col] == HIT)
		{
			bot_probability_board[coordinate.row - 1][coordinate.col] *= HIT_BONUS * LINE_HIT_BONUS;
		}
		else
		{
			bot_probability_board[coordinate.row - 1][coordinate.col - 1] *= HIT_BONUS;
		}
	}
	if (coordinate.row < 9)
	{
		if (coordinate.col < 9 && player->shot_board[coordinate.row - 1][coordinate.col] == HIT)
		{
			bot_probability_board[coordinate.row + 1][coordinate.col] *= HIT_BONUS * LINE_HIT_BONUS;
		}
		else
		{
			bot_probability_board[coordinate.row + 1][coordinate.col] *= HIT_BONUS;
		}
	}
}

coord bot_decide_shot(PLAYER *player)
{
	// remove any shots we've already made
	coord max_location = {255, 255};
	uint32_t max_probability = 0;
	for (int8_t r = 0; r < BOARD_R; r++)
	{
		for (int8_t c = 0; c < BOARD_C; c++)
		{
			if (player->shot_board[r][c] != NOT_TRIED)
			{
				bot_probability_board[r][c] = 0;
			}
			else if (bot_probability_board[r][c] > max_probability)
			{
				max_probability = bot_probability_board[r][c];
				max_location.row = r;
				max_location.col = c;
			}
		}
	}
	return max_location;
}

void bot_increment_probability_matrix(coord *coords, uint8_t num_coords, uint8_t weight)
{
	for (uint8_t ship_num = 0; ship_num < num_coords; ship_num++)
	{
		bot_probability_board[coords[ship_num].row][coords[ship_num].col] += weight;
	}
}
// just check if any of the coordinates of a particular possible ship orientation are impossible
bool bot_is_ship_possible(PLAYER *player, coord *coords, uint8_t num_coords)
{
	for (uint8_t ship_num = 0; ship_num < num_coords; ship_num++)
	{
		if (player->shot_board[coords[ship_num].row][coords[ship_num].col] == MISS)
		{
			return false;
		}
	}
	return true;
}

//just used for debugging the bot. 
void bot_print_probability_board()
{
	for (int8_t r = 0; r < BOARD_R; r++)
	{
		for (int8_t c = 0; c < BOARD_C; c++)
		{
			printf("%05ld ", bot_probability_board[r][c]);
		}
		printf("\n");
	}
	printf("\n");
}

// Clear the board
void board_clear(PLAYER *player)
{
	for (int8_t r = 0; r < BOARD_R; r++)
		for (int8_t c = 0; c < BOARD_C; c++)
			player->board[r][c] = EMPTY_SPACE;
	mark_count = 0;
}

int8_t board_get_coord(PLAYER *player, coord check_coord)
{
	return player->board[check_coord.row][check_coord.col];
}

void write_coords(PLAYER *player, coord *coord_to_write, uint8_t num_coords, int8_t ship_num)
{
	print_coords(coord_to_write, num_coords);
	for (uint8_t i = 0; i < num_coords; i++)
	{
		player->board[coord_to_write[i].row][coord_to_write[i].col] = ship_num;
	}
}

// Get mark at board location
int8_t board_get_vars(PLAYER *player, int8_t r, int8_t c)
{
	return player->board[r][c];
}

bool check_coords_free(PLAYER *player, coord *coord_to_write, uint8_t num_coords)
{
	for (uint8_t ship_num = 0; ship_num < num_coords; ship_num++)
	{
		if (player->board[coord_to_write[ship_num].row][coord_to_write[ship_num].col] != EMPTY_SPACE)
		{
			return false;
		}
	}
	return true;
}

int8_t get_ship_num(PLAYER *player, coord coord_to_check)
{
	return player->board[coord_to_check.row][coord_to_check.col];
}

bool check_coords_within_board(coord *coord_to_write, uint8_t num_coords)
{
	for (uint8_t ship_num = 0; ship_num < num_coords; ship_num++)
	{
		if (coord_to_write[ship_num].row >= BOARD_R || coord_to_write[ship_num].col >= BOARD_C)
		{
			return false;
		}
	}
	return true;
}
/*this function is used when we have already confirmed that a ship is impossible, either due to going out of bounds or overlapping a previous ship...
  but to show a nice gui, figure out which location*/ 
uint8_t find_invalid_coord(PLAYER *player, coord *coord_to_write, uint8_t num_coords)
{
	for (uint8_t i = 0; i < num_coords; i++)
	{
		if ((player->board[coord_to_write[i].row][coord_to_write[i].col] != EMPTY_SPACE) || !check_coords_within_board(&coord_to_write[i], 1))
		{
			return i;
		}
	}
	assert("find_invalid_coord shouldn't reach the end");
	return EMPTY_SPACE;
}

bool all_coords_valid(PLAYER *player, coord *coord_to_write, uint8_t num_coords)
{
	return check_coords_within_board(coord_to_write, num_coords) && check_coords_free(player, coord_to_write, num_coords);
}

void print_board(PLAYER *player)
{
	printf("board\n");
	for (int8_t r = 0; r < BOARD_R; r++)
	{
		for (int8_t c = 0; c < BOARD_C; c++)
		{

			printf("%d", player->board[r][c] + 1);
		}
		printf("\n");
	}
}