#ifndef D03855FB_AC4D_4B64_836C_C376F7966EE5
#define D03855FB_AC4D_4B64_836C_C376F7966EE5
#include <stdint.h>
#include "config.h"
#include <stdio.h>
typedef struct coords
{
    uint8_t row;
    uint8_t col;
} coord;

coord random_coord();

void print_coords(coord *coordlist, uint8_t length);

uint8_t coord_to_int(coord coord2conv);
coord int_to_coord(uint8_t num);

#endif /* D03855FB_AC4D_4B64_836C_C376F7966EE5 */
