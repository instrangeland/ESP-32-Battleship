#ifndef BC304F20_A8A6_450A_A2D2_67E4BCB2FC11
#define BC304F20_A8A6_450A_A2D2_67E4BCB2FC11

#include "ship.h"
#include "player.h"
#include "coord.h"
#include "esp_timer.h"

typedef enum game_type
{
    TWO_PLAYER_ONE_HANDHELD,
    TWO_PLAYER_TWO_HANDHELD,
    ONE_PLAYER,
} GAME_TYPE;

// Initialize the game logic.
void game_init(void);

// Update the game logic.
void game_tick(void);



#endif /* BC304F20_A8A6_450A_A2D2_67E4BCB2FC11 */
