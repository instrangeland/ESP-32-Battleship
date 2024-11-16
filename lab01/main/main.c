#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lcd.h"
#include "pac.h"

static const char *TAG = "lab01";

#define DELAYMS(ms) \
	vTaskDelay(((ms)+(portTICK_PERIOD_MS-1))/portTICK_PERIOD_MS)

//----------------------------------------------------------------------------//
// Car Implementation - Begin
//----------------------------------------------------------------------------//

// Car constants
#define CAR_CLR rgb565(220,30,0)
#define WINDOW_CLR rgb565(180,210,238)
#define TIRE_CLR BLACK
#define HUB_CLR GRAY

#define CAR_W 60
#define CAR_H 32

#define BODY_X0 0
#define BODY_Y0 12
#define BODY_X1 59
#define BODY_Y1 24

#define CAB_X0 1
#define CAB_Y0 0
#define CAB_X1 39
#define CAB_Y1 11

#define RTIRE_X0 11
#define RTIRE_Y0 24
#define FTIRE_X0 48
#define FTIRE_Y0 24
#define HUB_R 4
#define TIRE_R 7

#define HOOD_X0 40
#define HOOD_Y0 9
#define HOOD_X1 59
#define HOOD_Y1 11

#define RWINDOW_X0 3
#define RWINDOW_X1 18
#define FWINDOW_X0 21
#define FWINDOW_X1 37
#define WINDOW_Y0 1
#define WINDOW_Y1 8
#define WINDOW_R 2

/**
 * @brief Draw a car at the specified location.
 * @param x      Top left corner X coordinate.
 * @param y      Top left corner Y coordinate.
 * @details Draw the car components relative to the anchor point (top, left).
 */
void drawCar(coord_t x, coord_t y)
{
	lcd_fillRect2(BODY_X0+x, BODY_Y0+y, BODY_X1+x, BODY_Y1+y, CAR_CLR); //Bottom rectangle of car
	lcd_fillRect2(CAB_X0+x, CAB_Y0+y, CAB_X1+x, CAB_Y1+y, CAR_CLR); //Top rectangle of car
	lcd_fillTriangle(HOOD_X0+x, HOOD_Y0+y, HOOD_X0+x, HOOD_Y1+y, HOOD_X1+x, HOOD_Y1+y, CAR_CLR); //Hood of the car
	lcd_fillCircle(RTIRE_X0+x, RTIRE_Y0+y, TIRE_R, TIRE_CLR); //Rear tire of the car
	lcd_fillCircle(RTIRE_X0+x, RTIRE_Y0+y, HUB_R, HUB_CLR); //Rear tire hub of the car
	lcd_fillCircle(FTIRE_X0+x, FTIRE_Y0+y, TIRE_R, TIRE_CLR); //Front tire of the car
	lcd_fillCircle(FTIRE_X0+x, FTIRE_Y0+y, HUB_R, HUB_CLR); //Front tire hub of the car
	lcd_fillRoundRect2(RWINDOW_X0+x, WINDOW_Y0+y, RWINDOW_X1+x, WINDOW_Y1+y, WINDOW_R, WINDOW_CLR); //Rear window of the car
	lcd_fillRoundRect2(FWINDOW_X0+x, WINDOW_Y0+y, FWINDOW_X1+x, WINDOW_Y1+y, WINDOW_R, WINDOW_CLR); //Front window of the car
}

//----------------------------------------------------------------------------//
// Car Implementation - End
//----------------------------------------------------------------------------//

// Main display constants
#define BACKGROUND_CLR rgb565(0,60,90)
#define TITLE_CLR GREEN
#define STATUS_CLR WHITE
#define STR_BUF_LEN 12 // string buffer length
#define FONT_SIZE 2
#define FONT_W (LCD_CHAR_W*FONT_SIZE)
#define FONT_H (LCD_CHAR_H*FONT_SIZE)
#define STATUS_W (FONT_W*3)

#define WAIT 2000 // milliseconds
#define DELAY_EX3 20 // milliseconds

// Object position and movement
#define OBJ_X 100
#define OBJ_Y 100
#define OBJ_MOVE 3 // pixels


void app_main(void)
{
	ESP_LOGI(TAG, "Start up");
	lcd_init();
	lcd_fillScreen(BACKGROUND_CLR);
	lcd_setFontSize(FONT_SIZE);
	lcd_drawString(0, 0, "Hello World! (lcd)", TITLE_CLR);
	printf("Hello World! (terminal)\n");

	lcd_fillScreen(BACKGROUND_CLR);
	lcd_drawString(0, 0, "Exercise 1", TITLE_CLR);
	drawCar(OBJ_X, OBJ_Y);
	DELAYMS(WAIT);

	//Exercise 2 - Draw moving car (Method 1), one pass across display.
	// Clear the entire display and redraw all objects each iteration.
	// Use a loop and increment x by OBJ_MOVE each iteration.
	// Start x off screen (negative coordinate).
	for(coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE){
		lcd_fillScreen(BACKGROUND_CLR);
		lcd_drawString(0, 0, "Exercise 2", TITLE_CLR);
		drawCar(x, OBJ_Y);
		char str[STR_BUF_LEN];
		sprintf(str, "%3ld", x);
		lcd_drawString(0, LCD_H-FONT_H, str, STATUS_CLR);
	}


	// Exercise 3 - Draw moving car (Method 2), one pass across display.
	// Move by erasing car at old position, then redrawing at new position.
	// Objects that don't change or move are drawn once.

	lcd_fillScreen(BACKGROUND_CLR);
	lcd_drawString(0, 0, "Exercise 3", TITLE_CLR);

	// Draw car at each position, then erase and redraw at next position.
	for(coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE){
		lcd_fillRect(x-OBJ_MOVE, OBJ_Y, CAR_W, CAR_H, BACKGROUND_CLR);
		drawCar(x, OBJ_Y);
		lcd_fillRect(0, LCD_H-FONT_H, STATUS_W, LCD_H, BACKGROUND_CLR);
		char str[STR_BUF_LEN];
		sprintf(str, "%3ld", x);
		lcd_drawString(0, LCD_H-FONT_H, str, STATUS_CLR);
		DELAYMS(DELAY_EX3);
	}


	// Exercise 4 - Draw moving car (Method 3), one pass across display.
	// First, draw all objects into a cleared, off-screen frame buffer.
	// Then, transfer the entire frame buffer to the screen.

	lcd_frameEnable();

	// Draw car at each position, then erase and redraw at next position using frames.
	for(coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE){
		lcd_fillScreen(BACKGROUND_CLR);
		lcd_drawString(0, 0, "Exercise 4", TITLE_CLR);
		drawCar(x, OBJ_Y);
		char str[STR_BUF_LEN];
		sprintf(str, "%3ld", x);
		lcd_drawString(0, LCD_H-FONT_H, str, STATUS_CLR);
		lcd_writeFrame();
	}

	lcd_frameDisable();

	// Exercise 5 - Draw an animated Pac-Man moving across the display.
	// Use Pac-Man sprites instead of the car object.
	// Cycle through each sprite when moving the Pac-Man character.

	lcd_frameEnable();

	uint16_t i = 0;
	const uint8_t pidx[] = {0, 1, 2, 1};
	
	//Start an infinite loop for the Pac-Man animation
	for(;;){
		// Draw Pac-Man sprites at each position, change animation, then erase and redraw at next position using frames.
		for(coord_t x = -PAC_W; x <= LCD_W; x += OBJ_MOVE){
			lcd_fillScreen(BACKGROUND_CLR);
			lcd_drawString(0, 0, "Exercise 5", TITLE_CLR);
			lcd_drawBitmap(x, OBJ_Y, pac[pidx[i++ % sizeof(pidx)]], PAC_W, PAC_H, YELLOW);
			char str[STR_BUF_LEN];
			sprintf(str, "%3ld", x);
			lcd_drawString(0, LCD_H-FONT_H, str, STATUS_CLR);
			lcd_writeFrame();
		}
	}

	lcd_frameDisable();

}
