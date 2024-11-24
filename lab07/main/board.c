#include "board.h"
#include "config.h"
#include "stdio.h"

static uint16_t mark_count;

// Clear the board
void board_clear(PLAYER *player)
{
	for (int8_t r = 0; r < BOARD_R; r++)
		for (int8_t c = 0; c < BOARD_C; c++)
			player->board[r][c] = EMPTY_SPACE;
	mark_count = 0;
}

int8_t board_get_coord(PLAYER *player, COORD check_coord)
{
	return player->board[check_coord.row][check_coord.col];
}

void write_coords(PLAYER *player, COORD *coord_to_write, uint8_t num_coords, int8_t ship_num)
{
	for (uint8_t i = 0; i <= num_coords; i++)
	{
		player->board[coord_to_write[i].row][coord_to_write[i].col] = ship_num;
	}
}

// Get mark at board location
int8_t board_get_vars(PLAYER *player, int8_t r, int8_t c)
{
	return player->board[r][c];
}

bool check_coords_free(PLAYER *player, COORD *coord_to_write, uint8_t num_coords)
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

int8_t get_ship_num(PLAYER *player, COORD coord_to_check)
{
	return player->board[coord_to_check.row][coord_to_check.col];
}

bool check_coords_within_board(COORD *coord_to_write, uint8_t num_coords)
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

uint8_t find_invalid_coord(PLAYER *player, COORD *coord_to_write, uint8_t num_coords)
{
	for (uint8_t i = 0; i < num_coords; i++)
	{
		if (player->board[coord_to_write[i].row][coord_to_write[i].col] != EMPTY_SPACE)
		{
			return i;
		}
	}
	assert("find_invalid_coord shouldn't reach the end");
	return EMPTY_SPACE;
}

bool all_coords_valid(PLAYER *player, COORD *coord_to_write, uint8_t num_coords)
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