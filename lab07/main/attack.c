#include "attack.h"
ShotType has_shot_here(PLAYER *player, COORD coord_to_check)
{
    if (!check_coords_within_board(&coord_to_check, 1))
        return INVALID;
    if (player->shot_board[coord_to_check.row][coord_to_check.col])
        return PREVIOUS;
    if (board_get_coord(player, coord_to_check) != EMPTY_SPACE)
        return HIT;
    return MISS;
}
ShotType record_in_shots(PLAYER *player, COORD coord_to_check)
{
    if (!check_coords_within_board(&coord_to_check, 1))
        return INVALID;
    if (player->shot_board[coord_to_check.row][coord_to_check.col])
        return PREVIOUS;
    if (board_get_coord(player, coord_to_check) != EMPTY_SPACE)
        return HIT;
    return MISS;
}