#include "ship.h"
#include "coord.h"
#include <stdint.h>
#include <stdlib.h>
void get_coordinates(COORD *write_array, COORD start_coordinate, uint8_t length, orientation direction)
{
    if (direction == VERTICAL)
    {
        for (uint8_t iter_row = 0; iter_row < length; iter_row++)
        {
            write_array[iter_row].col = start_coordinate.col;
            write_array[iter_row].row = start_coordinate.row + iter_row;
        }
    }
    else
    {
        for (uint8_t iter_col = 0; iter_col < length; iter_col++)
        {
            write_array[iter_col].col = start_coordinate.col + iter_col;
            write_array[iter_col].row = start_coordinate.row;
        }
    }
}