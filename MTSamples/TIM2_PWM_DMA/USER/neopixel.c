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


#define DEBUG_BUFFER_SIZE 100
#define NEOPIXEL_DBG 1

#if NEOPIXEL_DBG
#include <stdio.h>
#define NEOPIXEL_DBG_MSG(fmt, ...) do { \
  static char dbgBuff[DEBUG_BUFFER_SIZE]; \
  snprintf(dbgBuff, DEBUG_BUFFER_SIZE, (fmt), ##__VA_ARGS__); \
  Uart_Put_Buff(dbgBuff, strlen(dbgBuff)); \
} while(0)
#else
#define NEOPIXEL_DBG_MSG(fmt, ...) do { } while(0)
#endif


static void neopixel_store_initial(neopixel_t *neopixel, uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue);


/**
 * @brief Initialize a neopixel structure
 * 
 * @param neopixel Pointer to the neopixel structure to initialize
 * @param start_pwm Function to start PWM DMA transfer
 * @param stop_pwm Function to stop PWM DMA transfer
 */
void neopixel_init(neopixel_t *neopixel, start_pwm_t start_pwm, stop_pwm_t stop_pwm)
{
    neopixel->led_data_len = NEOPIXEL_BUFFER_SIZE;
    neopixel->start_dma = start_pwm;
    neopixel->stop_dma = stop_pwm;
    neopixel->led_count = NEOPIXEL_LED_COUNT;
    memset(neopixel->led_data, PWM_HIGH_0, neopixel->led_data_len);
    memset(neopixel->led_initial, 0, NEOPIXEL_BACKUP_SIZE);
    neopixel_clear(neopixel);
}


/**
 * @brief Set the color of a specific LED
 * 
 * @param neopixel Pointer to the neopixel structure
 * @param led_index Index of the LED to set
 * @param red Red color component (0-255)
 * @param green Green color component (0-255)
 * @param blue Blue color component (0-255)
 */
void neopixel_set_pixel_color(neopixel_t *neopixel, uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue) {

    if (led_index >= neopixel->led_count) {
        return;
    }   

    neopixel_store_initial(neopixel, led_index, red, green, blue);

    // Update the PWM data
    uint32_t pos = led_index * NEOPIXEL_BIT_PER_LED;
    uint8_t mask;
    int i = 0;

    NEOPIXEL_DBG_MSG("led: %d, r: %d, g: %d, b: %d\r\n", led_index, red, green, blue);


    for (i = 0; i < 8; i++) {
        mask = (green & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
        neopixel->led_data[pos++] = mask;
    }
    for (i = 0; i < 8; i++) {
        mask = (red & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
        neopixel->led_data[pos++] = mask;
    }
    for (i = 0; i < 8; i++) {
        mask = (blue & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
        neopixel->led_data[pos++] = mask;
    }
}


static void neopixel_store_initial(neopixel_t *neopixel, uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue) {
    // Store the initial colors in the backup buffer
    neopixel->led_initial[led_index * 3] = red;
    neopixel->led_initial[led_index * 3 + 1] = green;
    neopixel->led_initial[led_index * 3 + 2] = blue;
}

/**
 * @brief Update the neopixel LEDs by starting the DMA transfer
 * 
 * @param neopixel Pointer to the neopixel structure
 */
void neopixel_update(neopixel_t *neopixel) {
    neopixel->start_dma(neopixel->led_data, neopixel->led_data_len);
}

/**
 * @brief Clear all LEDs to turn them off
 * 
 * @param neopixel Pointer to the neopixel structure
 */
void neopixel_clear(neopixel_t *neopixel) {
    memset(neopixel->led_data, PWM_HIGH_0, neopixel->led_data_len);
}

/**
 * @brief Set the brightness of the LEDs
 * 
 * @param neopixel Pointer to the neopixel structure
 * @param brightness Brightness level (0-255)
 */
void neopixel_set_brightness(neopixel_t *neopixel, uint8_t brightness) {
    // Convert brightness percentage to scale between 0 to 255
    uint16_t scale = (brightness * 255) / 100;

    for (uint32_t led_index = 0; led_index < neopixel->led_data_len / NEOPIXEL_BIT_PER_LED; led_index++) {
        uint32_t pos = led_index * NEOPIXEL_BIT_PER_LED;
        uint8_t original_red = neopixel->led_initial[led_index * 3];
        uint8_t original_green = neopixel->led_initial[led_index * 3 + 1];
        uint8_t original_blue = neopixel->led_initial[led_index * 3 + 2];

        // Use uint16_t for scaling to prevent overflow
        uint16_t red = (original_red * scale) / 255;
        uint16_t green = (original_green * scale) / 255;
        uint16_t blue = (original_blue * scale) / 255;

        //NEOPIXEL_DBG_MSG("led: %d, r: %d, g: %d, b: %d\r\n", led_index, red, green, blue);

        // Update the PWM data buffer
        uint8_t mask;
        int i = 0;

        for (i = 0; i < 8; i++)
        {
            mask = (green & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
            neopixel->led_data[pos++] = mask;
        }
        for (i = 0; i < 8; i++)
        {
            mask = (red & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
            neopixel->led_data[pos++] = mask;
        }
        for (i = 0; i < 8; i++)
        {
            mask = (blue & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
            neopixel->led_data[pos++] = mask;
        }
    }
}


/**
 * @brief Set the color of a specific LED using a single 32-bit color value
 * 
 * @param neopixel Pointer to the neopixel structure
 * @param led_index Index of the LED to set
 * @param color 32-bit color value (0x00RRGGBB)
 */
void neopixel_set_pixel_color_rgb(neopixel_t *neopixel, uint8_t led_index, uint32_t color) {
    neopixel_set_pixel_color(neopixel, led_index, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}



// End of file
