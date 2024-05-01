/**
 * @file neopixel_anim.h
 * @author Bayron Cabrera (bayron.nanez@gmail.com)
 * @brief  Finite State Machine for LED animations
 * @version 0.1
 * @date 2024-05-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef NEOPIXEL_ANIM_H
#define NEOPIXEL_ANIM_H

#include <stdint.h>
#include "neopixel.h"
#include "drv_uart.h"

typedef enum
{
    ANIM_IDLE,
    ANIM_FADE_IN,
    ANIM_FADE_OUT,
    ANIM_BLINK,
    ANIM_RAINBOW,
    ANIM_RAINBOW_CYCLE,
    ANIM_COLOR_WIPE,
    ANIM_THEATER_CHASE,
    ANIM_THEATER_CHASE_RAINBOW,
    ANIM_MAX
} anim_state_t;

typedef struct neopixel_animation
{
    uint8_t initial_colors;
    neopixel_t *neopixel;
    anim_state_t state;
    uint8_t led_index;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t brightness;
    uint32_t color;
    uint32_t delay;
    uint32_t last_update;
    uint32_t last_cycle;
    uint32_t last_blink;
    uint32_t last_chase;
    uint32_t last_wipe;
    uint32_t last_rainbow;
    uint32_t last_rainbow_cycle;
    uint32_t last_fade;
    uint32_t fade_delay;
    uint32_t fade_step;
    uint32_t fade_max;
    uint32_t fade_min;
    uint32_t fade_dir;
} neopixel_animation_t;

void neopixel_anim_init(neopixel_animation_t *anim, neopixel_t *neopixel);
void neopixel_anim_update(neopixel_animation_t *anim);
void neopixel_anim_fade_in(neopixel_animation_t *anim, uint32_t delay, uint32_t max_brightness);
void neopixel_anim_blink(neopixel_animation_t *anim, uint32_t delay, uint32_t led_index);
void neopixel_anim_rainbow(neopixel_animation_t *anim, uint32_t delay);
void neopixel_anim_rainbow_cycle(neopixel_animation_t *anim, uint32_t delay);
void neopixel_anim_color_wipe(neopixel_animation_t *anim, uint32_t delay, uint32_t color);
void neopixel_anim_theater_chase(neopixel_animation_t *anim, uint32_t delay, uint32_t color);
void neopixel_anim_theater_chase_rainbow(neopixel_animation_t *anim, uint32_t delay);




#endif // NEOPIXEL_ANIM_H