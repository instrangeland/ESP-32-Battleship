#ifndef C1BC6F03_7B43_423D_955E_B705B0780E2F
#define C1BC6F03_7B43_423D_955E_B705B0780E2F
#include "coord.h"
#include <stdbool.h>

typedef enum e_orientation
{
    VERTICAL,
    HORIZONTAL,
} orientation;

typedef struct ship
{
    char *name;
    bool placed;
    coord start_coord;
    coord coordinates[5];
    uint8_t length;
} SHIP;

void get_coordinates(coord *write_array, coord start_coordinate, uint8_t length, orientation direction);

#endif /* C1BC6F03_7B43_423D_955E_B705B0780E2F */
