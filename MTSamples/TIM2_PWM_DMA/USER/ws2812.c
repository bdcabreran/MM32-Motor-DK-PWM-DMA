/**
 * @file ws2812.c
 * @author Bayron Cabrera (bayron.nanez@gmail.com)
 * @brief WS2812 driver for Cortex-M microcontrollers
 * @version 0.1
 * @date 2024-04-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ws2812.h"
#include "hal_tim.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "hal_rcc.h"
#include "hal_misc.h"
#include "types.h"

extern uint32_t SystemCoreClock;

#define WS2812_USE_DMA 1

#if WS2812_USE_DMA
#define WS2812_USE_DMA_IRQ_ENABLE 1
#else 
#define WS2812_USE_DMA_IRQ_ENABLE 0
#endif

// https://www.mindmotion.com.cn/download/products/UM_MM32SPIN0280_EN.pdf

// Define the Timer and PWM channel to be used
#define WS2812_TIM      TIM2
#define WS2812_TIM_CH   TIM_CHANNEL_3
#define WS2812_DMA_CH   DMA1_Channel1           // Check User Manual for DMA Channel
#define WS2812_DMA_IRQ  DMA1_Channel1_IRQn      // Corresponding IRQ for the DMA Channel

// Define GPIO Pin configuration for PA9
#define WS2812_GPIO_PORT    GPIOA
#define WS2812_GPIO_PIN     GPIO_Pin_2
#define WS2812_GPIO_AF      GPIO_AF_2           // PA2 AF2 : TIM2_CH3/PWMTRG2
#define WS2812_GPIO_CLK     RCC_AHBENR_GPIOA
#define WS2812_GPIO_SOURCE  GPIO_PinSource2

#define WS2812_RESET_PERIOD   50                              // Reset pulse period

#define Prescaler          1
#define PWM_Frequency      800000  // 800kHz
#define WS2812_PERIOD      ((SystemCoreClock / PWM_Frequency) - 1)


// Adjust these values according to your timer and PWM settings
// PWM counts for a 0 code and 1 code, rounded to the nearest whole number
#define PWM_HIGH_0            34  // Timer count for logical "0" high period
#define PWM_HIGH_1            67  // Timer count for logical "1" high period

// We'll calculate PWM_LOW for "0" and "1" using the total period minus the high period
#define PWM_LOW_0             (WS2812_PERIOD - PWM_HIGH_0)  // Timer count for logical "0" low period
#define PWM_LOW_1             (WS2812_PERIOD - PWM_HIGH_1)  // Timer count for logical "1" low period

#define USE_32_DATA_SIZE 0

#if USE_32_DATA_SIZE
uint32_t led_data[WS2812_NUM_LEDS * WS2812_BIT_PER_LED]; // 24 bits per LED
#else 
uint8_t led_data[WS2812_NUM_LEDS * WS2812_BIT_PER_LED]; // 24 bits per LED
#endif 

uint32_t pwm_period = 0;

#define USE_CIRCULAR_MODE 0

void Start_WS2812_PWM_DMA(void) {
    // Set up the DMA first, but don't enable it yet
    exDMA_SetMemoryAddress(WS2812_DMA_CH, (uint32_t)&led_data[0]); // Set the memory address
    DMA_SetCurrDataCounter(WS2812_DMA_CH, sizeof(led_data)); // Set the data counter

    // Enable the timer DMA interface to allow DMA requests
    TIM_DMACmd(WS2812_TIM, TIM_DMA_CC3, ENABLE);

    // Then start the timer
    TIM_Cmd(WS2812_TIM, ENABLE); // Enable the TIM peripheral

    // Now enable the PWM outputs
    TIM_CtrlPWMOutputs(WS2812_TIM, ENABLE); // Enable PWM outputs

    // Finally, enable the DMA to start processing the DMA requests
    DMA_Cmd(WS2812_DMA_CH, ENABLE); // Enable the DMA channel
}


void Stop_WS2812_PWM_DMA(void) {
    // Disable the DMA channel first to stop any ongoing or further data transfers.
    DMA_Cmd(WS2812_DMA_CH, DISABLE);
    
    // Then disable the DMA requests from the timer.
    TIM_DMACmd(WS2812_TIM, TIM_DMA_CC3, DISABLE);

    // Manually set the GPIO pin low
    // Now disable the PWM outputs.
    TIM_CtrlPWMOutputs(WS2812_TIM, DISABLE);
    
    // Finally, disable the timer.
    TIM_Cmd(WS2812_TIM, DISABLE);

    GPIO_WriteBit(WS2812_GPIO_PORT, WS2812_GPIO_PIN, Bit_RESET); // Assuming there's a function like this available
}


void Start_WS2812_PWM(void) {
    // // Start the timer for PWM
    TIM_Cmd(WS2812_TIM, ENABLE);  // Enable the TIM peripheral
    TIM_CtrlPWMOutputs(WS2812_TIM, ENABLE);  // Enable PWM outputs
}

void Stop_WS2812_PWM(void) {
    // Disable the PWM outputs and the timer
    TIM_CtrlPWMOutputs(WS2812_TIM, DISABLE);  // Disable PWM outputs
    TIM_Cmd(WS2812_TIM, DISABLE);  // Disable the TIM peripheral
}


void ws2812_Gpio_Init(void)
{
    // Enable Clock
    RCC_AHBPeriphClockCmd(WS2812_GPIO_CLK, ENABLE);

    // Initialize the GPIO
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = WS2812_GPIO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(WS2812_GPIO_PORT, &GPIO_InitStruct);
    GPIO_PinAFConfig(WS2812_GPIO_PORT, WS2812_GPIO_SOURCE, WS2812_GPIO_AF);
}

void dma_timer_pwm_config(void) {

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    // Configure the DMA channel for PWM data transfer.
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(TIM2->CCR3); // Assuming CCR3 is used
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)led_data;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = sizeof(led_data);
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // Assuming the buffer contains 8-bit values
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; // Use DMA_Mode_Circular if continuous PWM updates are needed
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    // Enable the DMA channel.
    DMA_Cmd(DMA1_Channel1, DISABLE);

    // Enable the timer's DMA request for update events.
    // TIM_DMAConfig(TIM2, TIM_DMABase_CCR3, TIM_DMABurstLength_1Byte);
    //TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
}


void ws2812_DMA_Channel1_Init(void)
{
    // Enable Clock
    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_InitTypeDef DMA_InitStruct;
    DMA_DeInit(WS2812_DMA_CH);
    DMA_StructInit(&DMA_InitStruct);

    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&WS2812_TIM->CCR3;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&led_data[0];
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = 2;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    #if USE_32_DATA_SIZE
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    #else 
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    #endif
    #if USE_CIRCULAR_MODE
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Auto_reload = DMA_Auto_Reload_Enable;
    #else 
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Auto_reload = DMA_Auto_Reload_Disable;
    #endif
    DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(WS2812_DMA_CH, &DMA_InitStruct);
    // DMA_SetCurrDataCounter(WS2812_DMA_CH, sizeof(led_data));
}

void ws2812_DMA_Channel1_IT_Init()
{
    DMA_ITConfig(WS2812_DMA_CH, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = WS2812_DMA_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}



void pwm_timer_init(void) {
    // Enable timer clock
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM2EN, ENABLE); // Assuming TIM2 is used, modify as necessary for your timer



    // Set up the timer
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = WS2812_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = Prescaler - 1; 
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // Enable interrupt
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // Set up PWM mode for the timer's channel
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = PWM_HIGH_0; // CCRx register value for desired duty cycle
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // Output polarity

    // Init the PWM channel with the above configuration
    TIM_OC3Init(TIM2, &TIM_OCInitStructure); // Assuming using channel 3, modify as necessary

    // Enable the corresponding preload register
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable); // Assuming using channel 3, modify as necessary
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_SetCounter(TIM2, 0);

    // Necessary ?
    //TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0); // Select the input trigger
    //TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Disable); // Enable the Master/Slave mode

    // Enable TIM Update DMA request
    // TIM_DMAConfig(TIM2, TIM_DMABase_CCR3, TIM_DMABurstLength_1Byte); // Assuming channel 3 for DMA burst
    // TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
    
    TIM_DMACmd(TIM2, TIM_DMA_CC3, ENABLE);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);



    // Enable IT
    TIM_ClearFlag(TIM2, TIM_FLAG_CC3);

    //TIM_ITConfig(WS2812_TIM, TIM_IT_CC3, ENABLE);
    // TIM_ITConfig(WS2812_TIM, TIM_IT_Update, ENABLE);
    TIM_CCxCmd(WS2812_TIM, TIM_Channel_3, TIM_CCx_Enable);

    // Enable PWM 
    TIM_CtrlPWMOutputs(TIM2, ENABLE);

    // Enable the timer
    TIM_Cmd(TIM2, DISABLE);
}


void ws2812Init(void)
{
    // Initialize the GPIO
    ws2812_Gpio_Init();

#if WS2812_USE_DMA
    //ws2812_DMA_Channel1_Init();
    dma_timer_pwm_config();
#endif 

    #if WS2812_USE_DMA_IRQ_ENABLE
    ws2812_DMA_Channel1_IT_Init();
    #endif

    // ws2812_TIM_PWM_Init();

    pwm_timer_init();

    // pwm_timer_init2();

}

void WS2812SetPixelColor(uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
    int i = 0;
    uint8_t *p = &led_data[led * 24];
    for (i = 7; i >= 0; i--) {
        *p++ = (g & (1 << i)) ? 17 : 9;
    }
    for (i = 7; i >= 0; i--) {
        *p++ = (r & (1 << i)) ? 17 : 9;
    }
    for (i = 7; i >= 0; i--) {
        *p++ = (b & (1 << i)) ? 17 : 9;
    }
}

void WS2812SetAllPixelsGreen(void) {

    uint8_t intensity = 0xFF; // Maximum brightness for green.
    for(uint8_t led = 0; led < WS2812_NUM_LEDS; led++) {
        WS2812SetPixelColor(led, 0, intensity, 0); // Set each LED to green.
    }
}


// Function to set a single LED at a particular index
void WS2812_SetPixelColor(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue, uint8_t* led_data) {

    uint32_t pos = led_index * WS2812_BIT_PER_LED;
    uint8_t mask;
    int i = 0;

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

// Function to set a single LED at a particular index
void WS2812_SetPixelColor32(uint8_t led_index, uint32_t red, uint32_t green, uint32_t blue, uint32_t* led_data) {

    uint32_t pos = led_index * WS2812_BIT_PER_LED;
    uint8_t mask;
    int i = 0;

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



// Function to update the LED strip, triggers the DMA to start sending data
void WS2812_Update(void) {
    DMA_SetCurrDataCounter(WS2812_DMA_CH, sizeof(led_data));  // Set the correct buffer size
    DMA_Cmd(WS2812_DMA_CH, ENABLE);  // Enable the DMA
    TIM_Cmd(WS2812_TIM, ENABLE);  // Start the timer
}