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

// Get mark at board location
bool board_get_vars(int8_t r, int8_t c)
{
	return board[r][c];
}
