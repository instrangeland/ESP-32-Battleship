#include "coord.h"
#include <stdlib.h>

coord random_coord(uint8_t min, uint8_t max)
{
    coord rand_coord = {RAND_INT(0, BOARD_R), RAND_INT(0, BOARD_C)};
    return rand_coord;
}
