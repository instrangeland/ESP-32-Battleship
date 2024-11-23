#ifndef CONFIG_H_
#define CONFIG_H_

// Original time is 40.0E-3f
#define CONFIG_GAME_TIMER_PERIOD 40.0E-3f

// Board
#define CONFIG_BOARD_R 10 // Rows
#define CONFIG_BOARD_C 10 // Columns
#define CONFIG_BOARD_N 3 // Number of contiguous marks
// #define CONFIG_BOARD_R 5 // Rows
// #define CONFIG_BOARD_C 7 // Columns
// #define CONFIG_BOARD_N 4 // Number of contiguous marks

#define CONFIG_BOARD_SPACES (CONFIG_BOARD_R*CONFIG_BOARD_C)

// Colors
#define CONFIG_BACK_CLR CYAN //original color rgb565(0, 16, 42)
#define CONFIG_BTTLESHIP_CLR rgb565(0, 16, 42)
#define CONFIG_GRID_CLR rgb565(0, 16, 42)
#define CONFIG_MISS_MARK_CLR YELLOW
#define CONFIG_HIT_MARK_CLR RED
#define CONFIG_HIGH_CLR GREEN
#define CONFIG_MESS_CLR rgb565(0, 16, 42)

#endif // CONFIG_H_
