#include "coord.h"
#include <stdlib.h>

coord random_coord()
{
    coord rand_coord = {RAND_INT(0, BOARD_R), RAND_INT(0, BOARD_C)};
    return rand_coord;
}

void print_coords(coord *coordlist, uint8_t length)
{
    printf("len: %d\n", length);
    for (uint8_t i = 0; i < length; i++)
    {
        printf("row: %d col: %d \n", coordlist[i].row, coordlist[i].col);
    }
}