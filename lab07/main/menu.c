#include "menu.h"
#include "config.h"
#include "lcd.h"
#include "pin.h"
#include "hw.h"
#include "game.h"

#define MENU_FONT_SIZE 2
#define MESS_FNT_SIZE 1
#define MESS_X 20
#define MESS_Y 110
#define MESS_X_2 130
#define MESS_HEAD_X 25
#define MESS_HEAD_Y 10


enum MENU_STATE
{
    MENU_INIT,
    MENU_MAIN,
    MENU_START,
    MENU_IDLE_PLAYER_1,
    MENU_IDLE_PLAYER_2
};

static enum MENU_STATE menuState;
static bool pressed, gameType;
static uint64_t btns;


void menu_init(void) {
    menuState = MENU_INIT;
    pressed = false;
    gameType = false;
    btns = 0;
}

void menu_tick(void) {
    switch (menuState)
    {
    case MENU_INIT:
        menuState = MENU_MAIN;
        break;
    case MENU_MAIN:
        menuState = MENU_START;
        break;
    case MENU_START:
        if(!pin_get_level(HW_BTN_START) && !pressed && btns) {
            pressed = true;
            if(!gameType) {
                menuState = MENU_IDLE_PLAYER_1;
                game_init();
            } else {
                menuState = MENU_IDLE_PLAYER_2;
            }
        } else if(pressed && !btns) {
            pressed = false;
            menuState = MENU_START;
        } else {
            menuState = MENU_START;
        }
        break;
    case MENU_IDLE_PLAYER_1:
        break;
    case MENU_IDLE_PLAYER_2:
        break;
    default:
        break;
    }

    switch (menuState) {
    case MENU_INIT:
        break;
    case MENU_MAIN:
        lcd_fillScreen(CONFIG_BACK_CLR);
        lcd_setFontSize(MENU_FONT_SIZE);
        lcd_drawString(MESS_HEAD_X, MESS_HEAD_Y, "Welcome to Battleship!", CONFIG_MESS_CLR);
        lcd_setFontSize(MESS_FNT_SIZE);
        lcd_drawString(MESS_X, 35, "              @@@@@@@@@", CONFIG_MESS_CLR);
        lcd_drawString(MESS_X, 45, "                    __\\_\\__", CONFIG_MESS_CLR);
        lcd_drawString(MESS_X, 55, "         ___________|_____|___________", CONFIG_MESS_CLR);
        lcd_drawString(MESS_X, 65, "          \\                         /", CONFIG_MESS_CLR);
        lcd_drawString(MESS_X, 75, "           \\  O  O  O  O  O  O  O  /", CONFIG_MESS_CLR);
        lcd_drawString(MESS_X, 85, "^^^^^^^^^^^^\\_____________________/^^^^^^^^^^^", CONFIG_MESS_CLR);

        lcd_setFontSize(MESS_FNT_SIZE);
        lcd_drawString(MESS_X, MESS_Y, "Press Start to Begin a 1 player game", CONFIG_MESS_CLR);
        lcd_drawString(MESS_X, MESS_X_2, "Press Select to change game type", CONFIG_MESS_CLR);
        break;
    case MENU_START:
        btns = ~pin_get_in_reg() & HW_BTN_MASK;
        if(!pin_get_level(HW_BTN_SELECT) && !pressed && btns) {
            pressed = true;
            gameType = !gameType;
            lcd_fillScreen(CONFIG_BACK_CLR);
            lcd_setFontSize(MENU_FONT_SIZE);
            lcd_drawString(MESS_HEAD_X, MESS_HEAD_Y, "Welcome to Battleship!", CONFIG_MESS_CLR);
            lcd_setFontSize(MESS_FNT_SIZE);
            lcd_drawString(MESS_X, 35, "              @@@@@@@@@", CONFIG_MESS_CLR);
            lcd_drawString(MESS_X, 45, "                    __\\_\\__", CONFIG_MESS_CLR);
            lcd_drawString(MESS_X, 55, "         ___________|_____|___________", CONFIG_MESS_CLR);
            lcd_drawString(MESS_X, 65, "          \\                         /", CONFIG_MESS_CLR);
            lcd_drawString(MESS_X, 75, "           \\  O  O  O  O  O  O  O  /", CONFIG_MESS_CLR);
            lcd_drawString(MESS_X, 85, "^^^^^^^^^^^^\\_____________________/^^^^^^^^^^^", CONFIG_MESS_CLR);
            if(gameType) {
                lcd_drawString(MESS_X, MESS_Y, "Press Select to change to a 1 player game", CONFIG_MESS_CLR); 
                lcd_drawString(MESS_X, MESS_X_2, "Press Start to Begin a 2 player game", CONFIG_MESS_CLR);
            } else {
                lcd_drawString(MESS_X, MESS_Y, "Press Select to change to a 2 player game", CONFIG_MESS_CLR);
                lcd_drawString(MESS_X, MESS_X_2, "Press Start to Begin a 1 player game", CONFIG_MESS_CLR);
            }
        } else if(pressed && !btns) {
            pressed = false;
        }   
        break;
    case MENU_IDLE_PLAYER_1:
        game_tick();
        break;
    case MENU_IDLE_PLAYER_2:
        break;
    default:
        break;   
    }
}


