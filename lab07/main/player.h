#ifndef E36B9453_1CEF_4CB9_8042_0EDDFB8D9CA5
#define E36B9453_1CEF_4CB9_8042_0EDDFB8D9CA5
#include "coord.h"
#include "board.h"
#include "config.h"

bool shot_board[CONFIG_BOARD_R][CONFIG_BOARD_C];

typedef enum shot
{
    HIT,
    MISS,
    INVALID,
    PREVIOUS,
} ShotType;

void init_shot_board();
ShotType has_shot_here(coord coordinate);
ShotType record_in_shots(coord coordinate);

#endif /* E36B9453_1CEF_4CB9_8042_0EDDFB8D9CA5 */
