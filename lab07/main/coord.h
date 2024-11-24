#ifndef D03855FB_AC4D_4B64_836C_C376F7966EE5
#define D03855FB_AC4D_4B64_836C_C376F7966EE5
#include <stdint.h>
#include "config.h"
typedef struct coords
{
    uint8_t row;
    uint8_t col;
} coord;

coord random_coord(uint8_t min, uint8_t max);

#endif /* D03855FB_AC4D_4B64_836C_C376F7966EE5 */
