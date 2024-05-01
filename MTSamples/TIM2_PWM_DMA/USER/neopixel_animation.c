/**
 * @file neopixel_anim.c
 * @author Bayron Cabrera (bayron.nanez@gmail.com)
 * @brief  Finite State Machine for LED animations
 * @version 0.1
 * @date 2024-05-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "neopixel_animation.h"

#define DEBUG_BUFFER_SIZE 100
#define NEOPIXEL_DBG 1

#if NEOPIXEL_DBG
#include <stdio.h>
#define NEO_ANIM_DBG_MSG(fmt, ...) do { \
  static char dbgBuff[DEBUG_BUFFER_SIZE]; \
  snprintf(dbgBuff, DEBUG_BUFFER_SIZE, (fmt), ##__VA_ARGS__); \
  Uart_Put_Buff(dbgBuff, strlen(dbgBuff)); \
} while(0)
#else
#define NEO_ANIM_DBG_MSG(fmt, ...) do { } while(0)
#endif

// Private functions
uint32_t Wheel(uint8_t WheelPos);
uint32_t RGB(uint8_t r, uint8_t g, uint8_t b);
uint32_t NextColor(uint32_t current_color);


void neopixel_anim_init(neopixel_animation_t *anim, neopixel_t *neopixel)
{
    NEO_ANIM_DBG_MSG("Neopixel Animation Init\r\n");

    anim->neopixel = neopixel;
    anim->state = ANIM_IDLE;
    anim->led_index = 0;
    anim->red = 0;
    anim->green = 0;
    anim->blue = 0;
    anim->brightness = 100;
    anim->color = 0;
    anim->delay = 0;
    anim->last_update = 0;
    anim->last_cycle = 0;
    anim->last_blink = 0;
    anim->last_chase = 0;
    anim->last_wipe = 0;
    anim->last_rainbow = 0;
    anim->last_rainbow_cycle = 0;
    anim->last_fade = 0;
    anim->fade_delay = 0;
    anim->fade_step = 0;
    anim->fade_max = 0;
    anim->fade_min = 0;
    anim->fade_dir = 0;
}

void neopixel_anim_update(neopixel_animation_t *anim)
{
    uint32_t current_time = Get_Systick_Cnt();
    uint32_t elapsed_time = current_time - anim->last_update;

    switch (anim->state)
    {
    case ANIM_IDLE:
        break;
    case ANIM_FADE_IN:
        if (elapsed_time > anim->fade_delay)
        {
            anim->fade_step += 1;
            if (anim->fade_step >= anim->fade_max)
            {
                anim->fade_step = anim->fade_max;
                anim->state = ANIM_IDLE;
            }
            neopixel_set_brightness(anim->neopixel, anim->fade_step);
            neopixel_update(anim->neopixel);
            anim->last_update = current_time;
        }
        break;
    case ANIM_FADE_OUT:
        if (elapsed_time > anim->fade_delay)
        {
            anim->fade_step -= 1;
            if (anim->fade_step <= anim->fade_min)
            {
                anim->fade_step = anim->fade_min;
                anim->state = ANIM_IDLE;
            }
            neopixel_set_brightness(anim->neopixel, anim->fade_step);
            neopixel_update(anim->neopixel);
            anim->last_update = current_time;
        }
        break;
    case ANIM_BLINK:
        if (elapsed_time > anim->delay)
        {
            //NEO_ANIM_DBG_MSG("Blink\r\n");
            anim->color = (anim->color == 0) ? 0xFFFFFF : 0;
            neopixel_set_pixel_color_rgb(anim->neopixel, anim->led_index, anim->color);
            neopixel_update(anim->neopixel);
            anim->last_update = current_time;
        }
        break;

    case ANIM_RAINBOW:
        if (elapsed_time > anim->delay) {
            // Calculate Wheel index based on the current LED index and the total number of LEDs
            uint8_t wheel_pos = (anim->led_index * 256 / anim->neopixel->led_count) % 256;
            
            // Get color using the corrected wheel position
            uint32_t color = Wheel(wheel_pos);
            NEO_ANIM_DBG_MSG("Rainbow: %d, 0x%X\r\n", anim->led_index, color);
            
            // Set the pixel color and update the neopixel
            neopixel_set_pixel_color_rgb(anim->neopixel, anim->led_index, color);
            neopixel_update(anim->neopixel);
            
            // Move to the next LED, wrapping around if needed
            anim->led_index = (anim->led_index + 1) % anim->neopixel->led_count;

            // Update the last update time
            anim->last_update = current_time;
        }
        break;

    case ANIM_RAINBOW_CYCLE:
        if (elapsed_time > anim->delay) {
            // Apply an offset based on time and calculate wheel position
            anim->led_index = (anim->led_index + 1) % anim->neopixel->led_count;

            for (int i = 0; i < anim->neopixel->led_count; i++) {
                // Calculate position on color wheel for each LED
                uint8_t wheel_pos = ((i + anim->led_index) * 256 / anim->neopixel->led_count) % 256;
                
                // Get the color and apply it to each LED
                uint32_t color = Wheel(wheel_pos);
                
                // Set each LED's color
                neopixel_set_pixel_color_rgb(anim->neopixel, i, color);
            }
            neopixel_update(anim->neopixel);
            
            // Update the last update time
            anim->last_update = current_time;
        }
        break;
    
    case ANIM_COLOR_WIPE:
        if (elapsed_time > anim->delay) {
        // Determine the current color to set for the wipe animation
        uint32_t wipe_color = anim->color;

        // Set the color for the current LED index
        neopixel_set_pixel_color_rgb(anim->neopixel, anim->led_index, wipe_color);

        // Update the LED strip
        neopixel_update(anim->neopixel);

        // Move to the next LED
        anim->led_index++;

        // Reset the index after wiping through all the LEDs
        if (anim->led_index >= anim->neopixel->led_count) {
            anim->led_index = 0;

            // Optionally, change the wipe color after a complete pass
            anim->color = NextColor(anim->color); 
        }

        // Update the last update time
        anim->last_update = current_time;
        }
        break;

    case ANIM_THEATER_CHASE:
        if (elapsed_time > anim->delay) {
            // Set up two colors for the chase effect
            uint32_t on_color = anim->color;  // Color of the "on" lights
            uint32_t off_color = 0x000000;    // Color of the "off" lights

            // Set each LED in a chase pattern
            for (int i = 0; i < anim->neopixel->led_count; i++) {
                // Use modulo arithmetic to alternate the on/off pattern
                if ((i + anim->led_index) % 3 == 0) {
                    neopixel_set_pixel_color_rgb(anim->neopixel, i, on_color);
                } else {
                    neopixel_set_pixel_color_rgb(anim->neopixel, i, off_color);
                }
            }
            neopixel_update(anim->neopixel);

            // Move to the next chase position, wrapping around if needed
            anim->led_index = (anim->led_index + 1) % 3;

            // Optionally, change the chase color after a complete cycle
            if (anim->led_index == 0) {
                anim->color = NextColor(anim->color);
            }

            // Update the last update time
            anim->last_update = current_time;
        }
        break;

    case ANIM_THEATER_CHASE_RAINBOW:
        if (elapsed_time > anim->delay)
        {
            // Calculate the chase position
            anim->led_index = (anim->led_index + 1) % 3;

            // Set each LED in a chase pattern with a rainbow effect
            for (int i = 0; i < anim->neopixel->led_count; i++)
            {
                // Calculate the position in the rainbow for each LED
                uint8_t wheel_pos = ((i * 256 / anim->neopixel->led_count) + anim->led_index * 10) % 256;

                // Generate the rainbow color
                uint32_t rainbow_color = Wheel(wheel_pos);

                // Use modulo arithmetic to alternate the on/off pattern
                if ((i + anim->led_index) % 3 == 0)
                {
                    // "On" LED in rainbow color
                    neopixel_set_pixel_color_rgb(anim->neopixel, i, rainbow_color);
                }
                else
                {
                    // "Off" LED
                    neopixel_set_pixel_color_rgb(anim->neopixel, i, 0x000000);
                }
            }

            // Update the LED strip
            neopixel_update(anim->neopixel);

            // Update the last update time
            anim->last_update = current_time;
        }
        break;
    }


    
}

void neopixel_anim_fade_in(neopixel_animation_t *anim, uint32_t delay, uint32_t max_brightness)
{
    anim->state = ANIM_FADE_IN;
    anim->fade_delay = delay;
    anim->fade_max = max_brightness;
    anim->fade_step = 0;
    anim->last_update = Get_Systick_Cnt();
}

void neopixel_anim_fade_out(neopixel_animation_t *anim, uint32_t delay, uint32_t min_brightness)
{
    anim->state = ANIM_FADE_OUT;
    anim->fade_delay = delay;
    anim->fade_min = min_brightness;
    anim->fade_step = anim->neopixel->led_data_len;
    anim->last_update = Get_Systick_Cnt();
}

void neopixel_anim_blink(neopixel_animation_t *anim, uint32_t delay, uint32_t led_index)
{
    anim->state = ANIM_BLINK;
    anim->delay = delay;
    anim->led_index = led_index;
    anim->last_update = Get_Systick_Cnt();
}

void neopixel_anim_rainbow(neopixel_animation_t *anim, uint32_t delay)
{
    anim->state = ANIM_RAINBOW;
    anim->delay = delay;
    anim->led_index = 0;
    anim->last_update = Get_Systick_Cnt();
}

void neopixel_anim_rainbow_cycle(neopixel_animation_t *anim, uint32_t delay)
{
    anim->state = ANIM_RAINBOW_CYCLE;
    anim->delay = delay;
    anim->led_index = 0;
    anim->last_update = Get_Systick_Cnt();
}

void neopixel_anim_color_wipe(neopixel_animation_t *anim, uint32_t delay, uint32_t color)
{
    anim->state = ANIM_COLOR_WIPE;
    anim->delay = delay;
    anim->color = color;
    anim->led_index = 0;
    anim->last_update = Get_Systick_Cnt();
}

void neopixel_anim_theater_chase(neopixel_animation_t *anim, uint32_t delay, uint32_t color)
{
    anim->state = ANIM_THEATER_CHASE;
    anim->delay = delay;
    anim->color = color;
    anim->led_index = 0;
    anim->last_update = Get_Systick_Cnt();
}

void neopixel_anim_theater_chase_rainbow(neopixel_animation_t *anim, uint32_t delay)
{
    anim->state = ANIM_THEATER_CHASE_RAINBOW;
    anim->delay = delay;
    anim->led_index = 0;
    anim->last_update = Get_Systick_Cnt();
}

uint32_t Wheel(uint8_t WheelPos) {
    if (WheelPos < 85) {
        // From 0 to 84: Transition from Red to Green
        return RGB(255 - WheelPos * 3, WheelPos * 3, 0);
    } else if (WheelPos < 170) {
        // From 85 to 169: Transition from Green to Blue
        WheelPos -= 85;
        return RGB(0, 255 - WheelPos * 3, WheelPos * 3);
    } else {
        // From 170 to 255: Transition from Blue to Red
        WheelPos -= 170;
        return RGB(WheelPos * 3, 0, 255 - WheelPos * 3);
    }
}

uint32_t RGB(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

uint32_t NextColor(uint32_t current_color) {
    switch (current_color) {
        // Cycles through the following colors:
        case 0xFF0000: return 0x00FF00; // Red to Green
        case 0x00FF00: return 0x0000FF; // Green to Blue
        case 0x0000FF: return 0xFFFF00; // Blue to Yellow
        case 0xFFFF00: return 0x00FFFF; // Yellow to Cyan
        case 0x00FFFF: return 0xFF00FF; // Cyan to Magenta
        case 0xFF00FF: return 0xFFFFFF; // Magenta to White
        case 0xFFFFFF: return 0xFF0000; // White to Red

        // Default color (Red)
        default: return 0xFF0000; // Default back to Red
    }
}

