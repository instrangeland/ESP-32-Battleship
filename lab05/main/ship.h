#ifndef C779A536_9273_42C1_B781_35E945807AE1
#define C779A536_9273_42C1_B781_35E945807AE1
#include "coord.h"
#include <stdint.h>

typedef struct
{
    bool placed = false;
    Coord start_coord;         // obv
    Coord *placed_coordinates; // a pointer to a list of coordinates it has on the screen. We'll need to make a new one.
    uint8_t length;
} Ship;

#endif /* C779A536_9273_42C1_B781_35E945807AE1 */
