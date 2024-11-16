#include "config.h"
#include "board.h"
#include "graphics.h"
#include "nav.h"
#include "com.h"
#include "game.h"
#include "joy.h"
#include "lcd.h"
#include "pin.h"
#include "hw.h"

// Define enums for the game state machine and to manage player turns and winners.
enum gameControl_st {init_st, new_game_st, wait_mark_st, mark_st, wait_restart_st}; 
enum player_control {X, O, no_p};

// Define bit shift and bit mask options for UART data manipulation.
#define BIT_SHIFT 4
#define BIT_MASK 0x0F

// Define global static variables to manage the game state machine and game logic.
static enum gameControl_st currentState;
static int8_t row, column, winningPlayer, com_data;
static uint8_t playerTurn;
static bool setMark, dataCheck;

// Initialize the game variables and logic.
// @param void: None
// @return void: None
void game_init(void){
    currentState = init_st;
    playerTurn = X;
    row = 1;
    column = 1;
    setMark = false;
    winningPlayer = -1;
    dataCheck = false;
    com_data = 0;
}

// Update the game logic through state machine logic.
// @param void: None
// @return void: None
void game_tick(void){
    // Switch statement to control state transitions.
    // Transitions are based on the current state and the input conditions.
    switch(currentState){
        // Initialize the game state machine and transition to the new_game_st state.
        case init_st:
            currentState = new_game_st;
            break;
        // Start a new game and transition to the wait_mark_st state.
        case new_game_st:
            // Determine if the uart communication is still reading data. If it is, keep reading
            // until the RX buffer is empty. 
            while(com_read(&com_data, 1)) {
                continue;
            }

            // Transition to the wait_mark_st state and display a Mealy action message showing the current player's turn.
            currentState = wait_mark_st;
            graphics_drawMessage("It's X's turn.", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
            break;
        // Wait for the player to set a mark on the board and transition to the mark_st state.
        case wait_mark_st:
            // Check if the the mark has been set by the player. If so, 
            // get the current cursor location and write the location to a single byte of data.
            // Pass that data to the UART TX buffer and transition to the mark_st state.
            // If the UART RX buffer is reading data, read the cursor location to the row and column variables.
            if (setMark){
                nav_get_loc(&row, &column);
                com_data = (row << BIT_SHIFT) | column; 
                com_write(&com_data, 1);
                currentState = mark_st;
                setMark = false;
            } else if(com_read(&com_data, 1)){
                row = com_data >> BIT_SHIFT;
                column = com_data & BIT_MASK;
                // Determine if the mark has been set at the row, column location. If it has not been set,
                // set the dataCheck value to true to process the wait_mark_st actions later.
                if(board_get(row, column) == no_m){
                    dataCheck = true;
                }
            } else {
                currentState = wait_mark_st;
            }
            break;
        // Set the mark on the board and transition to the wait_mark_st state.
        case mark_st:
            // Determine if the winningPlayer has been set. If it has been set, determine the winning
            // condition and transition to the wait_restart_st state. Display a Mealy message showing who won the game.
            // If the winningPlayer has not been set, as a Mealy action, display a message showing the next player's 
            // turn and transition to the wait_mark_st state.
            if(winningPlayer == no_p || winningPlayer == X || winningPlayer == O){
                currentState = wait_restart_st;
                // Determine the winning condition and display the appropriate Mealy action win message.
                if(winningPlayer == X){
                    graphics_drawMessage("X wins!", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                } else if(winningPlayer == O){
                    graphics_drawMessage("O wins!", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                } else if(winningPlayer == no_p){
                    graphics_drawMessage("It's a draw!", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                }
            } else {
                currentState = wait_mark_st;
                // Determine which player's turn it is and display the appropriate Mealy action message.
                if(playerTurn == X){
                    graphics_drawMessage("It's X's turn.", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                } else if(playerTurn == O){
                    graphics_drawMessage("It's O's turn.", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                }
            }
            break;
        // Wait for the player to press the start button to restart the game and transition to the new_game_st state.
        case wait_restart_st:
            if(!pin_get_level(HW_BTN_START)){
                currentState = new_game_st;
            } else {
                currentState = wait_restart_st;
            }
            break;
        // Default case to reset the game state machine to the init_st state.
        default:
            currentState = init_st;
    }

    // Switch statement to control state actions.
    // Actions are based on the current state.
    switch(currentState){
        // Initialize the game state machine and set the initial game variables.
        case init_st:
            // Take no action. This state will transition to the new_game_st state upon the first tick.
            break;
        // Start a new game and set the initial game variables.
        case new_game_st:
            lcd_fillScreen(CONFIG_BACK_CLR);
            graphics_drawGrid(CONFIG_GRID_CLR);
            board_clear();
            nav_set_loc(1, 1);
            playerTurn = X;
            winningPlayer = -1;
            break;
        // Wait for the player to set a mark on the board and check if the mark is valid.
        case wait_mark_st:
            setMark = false;
            // Check if button A is pressed. If it is, get the current location of the cursor.
            if(!pin_get_level(HW_BTN_A) || dataCheck){
                if(!pin_get_level(HW_BTN_A)){
                    nav_get_loc(&row, &column);
                } else if(dataCheck){
                    nav_set_loc(row, column);
                    dataCheck = false;
                }
                // Check if the cursor location has been marked previously. If it has not, set the mark based
                // on which player's turn it is.
                if((board_get(row, column) == no_m) && playerTurn == X){
                    setMark = board_set(row, column, X_m);
                } else if((board_get(row, column) == no_m) && playerTurn == O){
                    setMark = board_set(row, column, O_m);
                } 
            }
            break;
        // Set the mark on the board and check if the game has been won or if the game is a draw.
        case mark_st:
            // Check which player's turn it is and draw the appropriate mark on the board.
            if(playerTurn == X){
                graphics_drawX(row, column, CONFIG_MARK_CLR);
                // Check if player X has won the game after drawing a mark.
                // If they have won, set the winningPlayer variable player X.
                // If the game is a draw, set the winningPlayer variable to no_p. If neither case is true,
                // move on to player O's turn.
                if(board_winner(X_m)){
                    winningPlayer = X;
                } else if(board_mark_count() == CONFIG_BOARD_SPACES){
                    winningPlayer = no_p;
                } else{
                    playerTurn = O;
                }
            } else if(playerTurn == O){
                graphics_drawO(row, column, CONFIG_MARK_CLR);
                // Check if player O has won the game after drawing a mark.
                // If they have won, set the winningPlayer variable to player O.
                // If the game is a draw, set the winningPlayer variable to no_p. If neither case is true,
                // move on to player X's turn.
                if(board_winner(O_m)){
                    winningPlayer = O;
                } else if(board_mark_count() == CONFIG_BOARD_SPACES){
                    winningPlayer = no_p;
                } else{
                    playerTurn = X;
                }
            }
            break;
        // Wait for the player to press the start button to restart the game.
        case wait_restart_st:
            // Take no action in this state. The game will wait at the transition case until the start button is pressed.
            break;
        // Default case to reset the game state machine to the init_st state.
        default:
            break;
    }
}