#include "board.h"
#include "config.h"

#define BOARD_R CONFIG_BOARD_R // Rows
#define BOARD_C CONFIG_BOARD_C // Columns
#define BOARD_N CONFIG_BOARD_N // Number of contiguous marks
#define BOARD_SPACES CONFIG_BOARD_SPACES

static bool board[BOARD_R][BOARD_C];
static uint16_t mark_count;

// Clear the board
void board_clear(void)
{
	for (int8_t r = 0; r < BOARD_R; r++)
		for (int8_t c = 0; c < BOARD_C; c++)
			board[r][c] = false;
	mark_count = 0;
}

bool board_get_coord(coord check_coord)
{
	return board[check_coord.row][check_coord.col];
}

void write_coords(coord *coord_to_write, uint8_t num_coords)
{
	for (uint8_t ship_num = 0; ship_num < num_coords; ship_num++)
	{
		board[coord_to_write[ship_num].row][coord_to_write[ship_num].col] = true;
	}
}

// Get mark at board location
bool board_get_vars(int8_t r, int8_t c)
{
	return board[r][c];
}

bool check_coords_free(coord *coord_to_write, uint8_t num_coords)
{
	for (uint8_t ship_num = 0; ship_num < num_coords; ship_num++)
	{
		if (board[coord_to_write[ship_num].row][coord_to_write[ship_num].col])
		{
			return false;
		}
	}
	return true;
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

coord *find_full_coord(coord *coord_to_write, uint8_t num_coords)
{
	for (uint8_t ship_num = 0; ship_num < num_coords; ship_num++)
	{
		if (board[coord_to_write[ship_num].row][coord_to_write[ship_num].col])
		{
			return board[coord_to_write[ship_num].row][coord_to_write[ship_num].col];
		}
	}
	return NULL;
}
