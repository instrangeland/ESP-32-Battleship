#include "esp_attr.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gptimer.h"
#include "hw.h"
#include "lcd.h"
#include "pin.h"
#include "watch.h"

static const char *TAG = "lab03"; 

#define ISR_RESET_COUNTER 500 // Reset the ISR counters every 500 iterations
#define ALARM_COUNT_PERIOD 10000 // period = 10ms @resolution 1MHz
#define TIMER_RESOLUTION_HZ 1 * 1000 * 1000 // 1MHz, 1 tick = 1us

//Global variables for the timer_callback function & ISR measurements
volatile bool running = false; 
volatile uint16_t timer_ticks = 0;
volatile int64_t isr_max; // Maximum ISR execution time (uS)
volatile int32_t isr_cnt; // Cout of ISR invocations


// Timer callback function to determine which control button has been pressed, if the timer is running,
// and to increment the timer_ticks by 1 if the timer is running. This function returns the running status of the timer.
// @param timer: The timer handle, defined and configured by the user.
// @param edata: The alarm event data
// @param user_ctx: The user context, usually NULL
// @return: The running status of the timer
static bool IRAM_ATTR timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx){
    
    uint64_t start = esp_timer_get_time();

    // Check if button A has been pressed. If A has been pressed, set running to true (start the timer).
    // If B has been pressed, set running to false (pause the timer). 
    // If START has been pressed, set running to false and reset timer_ticks to 0 (reset the timer).
    if(!pin_get_level(HW_BTN_A)){
        running = true;
    }
    else if(!pin_get_level(HW_BTN_B)){
        running = false;
    }
    else if(!pin_get_level(HW_BTN_START)){
        running = false;
        timer_ticks = 0;
    }

    // Determine if the timer is running. If so, increment timer_ticks by 1.
    if(running){
        timer_ticks++;
    }  


    uint64_t end = esp_timer_get_time();
    uint64_t isr_time = end - start;

    // Update the maximum ISR execution time if the current ISR 
    // time is greater than the maximum ISR time.
    if(isr_time > isr_max){
        isr_max = isr_time;
    }

    isr_cnt++;

    return running;
}


int app_main(){

    uint64_t start, end;

    //Reset the A, B, and START buttons and determine running time.
    start = esp_timer_get_time();

    pin_reset(HW_BTN_A);
    pin_reset(HW_BTN_B);
    pin_reset(HW_BTN_START);

    pin_input(HW_BTN_A, true);
    pin_input(HW_BTN_B, true);
    pin_input(HW_BTN_START, true);   

    end = esp_timer_get_time();
    ESP_LOGI(TAG, "IO Pin Configuration Time: %lld us", end - start);


    // Configure the timer and set the alarm action.
    start = esp_timer_get_time();

    gptimer_handle_t timer = NULL;
    gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = TIMER_RESOLUTION_HZ, // 1MHz, 1 tick = 1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &timer));

    gptimer_event_callbacks_t timer_event_callbacks = {
        .on_alarm = timer_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer, &timer_event_callbacks, NULL));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0, // counter will reload with 0 on alarm event
        .alarm_count = ALARM_COUNT_PERIOD, 
        .flags.auto_reload_on_alarm = true, // enable auto-reload
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer, &alarm_config));

    ESP_ERROR_CHECK(gptimer_enable(timer));
    ESP_ERROR_CHECK(gptimer_start(timer));

    end = esp_timer_get_time();
    ESP_LOGI(TAG, "Stopwatch Configuration Time: %lld us", end - start);

    start = esp_timer_get_time();
    ESP_LOGI(TAG, "Stopwatch update");
    end = esp_timer_get_time();
    ESP_LOGI(TAG, "ESP_LOGI 'Stopwatch update' time: %lld us", end - start);

    lcd_init(); //Initialize LCD display
    watch_init(); // Initialize stopwatch face

    // Start an infinite for loop to update the watch face with the timer_ticks value
    for(;;){
        watch_update(timer_ticks);

        // Print the maximum ISR execution time every 5 seconds (500 iterations)
        // Set the isr_cnt and isr_max to 0 after printing to reset the counts
        if(isr_cnt >= ISR_RESET_COUNTER){
            ESP_LOGI(TAG, "ISR maximum execution time: %lld us", isr_max);
            isr_cnt = 0;
            isr_max = 0;
        }
    }   

    return 0;
}