#ifndef E36B9453_1CEF_4CB9_8042_0EDDFB8D9CA5
#define E36B9453_1CEF_4CB9_8042_0EDDFB8D9CA5
#include "coord.h"
#include "config.h"
#include <stdbool.h>
#include "ship.h"

typedef struct player
{
    bool ai;
    SHIP ships[5];
    bool shot_board[CONFIG_BOARD_R][CONFIG_BOARD_C];
    int8_t board[BOARD_R][BOARD_C];

} PLAYER;


void init_player(PLAYER *player, bool ai);
void init_shot_board(PLAYER *player);

#endif /* E36B9453_1CEF_4CB9_8042_0EDDFB8D9CA5 */
