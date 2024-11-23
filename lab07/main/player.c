#include "player.h"
void init_shot_board()
{
    for (int8_t r = 0; r < BOARD_R; r++)
        for (int8_t c = 0; c < BOARD_C; c++)
            shot_board[r][c] = false;
}
ShotType has_shot_here(coord coord_to_check)
{
    if (!check_coords_within_board(&coord_to_check, 1))
        return INVALID;
    if (shot_board[coord_to_check.row][coord_to_check.col])
        return PREVIOUS;
    if (board_get_coord(coord_to_check) != EMPTY_SPACE)
        return HIT;
    else
        return MISS;
}
ShotType record_in_shots(coord coord_to_check)
{
    if (!check_coords_within_board(&coord_to_check, 1))
        return INVALID;
    if (shot_board[coord_to_check.row][coord_to_check.col])
        return PREVIOUS;
    if (board_get_coord(coord_to_check) != EMPTY_SPACE)
        return HIT;
    else
        return MISS;
}