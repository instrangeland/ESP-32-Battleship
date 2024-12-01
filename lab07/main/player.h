#ifndef E36B9453_1CEF_4CB9_8042_0EDDFB8D9CA5
#define E36B9453_1CEF_4CB9_8042_0EDDFB8D9CA5
#include "coord.h"
#include "config.h"
#include <stdbool.h>
#include "ship.h"

typedef enum shot_type {
    MISS,
    HIT,
    NOT_TRIED,
} SHOT_TYPE;

typedef struct player
{
    bool ai;
    SHIP ships[5];
    SHOT_TYPE shot_board[CONFIG_BOARD_R][CONFIG_BOARD_C];
    int8_t board[BOARD_R][BOARD_C];

} PLAYER;

void init_player(PLAYER *player, bool ai);
void init_shot_board(PLAYER *player);
bool attempt_shot(PLAYER *attacking_player, PLAYER *defending_player, coord shot_location);

#endif /* E36B9453_1CEF_4CB9_8042_0EDDFB8D9CA5 */
