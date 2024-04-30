/**
 * @file ws2812.h
 * @author Bayron Cabrera (bayron.nanez@gmail.com)
 * @brief  WS2812 driver for Cortex-M microcontrollers
 * @version 0.1
 * @date 2024-04-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WS2812_H
#define WS2812_H

#include <stdint.h>

// https://www.mindmotion.com.cn/download/products/UM_MM32SPIN0280_EN.pdf

/**
 * @brief WS2812 Hardware Information 
 * PA2 
 * AF1 : UART2_TX
 * AF2 : TIM2_CH3/PWMTRG2
 * AF3 : TIM1_CH2
 */

// // Function pointer to the DMA interrupt handler
// typedef void (*ws2812_dma_callback_t)(void);

// // Function pointer to the TIM interrupt handler
// typedef void (*ws2812_tim_callback_t)(void);

// // Configuration Structure
// typedef struct
// {
//     TIM_HandleTypeDef *tim_handle;
//     DMA_HandleTypeDef *dma_handle;
//     uint32_t tim_channel;
//     uint32_t dma_channel;
//     uint32_t dma_irq;
//     uint32_t tim_irq;
//     ws2812_dma_callback_t dma_callback;
//     ws2812_tim_callback_t tim_callback;
// } ws2812_config_t;

// Define the number of LEDs in the strip
#define WS2812_NUM_LEDS      1
#define WS2812_BIT_PER_LED   6      // 8 bits per color, RGB

void ws2812Init(void);
void WS2812SetPixelColor(uint8_t led, uint8_t r, uint8_t g, uint8_t b);
void WS2812SetAllPixelsGreen(void);
void Start_WS2812_PWM_DMA(void);
void Stop_WS2812_PWM_DMA(void);
void Start_WS2812_PWM(void);
void Stop_WS2812_PWM(void);
void WS2812_SetPixelColor(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue, uint8_t* led_data);
void WS2812_SetPixelColor32(uint8_t led_index, uint32_t red, uint32_t green, uint32_t blue, uint32_t* led_data);

#define SET_CCR3_VAL(Value)         WRITE_REG(TIM2->CCR3, Value)


#endif // WS2812_H