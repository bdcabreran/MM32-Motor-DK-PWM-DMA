/**
 * @file bsp_led_pwm.h
 * @author Bayron Cabrera (bayron.nanez@gmail.com)
 * @brief  BSP for LED PWM
 * @version 0.1
 * @date 2024-07-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __BSP_LED_PWM_H
#define __BSP_LED_PWM_H

#include "main.h"
#include "stdint.h"
#include <stdint.h>
#include "mm32_device.h"
#include "hal_tim.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "hal_misc.h"

// RGYW LEDs Configurations

#define LED_RGYW_TIMER_RED (TIM2)
#define LED_RGYW_TIMER_CHANNEL_RED (TIM_Channel_1)
#define LED_RGYW_TIMER_CC_RED (LED_RGYW_TIMER_RED->CCR1)
#define LED_RGYW_RED_PORT GPIOC
#define LED_RGYW_RED_PIN GPIO_Pin_13
#define LED_RGYW_RED_PIN_SRC GPIO_PinSource13
#define LED_RGYW_RED_PIN_AF GPIO_AF_6

#define LED_RGYW_TIMER_GREEN (TIM2)
#define LED_RGYW_TIMER_CHANNEL_GREEN (TIM_Channel_2)
#define LED_RGYW_TIMER_CC_GREEN (LED_RGYW_TIMER_GREEN->CCR2)
#define LED_RGYW_GREEN_PORT GPIOC
#define LED_RGYW_GREEN_PIN GPIO_Pin_14
#define LED_RGYW_GREEN_PIN_SRC GPIO_PinSource14
#define LED_RGYW_GREEN_PIN_AF GPIO_AF_6

#define LED_RGYW_TIMER_YELLOW (TIM2)
#define LED_RGYW_TIMER_CHANNEL_YELLOW (TIM_Channel_3)
#define LED_RGYW_TIMER_CC_YELLOW (LED_RGYW_TIMER_YELLOW->CCR3)
#define LED_RGYW_YELLOW_PORT GPIOC
#define LED_RGYW_YELLOW_PIN GPIO_Pin_15
#define LED_RGYW_YELLOW_PIN_SRC GPIO_PinSource15
#define LED_RGYW_YELLOW_PIN_AF GPIO_AF_6

// The White LED is not PWM controlled
#define LED_RGYW_WHITE_PORT GPIOC
#define LED_RGYW_WHITE_PIN GPIO_Pin_10

#define MCU_CLOCK (48000000UL)
#define MCU_CLOCK_MHZ (48U)
#define LED_RGYW_PWM_FREQUENCY (1000U)       /* (Hz) PWM frequency for LED control. */
#define LED_RGYW_PWM_SCALER (MCU_CLOCK_MHZ)
#define LED_RGYW_PWM_PRESCALER (LED_RGYW_PWM_SCALER - 1)
#define LED_RGYW_PWM_MAX_PULSE ((uint32_t)MCU_CLOCK / LED_RGYW_PWM_FREQUENCY / LED_RGYW_PWM_SCALER)
#define LED_RGYW_PWM_PERIOD (LED_RGYW_PWM_MAX_PULSE - 1)

#define LED_RGYW_COMMON_ANODE (1)
#define LED_RGYW_COMMON_CATHODE (2)
#define LED_RGYW_COMMON_CONFIG (LED_RGYW_COMMON_ANODE)

#if (LED_RGYW_COMMON_CONFIG == LED_RGYW_COMMON_CATHODE)

#define LED_RGYW_RED_ON GPIO_SetBits(LED_RGYW_RED_PORT, LED_RGYW_RED_PIN)
#define LED_RGYW_GREEN_ON GPIO_SetBits(LED_RGYW_GREEN_PORT, LED_RGYW_GREEN_PIN)
#define LED_RGYW_YELLOW_ON GPIO_SetBits(LED_RGYW_YELLOW_PORT, LED_RGYW_YELLOW_PIN)
#define LED_RGYW_WHITE_ON GPIO_SetBits(LED_RGYW_WHITE_PORT, LED_RGYW_WHITE_PIN)

#define LED_RGYW_RED_OFF GPIO_ResetBits(LED_RGYW_RED_PORT, LED_RGYW_RED_PIN)
#define LED_RGYW_GREEN_OFF GPIO_ResetBits(LED_RGYW_GREEN_PORT, LED_RGYW_GREEN_PIN)
#define LED_RGYW_YELLOW_OFF GPIO_ResetBits(LED_RGYW_YELLOW_PORT, LED_RGYW_YELLOW_PIN)
#define LED_RGYW_WHITE_OFF GPIO_ResetBits(LED_RGYW_WHITE_PORT, LED_RGYW_WHITE_PIN)

#define RGB_LED_GPIO_MODE GPIO_Mode_Out_PP
#else

#define LED_RGYW_RED_ON GPIO_ResetBits(LED_RGYW_RED_PORT, LED_RGYW_RED_PIN)
#define LED_RGYW_GREEN_ON GPIO_ResetBits(LED_RGYW_GREEN_PORT, LED_RGYW_GREEN_PIN)
#define LED_RGYW_YELLOW_ON GPIO_ResetBits(LED_RGYW_YELLOW_PORT, LED_RGYW_YELLOW_PIN)
#define LED_RGYW_WHITE_ON GPIO_ResetBits(LED_RGYW_WHITE_PORT, LED_RGYW_WHITE_PIN)

#define LED_RGYW_RED_OFF GPIO_SetBits(LED_RGYW_RED_PORT, LED_RGYW_RED_PIN)
#define LED_RGYW_GREEN_OFF GPIO_SetBits(LED_RGYW_GREEN_PORT, LED_RGYW_GREEN_PIN)
#define LED_RGYW_YELLOW_OFF GPIO_SetBits(LED_RGYW_YELLOW_PORT, LED_RGYW_YELLOW_PIN)
#define LED_RGYW_WHITE_OFF GPIO_SetBits(LED_RGYW_WHITE_PORT, LED_RGYW_WHITE_PIN)

#define RGB_LED_GPIO_MODE GPIO_Mode_Out_OD

#endif

/**
 * @brief Initialize the Timer for the RGYW LEDs
 * 
 */
void LED_RGYW_TIM_Init(void);

void LED_SetPWM_Red(uint16_t DutyCycle);
void LED_SetPWM_Green(uint16_t DutyCycle);
void LED_SetPWM_Yellow(uint16_t DutyCycle);
void LED_PWM_Start(void);
void LED_PWM_Stop(void);


#endif 