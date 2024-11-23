#include "board.h"
#include "config.h"



static int8_t board[BOARD_R][BOARD_C];
static uint16_t mark_count;

// Clear the board
void board_clear(void)
{
	for (int8_t r = 0; r < BOARD_R; r++)
		for (int8_t c = 0; c < BOARD_C; c++)
			board[r][c] = EMPTY_SPACE;
	mark_count = 0;
}

int8_t board_get_coord(coord check_coord)
{
	return board[check_coord.row][check_coord.col];
}

void write_coords(coord *coord_to_write, uint8_t num_coords, int8_t ship_num)
{
	for (uint8_t ship_num = 0; ship_num < num_coords; ship_num++)
	{
		board[coord_to_write[ship_num].row][coord_to_write[ship_num].col] = ship_num;
	}
}

// Get mark at board location
int8_t board_get_vars(int8_t r, int8_t c)
{
	return board[r][c];
}

bool check_coords_free(coord *coord_to_write, uint8_t num_coords)
{
	for (uint8_t ship_num = 0; ship_num < num_coords; ship_num++)
	{
		if (board[coord_to_write[ship_num].row][coord_to_write[ship_num].col] != EMPTY_SPACE)
		{
			return false;
		}
	}
	return true;
}

int8_t get_ship_num(coord coord_to_check)
{
	return board[coord_to_check.row][coord_to_check.col];
}

bool check_coords_within_board(coord *coord_to_write, uint8_t num_coords)
{
	printf("index %d\n", num_coords);
	for (uint8_t ship_num = 0; ship_num < num_coords; ship_num++)
	{
		if (coord_to_write[ship_num].row >= BOARD_R || coord_to_write[ship_num].col >= BOARD_C)
		{
			return false;
		}
	}
	return true;
}

uint8_t find_full_coord(coord *coord_to_write, uint8_t num_coords)
{
	for (uint8_t ship_num = 0; ship_num < num_coords; ship_num++)
	{
		if (board[coord_to_write[ship_num].row][coord_to_write[ship_num].col])
		{
			return ship_num;
		}
	}
	return EMPTY_SPACE;
}

bool all_coords_valid(coord *coord_to_write, uint8_t num_coords)
{
	return check_coords_within_board(coord_to_write, num_coords) && check_coords_free(coord_to_write, num_coords);
}