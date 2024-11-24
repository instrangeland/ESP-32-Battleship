#ifndef C48FF4F2_3C91_4E7A_9312_7060C8B7CC04
#define C48FF4F2_3C91_4E7A_9312_7060C8B7CC04
#include "player.h"
#include "ship.h"
#include "coord.h"
#include "config.h"
#include "board.h"
typedef enum shot
{
    HIT,
    MISS,
    INVALID,
    PREVIOUS, // I meant previous as in "we shot this place before, captain"
} ShotType;

ShotType has_shot_here(PLAYER *player, coord coordinate);
ShotType record_in_shots(PLAYER *player, coord coordinate);

#endif /* C48FF4F2_3C91_4E7A_9312_7060C8B7CC04 */
