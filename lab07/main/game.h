#ifndef GAME_H_
#define GAME_H_
#include "ship.h"
#include "player.h"
#include "coord.h"
#include "esp_timer.h"
extern SHIP *ships[];
extern PLAYER this_player;
extern PLAYER other_player;
// Initialize the game logic.
void game_init(void);

// Update the game logic.
void game_tick(void);

#endif // GAME_H_
