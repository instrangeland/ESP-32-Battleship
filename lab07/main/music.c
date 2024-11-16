#include "music.h"

#define SAMPLE_RATE 24000
#define CHANNELS 2
tone_t channel_tones[CHANNELS];
uint32_t channel_frequencies[CHANNELS];
bool channel_active[CHANNELS];

global_t = 0;

uint32_t create_tone(tone_t tone, uint32_t global_t, uint32_t freq, uint8_t *audio)
{
    uint32_t total_samples = SAMPLE_RATE / freq;
    float val = 0;
    switch (tone)
    {
    case SQUARE_T:
        printf("square");
        // wow this very square
        for (uint32_t t = 0; t < total_samples; t++)
        {
            uint32_t new_t = (t + global_t) % freq;
            if (new_t < (total_samples) / HALF)
            {
                val = MAX_TONE;
            }
            else
            {
                val = 0;
            }
            audio[new_t % freq] += (uint8_t)val;
        }
        break;

    case SAW_T:
        // do saw this dumb
        for (uint32_t t = 0; t < total_samples; t++)
        {
            uint32_t new_t = (t + global_t) % freq;
            float adjusted_t;
            if (new_t < NOT_LAST_QUARTER * (total_samples) / QUARTER)
            {
                adjusted_t = (new_t + (total_samples / QUARTER));
            }
            else
            {
                adjusted_t = (new_t - (NOT_LAST_QUARTER * total_samples / QUARTER));
            }
            val = MAX_TONE * (adjusted_t / (total_samples));
            audio[new_t] += (uint8_t)val;
        }
        break;
    case TRIANGLE_T:
        // make triangel
        for (uint32_t t = 0; t < total_samples; t++)
        {
            uint32_t new_t = (t + global_t) % freq;
            float adjusted_t;
            if (new_t < NOT_LAST_QUARTER * (total_samples) / QUARTER)
            {
                adjusted_t = (new_t + (total_samples / QUARTER));
            }
            else
            {
                adjusted_t = (new_t - (NOT_LAST_QUARTER * total_samples / QUARTER));
            }

            if (adjusted_t <= total_samples / HALF)
            {
                val = MAX_TONE * (adjusted_t / (total_samples / HALF));
            }
            else
            {
                val = MAX_TONE - MAX_TONE * (adjusted_t / (total_samples / HALF));
            }
            audio[t] += (uint8_t)val;
        }
        break;
    case SINE_T:
        // does sine
        for (uint32_t t = 0; t < total_samples; t++)
        {
            uint32_t new_t = (t + global_t) % freq;
            float adjusted_t;

            adjusted_t = TAU * (new_t) / (total_samples);

            val = sinf(adjusted_t) * HALF_TONE + HALF_TONE;
            audio[new_t] += (uint8_t)val;
        }
    case LAST_T:
        printf("ERROR PLS HELP WTFETCH");
        break;
    }
    return SAMPLE_RATE / freq;
}

void set_channel(uint8_t channel, uint32_t freq, tone_t tone)
{
    if ((channel >= 0) && (channel < CHANNELS))
    {
        channel_frequencies[channel] = freq;
        channel_tones[channel] = tone;
    }
}

void activate(uint8_t channel)
{
    if ((channel >= 0) && (channel < CHANNELS))
    {
        channel_active[channel] = true;
    }
}

void deactivate(uint8_t channel)
{
    if ((channel >= 0) && (channel < CHANNELS))
    {
        channel_active[channel] = false;
    }
}

void generate_audio()
{
    uint32_t min_t = 0;
    for (uint8_t channel = 0; channel < CHANNELS; channel++)
    {
    }
}