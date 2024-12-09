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

uint8_t coord_to_int(coord coord2conv) {
    return coord2conv.col * 10 + coord2conv.row;
}
coord int_to_coord(uint8_t num) {
    coord coord_from_num;
    coord_from_num.row = num % 10;
    num -= num % 10;
    coord_from_num.col = num / 10;
    return coord_from_num;
}