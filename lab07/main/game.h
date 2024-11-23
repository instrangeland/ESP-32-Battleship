#ifndef GAME_H_
#define GAME_H_
#include "ship.h"
#include "coord.h"
extern SHIP *ships[];
// Initialize the game logic.
void game_init(void);

// Update the game logic.
void game_tick(void);

#endif // GAME_H_
