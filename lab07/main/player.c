#include "player.h"
void init_shot_board(PLAYER *player)
{
    for (int8_t r = 0; r < BOARD_R; r++)
        for (int8_t c = 0; c < BOARD_C; c++)
            player->shot_board[r][c] = false;
}

void init_player(PLAYER *player, bool ai)
{
    SHIP carrier = {"Carrier", false, {0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, 5};
    SHIP battleship = {"Battleship", false, {0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, 4};
    SHIP submarine = {"Submarine", false, {0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, 3};
    SHIP cruiser = {"Cruiser", false, {0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, 3};
    SHIP destroyer = {"Destroyer", false, {0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, 2};
    player->ships[0] = carrier;
    player->ships[1] = battleship;
    player->ships[2] = submarine;
    player->ships[3] = cruiser;
    player->ships[4] = destroyer;
    player->ai = ai;
    init_shot_board(player);
}