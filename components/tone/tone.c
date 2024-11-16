#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "sound.h"
#include "tone.h"
#include <stdio.h>

#define LOWEST_FREQ 20U // Hz
#define DOUBLE_NYQUIST LOWEST_FREQ * 2
#define MAX_TONE 0xFF
#define HALF_TONE 0x7F
#define HALF 2
#define QUARTER 4
#define TAU 6.28
#define NOT_LAST_QUARTER 3

uint8_t *audio = NULL;
uint32_t sample_rate = 0;
// inits the sound buffer and jank
int32_t tone_init(uint32_t sample_hz)
{
    sample_rate = sample_hz;
    if (sample_hz < (DOUBLE_NYQUIST))
        return -1;
    audio = malloc(sizeof(uint8_t) * (sample_hz) / LOWEST_FREQ);
    sound_init(sample_hz);
    return 0;
}

// Free resources used for tone generation (DAC, etc.).
// Return zero if successful, or non-zero otherwise.
int32_t tone_deinit(void)
{
    free(audio);
    sound_deinit();
    return 0;
}

// Start playing the specified tone.
// tone: one of the enumerated tone types.
// freq: frequency of the tone in Hz.
void tone_start(tone_t tone, uint32_t freq)
{
    uint32_t total_samples = sample_rate / freq;
    float val = 0;
    switch (tone)
    {
    case SQUARE_T:
        printf("square");
        // wow this very square
        for (uint32_t t = 0; t < total_samples; t++)
        {
            if (t < (total_samples) / HALF)
            {
                val = MAX_TONE;
            }
            else
            {
                val = 0;
            }
            audio[t] = (uint8_t)val;
        }
        break;

    case SAW_T:
        // do saw this dumb
        for (uint32_t t = 0; t < total_samples; t++)
        {
            float adjusted_t;
            if (t < NOT_LAST_QUARTER * (total_samples) / QUARTER)
            {
                adjusted_t = (t + (total_samples / QUARTER));
            }
            else
            {
                adjusted_t = (t - (NOT_LAST_QUARTER * total_samples / QUARTER));
            }
            val = MAX_TONE * (adjusted_t / (total_samples));
            audio[t] = (uint8_t)val;
        }
        break;
    case TRIANGLE_T:
        // make triangel
        for (uint32_t t = 0; t < total_samples; t++)
        {
            float adjusted_t;
            if (t < NOT_LAST_QUARTER * (total_samples) / QUARTER)
            {
                adjusted_t = (t + (total_samples / QUARTER));
            }
            else
            {
                adjusted_t = (t - (NOT_LAST_QUARTER * total_samples / QUARTER));
            }

            if (adjusted_t <= total_samples / HALF)
            {
                val = MAX_TONE * (adjusted_t / (total_samples / HALF));
            }
            else
            {
                val = MAX_TONE - MAX_TONE * (adjusted_t / (total_samples / HALF));
            }
            audio[t] = (uint8_t)val;
        }
        break;
    case SINE_T:
        // does sine
        for (uint32_t t = 0; t < total_samples; t++)
        {
            float adjusted_t;

            adjusted_t = TAU * (t) / (total_samples);

            val = sinf(adjusted_t) * HALF_TONE + HALF_TONE;
            audio[t] = (uint8_t)val;
        }
    case LAST_T:
        printf("ERROR PLS HELP WTFETCH");
        break;
    }
    sound_cyclic(audio, total_samples);
}