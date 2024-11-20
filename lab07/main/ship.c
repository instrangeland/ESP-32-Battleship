#include "ship.h"
#include "coord.h"
#include <stdint.h>
#include <stdlib.h>
coord *get_coordinates(coord start_coordinate, uint8_t length, orientation direction)
{
    coord *coordinates;
    coordinates = (coord *)malloc(sizeof(coord) * length);
    if (direction == VERTICAL)
    {
        for (uint8_t iter_row = 0; iter_row < length; iter_row++)
        {
            coordinates[iter_row].col = start_coordinate.col;
            coordinates[iter_row].row = start_coordinate.row + iter_row;
        }
    }
    else
    {
        for (uint8_t iter_col = 0; iter_col < length; iter_col++)
        {
            coordinates[iter_col].col = start_coordinate.col + iter_col;
            coordinates[iter_col].row = start_coordinate.row;
        }
    }
    return coordinates;
}