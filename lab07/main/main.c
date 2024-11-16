// https://mixbutton.com/mixing-articles/music-note-to-frequency-chart/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"

#include "hw.h"
#include "lcd.h"
#include "pin.h"
#include "joy.h"

static const char *TAG = "lab05";

// Main application
void app_main(void)
{
	ESP_LOGI(TAG, "Start up");

	// Configure I/O pins for buttons
	pin_reset(HW_BTN_A);
	pin_input(HW_BTN_A, true);
	pin_reset(HW_BTN_B);
	pin_input(HW_BTN_B, true);
	pin_reset(HW_BTN_MENU);
	pin_input(HW_BTN_MENU, true);
	pin_reset(HW_BTN_OPTION);
	pin_input(HW_BTN_OPTION, true);
	pin_reset(HW_BTN_SELECT);
	pin_input(HW_BTN_SELECT, true);
	pin_reset(HW_BTN_START);
	pin_input(HW_BTN_START, true);

	lcd_init();					   // Initialize LCD display and device handle
	lcd_fillScreen(SBG_CL);		   // Clear the screen
	lcd_setFontBackground(SBG_CL); // Set font background

	joy_init(); // Initialize joystick driver
	draw_joystick_status();

	// Schedule the tick() function to run at the specified period.
	// The joy_get_displacement() function is not safe to call from an
	// ISR context. Therefore, the update() function must be called from
	// a software task context provided by the RTOS.
	update_timer = xTimerCreate(
		"update_timer",		   // Text name for the timer.
		pdMS_TO_TICKS(UP_PER), // The timer period in ticks.
		pdTRUE,				   // Auto-reload the timer when it expires.
		NULL,				   // No need for a timer ID.
		update				   // Tick function (callback) when timer expires.
	);
	if (update_timer == NULL)
	{
		ESP_LOGE(TAG, "Error creating timer");
		return;
	}
	if (xTimerStart(update_timer, pdMS_TO_TICKS(TIME_OUT)) != pdPASS)
	{
		ESP_LOGE(TAG, "Error starting timer");
		return;
	}
}
