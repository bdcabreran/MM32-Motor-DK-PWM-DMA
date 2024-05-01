/**
 * @file neopixel.c
 * @author Bayron Cabrera (bayron.nanez@gmail.com)
 * @brief  Library for neopixel RGB LEDs
 * @version 0.1
 * @date 2024-05-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "neopixel.h"


void neopixel_init(neopixel_t *neopixel, uint8_t *data, uint16_t len, start_pwm_t start_pwm, stop_pwm_t stop_pwm)
{
    neopixel->led_data = data;
    neopixel->led_data_len = len;
    neopixel->start_dma = start_pwm;
    neopixel->stop_dma = stop_pwm;

    neopixel_clear(neopixel);
}


// Function to set a single LED at a particular index
void neopixel_set_pixel_color(neopixel_t *neopixel, uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue) {

    uint32_t pos = led_index * NEOPIXEL_BIT_PER_LED;
    uint8_t mask;
    int i = 0;

    if (pos >= neopixel->led_data_len) {
        return;
    }   

    uint8_t *led_data = neopixel->led_data;

    // GREEN color
    for (i = 0; i < 8; i++) {
        mask = (green & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
        led_data[pos++] = mask;
    }
    // RED color
    for (i = 0; i < 8; i++) {
        mask = (red & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
        led_data[pos++] = mask;
    }
    // BLUE color
    for (i = 0; i < 8; i++) {
        mask = (blue & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
        led_data[pos++] = mask;
    }
}


void neopixel_update(neopixel_t *neopixel) {
    neopixel->start_dma(neopixel->led_data, neopixel->led_data_len);
}

void neopixel_clear(neopixel_t *neopixel) {
    memset(neopixel->led_data, PWM_HIGH_0, neopixel->led_data_len);
}

void neopixel_set_brightness(neopixel_t *neopixel, uint8_t brightness) {
    uint32_t i;
    uint8_t *led_data = neopixel->led_data;
    for (i = 0; i < neopixel->led_data_len; i++) {
        led_data[i] = (led_data[i] == PWM_HIGH_1) ? PWM_HIGH_1 : (led_data[i] * brightness) / 255;
    }
}

void neopixel_set_pixel_color_rgb(neopixel_t *neopixel, uint8_t led_index, uint32_t color) {
    neopixel_set_pixel_color(neopixel, led_index, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}



// End of file
