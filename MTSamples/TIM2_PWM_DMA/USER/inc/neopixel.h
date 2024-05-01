/**
 * @file neopixel.h
 * @author Bayron Cabrera (bayron.nanez@gmail.com)
 * @brief  Library for neopixel RGB LEDs
 * @version 0.1
 * @date 2024-04-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include <stdint.h>


typedef void (*start_pwm_t)(uint8_t *, uint16_t);
typedef void (*stop_pwm_t)(void);

#define NEOPIXEL_BIT_PER_LED   24

// frequency = 650000Hz
// SystemCoreClock = 96000000Hz
// PWM Period:ARR = (SystemCoreClock / frequency) - 1 = 147

// Adjust the values of PWM_HIGH_0 and PWM_HIGH_1 to match the timing of the neopixel LEDs
#define PWM_HIGH_0 34  // 354.28 ns
#define PWM_HIGH_1 67  // 698.14 ns

typedef struct {
    uint8_t *led_data;
    uint16_t led_data_len;
    start_pwm_t start_dma;
    stop_pwm_t stop_dma;
} neopixel_t;

void neopixel_init(neopixel_t *neopixel, uint8_t *data, uint16_t len, start_pwm_t start_pwm, stop_pwm_t stop_pwm);
void neopixel_set_pixel_color(neopixel_t *neopixel, uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue);
void neopixel_update(neopixel_t *neopixel);
void neopixel_clear(neopixel_t *neopixel);
void neopixel_set_brightness(neopixel_t *neopixel, uint8_t brightness);
void neopixel_set_pixel_color_rgb(neopixel_t *neopixel, uint8_t led_index, uint32_t color);

#endif // NEOPIXEL_H