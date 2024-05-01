/**
 * @file pwm_dma.h
 * @author Bayron Cabrera (bayron.nanez@gmail.com)
 * @brief  This Function Contains a Basic Library to Configure the PWM in the TIM2 using DMA
 * @version 0.1
 * @date 2024-05-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWM_DMA_H_
#define __PWM_DMA_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "mm32_device.h"
#include "hal_tim.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "hal_misc.h"

extern uint32_t SystemCoreClock;
#define TIMER_FREQUENCY     SystemCoreClock // 96 MHz
#define PWM_TIMER_FREQUENCY 650000U // 650 kHz
#define PWM_TIMER_PERIOD    (TIMER_FREQUENCY / PWM_TIMER_FREQUENCY) - 1
#define PWM_TIMER_PRESCALER 0


void PWM_DMA_Init();
void PWM_DMA_Start(uint8_t *, uint16_t);
void PWM_DMA_Stop();


#endif /* __PWM_H_ */
