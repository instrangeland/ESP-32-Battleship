#include "player.h"

void init_shot_board(PLAYER *player)
{
    for (int8_t r = 0; r < BOARD_R; r++)
        for (int8_t c = 0; c < BOARD_C; c++)
            player->shot_board[r][c] = NOT_TRIED;
}

bool test_loss(PLAYER *player)
{
    for (uint8_t ship_num = 0; ship_num < 5; ship_num++)
    {
        if (player->ships[ship_num].hits_remaining > 0)
            return false;
    }
    return true;
}

SHOT_TYPE get_shot_location(PLAYER *player, coord check_coord)
{
    return player->shot_board[check_coord.row][check_coord.col];
}

bool attempt_shot(PLAYER *attacking_player, PLAYER *defending_player, coord shot_location)
{
    if (defending_player->board[shot_location.row][shot_location.col] != EMPTY_SPACE)
    {
        defending_player->ships[defending_player->board[shot_location.row][shot_location.col]].hits_remaining--;
        defending_player->board[shot_location.row][shot_location.col] = MAIN_BOARD_HIT;
        attacking_player->shot_board[shot_location.row][shot_location.col] = HIT;
        return true;
    }
    else
    {
        attacking_player->shot_board[shot_location.row][shot_location.col] = MISS;
        return false;
    }
}

void init_player(PLAYER *player, bool ai)
{
    SHIP carrier = {"Carrier", false, {0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, 5, 5};
    SHIP battleship = {"Battleship", false, {0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, 4, 4};
    SHIP submarine = {"Submarine", false, {0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, 3, 3};
    SHIP cruiser = {"Cruiser", false, {0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, 3, 3};
    SHIP destroyer = {"Destroyer", false, {0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, 2, 2};
    player->ships[0] = carrier;
    player->ships[1] = battleship;
    player->ships[2] = submarine;
    player->ships[3] = cruiser;
    player->ships[4] = destroyer;
    player->ai = ai;
    init_shot_board(player);
}