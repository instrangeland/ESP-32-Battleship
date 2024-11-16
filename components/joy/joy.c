#include "joy.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"

bool already_init = false;

int16_t center_x = 0;
int16_t center_y = 0;
adc_oneshot_unit_handle_t adc_handle = NULL;
// Initialize the joystick driver. Must be called before use.
// May be called multiple times. Return if already initialized.
// Return zero if successful, or non-zero otherwise.
int32_t joy_init(void)
{
    // this is an if statement
    if (adc_handle == NULL)
    {
        adc_oneshot_unit_init_cfg_t init_config1 = {
            .unit_id = ADC_UNIT_1,
            .ulp_mode = ADC_ULP_MODE_DISABLE,
        };
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc_handle));

        adc_oneshot_chan_cfg_t config = {
            .bitwidth = ADC_BITWIDTH_DEFAULT,
            .atten = ADC_ATTEN_DB_12,
        };
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &config));
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_7, &config));
        printf("calibrating... please wait:\n");

        int_fast32_t averaging_center_x = 0;
        int_fast32_t averaging_center_y = 0;
        int_fast32_t received_val = 0;
        // this one is a for loop
        for (uint8_t sample = 0; sample < JOYSTICK_CENTER_SAMPLES; sample++)
        {
            ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &received_val)); // x
            averaging_center_x += received_val;
            ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_7, &received_val)); // y
            averaging_center_y += received_val;
            printf("sample %d complete\n", sample);
        }
        center_x = averaging_center_x / JOYSTICK_CENTER_SAMPLES;
        center_y = averaging_center_y / JOYSTICK_CENTER_SAMPLES;
        printf("sampling complete\n");
    }
    return 0;
}

// Free resources used by the joystick (ADC unit).
// Return zero if successful, or non-zero otherwise.
int32_t joy_deinit(void)
{
    if (adc_handle != NULL)
    {
        ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
        adc_handle = NULL;
    }
    return 0;
}

// Get the joystick displacement from center position.
// Displacement values range from 0 to +/- JOY_MAX_DISP.
// This function is not safe to call from an ISR context.
// Therefore, it must be called from a software task context.
// *dcx: pointer to displacement in x.
// *dcy: pointer to displacement in y.
void joy_get_displacement(int32_t *dcx, int32_t *dcy)
{
    int_fast32_t input = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &input)); // x
    *dcx = input;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_7, &input)); // y
    *dcy = input;
    *dcx -= center_x;
    *dcy -= center_y;
}