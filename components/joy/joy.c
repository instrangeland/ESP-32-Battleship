#include "esp_adc/adc_oneshot.h"
#include <stdint.h>
#include "joy.h"

// Set the number of times the joystick center position is read
// when joy_init is called.
#define JOY_CONFIG_READS 5

// Global variables to store the joystick center position
volatile int32_t joy_center_x = 0;
volatile int32_t joy_center_y = 0;
adc_oneshot_unit_handle_t adc1_handle = NULL;

// Initialize the joystick driver. Must be called before use.
// @param void
// @return int32_t, returns 0 if successful, non-zero otherwise.
int32_t joy_init(void) {

    // Determine if the ADC handle is initialized. 
    // If so, return 0. If the handle is NULL, initialize the handle
    if(adc1_handle != NULL){
        return 0;
    }
    else if(adc1_handle == NULL){
    
        // Initialize the joystick ADC handlers and configurations
        adc_oneshot_unit_init_cfg_t init_config1 = {
            .unit_id = ADC_UNIT_1,
            .ulp_mode = ADC_ULP_MODE_DISABLE,
        };
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));


        // Configure the joystick ADC channels using oneshot drivers
        adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
        };
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config));
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_7, &config));

        //Store temporary ADC reads for calibrating the joystick
        int_fast32_t joy_center_config[2][JOY_CONFIG_READS];
        
        // Capture multiple joystick center readings from the ADC, store them in the array
        // temporarily, and add them to the global position variables.
        for (int i = 0; i < JOY_CONFIG_READS; i++){
            adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &joy_center_config[0][i]);
            adc_oneshot_read(adc1_handle, ADC_CHANNEL_7, &joy_center_config[1][i]);
            joy_center_x += joy_center_config[0][i];
            joy_center_y += joy_center_config[1][i];
        }

        // Average the joystick center readings
        joy_center_x /= JOY_CONFIG_READS;
        joy_center_y /= JOY_CONFIG_READS;

        return 0;
    }
    return -1;
    
}

// Free resources used by the joystick (ADC unit).
// @param void: takes no arguments
// @return int32_t: returns 0 if successful, non-zero otherwise.
int32_t joy_deinit(void){

    //Determine if the ADC handle is initialized. If so, delete the handle.
    // If the handle is NULL, return -1.
    if(adc1_handle != NULL){
        ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
        return 0;
    }
    return -1;
    
}

// Determine the joystick displacement from the center position
// and modify memory location values to store the displacement for use in main.c
// @param *dcx: pointer to displacement in x.
// @param *dcy: pointer to displacement in y.
// @return void: returns nothing
void joy_get_displacement(int32_t *dcx, int32_t *dcy){
    int_fast32_t joy_x = 0;
    int_fast32_t joy_y = 0;

    adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &joy_x);
    adc_oneshot_read(adc1_handle, ADC_CHANNEL_7, &joy_y);

    *dcx = joy_x - joy_center_x;
    *dcy = joy_y - joy_center_y;

    // Limit the joystick's x displacement to the maximum value allowed (2048), if determined to be greater or less
    // than the maximum value. Set the displacement value in memory.
    if(*dcx > JOY_MAX_DISP){
        *dcx = JOY_MAX_DISP;
    }
    else if(*dcx < -JOY_MAX_DISP){
        *dcx = -JOY_MAX_DISP;
    }

    // Limit the joystick's y displacement to the maximum value allowed (2048), if determined to be greater or less
    // than the maximum value. Set the y-displacement value in memory.
    if(*dcy > JOY_MAX_DISP){
        *dcy = JOY_MAX_DISP;
    }
    else if(*dcy < -JOY_MAX_DISP){
        *dcy = -JOY_MAX_DISP;
    }
}