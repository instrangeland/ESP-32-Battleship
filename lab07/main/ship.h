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
    COORD start_coord;
    COORD coordinates[5];
    uint8_t length;
} SHIP;

void get_coordinates(COORD *write_array, COORD start_coordinate, uint8_t length, orientation direction);

#endif /* C1BC6F03_7B43_423D_955E_B705B0780E2F */
