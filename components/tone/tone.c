#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "tone.h"

// Define the maxmimum, bias, and wave period marking values for use in computing the tone buffer.
#define BIAS_LEVEL 0x80
#define BIAS_LEVEL_LOW 0x7F
#define MAX_VALUE 0xFF
#define QUARTER_PERIOD 4
#define THIRD_PERIOD 3

/// Buffer size, buffer, and global sample rate for the tone driver.
uint32_t buffer_size = 0;
uint8_t* buffer = NULL;
uint32_t sample_rate = 0;

// Initialize the tone driver. Must be called before using.
// May be called again to change sample rate.
// Processes a sample rate into a buffer size and sets sample_rates for use
// elsewhere in the tone.c file.
// @param sample_hz: sample rate in Hz to playback tone.
// @return zero if successful, or non-zero otherwise.
int32_t tone_init(uint32_t sample_hz){
    // Check if the sample rate is below the allowed frequency. If so, return -1.
    if(sample_hz < 2*LOWEST_FREQ){
        return -1;
    }

    sound_init(sample_hz);
    buffer_size = sample_hz / LOWEST_FREQ;
    sample_rate = sample_hz;
    buffer = malloc(sizeof(uint8_t)*buffer_size);

    return 0;
}

// Free resources used for tone generation (DAC, etc.).
// Determine if the sound buffer was created first
// and then free the buffer and set its pointer to NULL.
// @return zero if successful, or non-zero otherwise.
int32_t tone_deinit(void){

    // First check to see if the buffer is NULL. If not, free the buffer and set it to NULL.
    if(buffer != NULL){
        free(buffer);
        buffer = NULL;
    }

    sound_deinit();
    
    return 0;
}

// Generate a tone, place it in a buffer, and play the specified tone at the
// specified frequency.
// @param tone: one of the enumerated tone types.
// @param freq: frequency of the tone in Hz.
// @return void: Returns from function only if the parameters are invalid.
void tone_start(tone_t tone, uint32_t freq){
    // Make sure the frequency is within the allowed range and that the tone is
    // a specified tone enum in tone.h.
    if(freq < LOWEST_FREQ || freq > sample_rate / 2 || tone > LAST_T){
        return;
    }

    uint32_t period_samples = sample_rate / freq;
    
    // Iterate through each value in the period sample rate and set the buffer index to the appropriate value.
    for(uint32_t i = 0; i < period_samples; i++){

        //Run through a switch to determine which type of tone enum is being used.
        switch(tone){
            case SINE_T:
                buffer[i] = (uint8_t)(BIAS_LEVEL + BIAS_LEVEL_LOW * sinf(2 * M_PI * i / period_samples));
                break;
            case SQUARE_T:
                // If the index is less than half of the samples in the period, set the buffer index to the max value.
                // Otherwise, set the buffer index to 0.
                if(i < period_samples / 2){
                    buffer[i] = MAX_VALUE;
                } else{
                    buffer[i] = 0;
                }
                break;
            case TRIANGLE_T:
                // If the index is less than a quarter of the samples in the period, set the buffer index to
                // the bias level plus the bias level low (to avoid overflow) to get the first quarter of the triangle wave.
                // If the index is less than three quarters of the samples in the period, set the buffer index
                // the bias level plus the lower bias level to compute the lower triangle wave
                // For all other values, set the buffer index to the bias level minus the lower bias level plus the slope
                // calculation. This gives the last quarter of the triangle wave.
                if(i < period_samples / QUARTER_PERIOD){
                    buffer[i] = (uint8_t)(BIAS_LEVEL + (QUARTER_PERIOD * BIAS_LEVEL_LOW * i / period_samples));
                } else if (i < THIRD_PERIOD * period_samples / QUARTER_PERIOD){
                    buffer[i] = (uint8_t)(BIAS_LEVEL + BIAS_LEVEL_LOW - (QUARTER_PERIOD * BIAS_LEVEL_LOW * (i - period_samples / QUARTER_PERIOD)) / period_samples);
                } else {
                    buffer[i] = (uint8_t)(BIAS_LEVEL - BIAS_LEVEL_LOW + (QUARTER_PERIOD * BIAS_LEVEL_LOW * (i - THIRD_PERIOD * period_samples / QUARTER_PERIOD)) / period_samples);
                }
                break;
            case SAW_T:
                buffer[i] = (uint8_t)(BIAS_LEVEL + BIAS_LEVEL_LOW * (2.0f* i / period_samples));
                break;
            default:
                break;
        }
    }

    // Play the sound cyclically.
    sound_cyclic(buffer, period_samples);

}
