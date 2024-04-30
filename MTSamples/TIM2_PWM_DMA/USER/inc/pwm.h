/**
  ******************************************************************************
  * @file   pwm.h
  * @author samuel.cifuentes@titoma.com
  * @brief  Header the TIMERx and channels for PWM based modules.
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWM_H_
#define __PWM_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "mm32_device.h"
#include "hal_tim.h"
#include "hal_dma.h"

#define TIMER_FREQUENCY     96000000U // 96 MHz
#define PWM_TIMER_FREQUENCY 800000U // 800 kHz
#define PWM_TIMER_PRESCALER 1
//#define PWM_TIMER_PERIOD    ((HAL_RCC_GetHCLKFreq() / PWM_TIMER_FREQUENCY) / PWM_TIMER_PRESCALER)
#define PWM_TIMER_PERIOD    ((TIMER_FREQUENCY / PWM_TIMER_FREQUENCY) / PWM_TIMER_PRESCALER)

#define TIME_FOR_NEXT_STREAM 2

typedef struct pwm_handler_ {
	TIM_TypeDef   *timer;
	__IO uint32_t *capture_compare;
	uint8_t       channel;

	uint32_t            dma_trigger;
	DMA_Channel_TypeDef *dma;
	uint32_t            last_dma_tx_tick;

}pwm_handler_t;


void pwm_start(pwm_handler_t *, uint8_t);
void pwm_stop(pwm_handler_t *);

void pwm_dma_start(pwm_handler_t *, uint8_t *, uint16_t);
void pwm_dma_stop(pwm_handler_t *);

uint8_t pwm_dma_is_busy(pwm_handler_t *hpwm);
uint8_t pwm_get_compare(pwm_handler_t *hpwm);
void pwm_dma_start32(pwm_handler_t *hpwm, uint32_t *buffer, uint16_t len);

#endif /* __PWM_H_ */
