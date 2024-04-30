/**
  ******************************************************************************
  * @file   pwm.c
  * @author samuel.cifuentes@titoma.com
  * @brief Functions to handle the PWM related modules such as neopixel, buzzer, vibrator.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "pwm.h"
#include "mm32_reg_redefine_v1.h"	

#define TIM_CCx_DISABLE    0x00000000U                            /*!< Input or output channel is disabled */
#define IS_TIM_BREAK_INSTANCE(INSTANCE)\
  ((INSTANCE) == TIM1)


#define IS_TIM_SLAVE_INSTANCE(INSTANCE)\
  (((INSTANCE) == TIM1)    || \
   ((INSTANCE) == TIM2)    || \
   ((INSTANCE) == TIM3))

// #define TIM_SMCR_SMS_Pos                    (0U)                               
// #define TIM_SMCR_SMS_Msk                    (0x7UL << TIM_SMCR_SMS_Pos)         /*!< 0x00000007 */
// #define TIM_SMCR_SMS                        TIM_SMCR_SMS_Msk                   /*!<SMS[2:0] bits (Slave mode selection) */
// #define TIM_SMCR_SMS_0                      (0x1UL << TIM_SMCR_SMS_Pos)         /*!< 0x00000001 */
// #define TIM_SMCR_SMS_1                      (0x2UL << TIM_SMCR_SMS_Pos)         /*!< 0x00000002 */
// #define TIM_SMCR_SMS_2                      (0x4UL << TIM_SMCR_SMS_Pos)         /*!< 0x00000004 */
/** @defgroup Channel_CC_State TIM Capture/Compare Channel State
  * @{
  */
#define TIM_CCx_ENABLE                   0x00000001U                            /*!< Input or output channel is enabled */
#define TIM_CCx_DISABLE                  0x00000000U                            /*!< Input or output channel is disabled */
#define TIM_CCxN_ENABLE                  0x00000004U                            /*!< Complementary output channel is enabled */
#define TIM_CCxN_DISABLE                 0x00000000U  

#define TIM_SLAVEMODE_TRIGGER                (TIM_SMCR_SMS_RESET | TIM_SMCR_SMS_ENCODER2)                  /*!< Trigger Mode                  */

#define IS_TIM_SLAVEMODE_TRIGGER_ENABLED(__TRIGGER__) ((__TRIGGER__) == TIM_SLAVEMODE_TRIGGER)

void pwm_start(pwm_handler_t *hpwm, uint8_t compare)
{
	/* Scale the values to the range {0, PWM_TIMER_PERIOD} */
	uint8_t compare_scaled = (uint8_t)(((float)(compare / 255.0)) * PWM_TIMER_PERIOD);

	/* Set the PWM value */
	*((uint32_t *)hpwm->capture_compare) = compare_scaled;

	/* Enable the channel */
	hpwm->timer->CCER &= ~(TIM_CCER_CC1E << (hpwm->channel & 0x1FU));/* 0x1FU = 31 bits max shift */
	hpwm->timer->CCER |= (uint32_t)(TIM_CCER_CC1EN << hpwm->channel);

	if (IS_TIM_BREAK_INSTANCE(hpwm->timer) != RESET) {
		/* Enable the main output */
		hpwm->timer->BDTR |= TIM_BDTR_MOE;
	}

	/* Enable the timer */
	hpwm->timer->CR1 |= TIM_CR1_CEN;
}

void pwm_stop(pwm_handler_t *hpwm)
{
    // Disable the PWM channel by clearing the CCxE bit in the CCER register.
    // Assuming hpwm->channel contains the channel number multiplied by 4 (i.e., 0 for channel 1, 4 for channel 2, etc.)
    hpwm->timer->CCER &= ~(TIM_CCER_CC1E << (hpwm->channel & 0x1FU)); // Clear the CCxE bit
}

/**
 * @brief  This function configures and starts a DMA transmission given the configuration of the neoPixel manager.
 * @param  hpwm: the pointers to the HW for the PWM transmission.
 * @param  buffer: the pointer to the beginning of the memory to be transmitted.
 * @param  len: the length of the data to be transmitted
 * @retval None
 */
void pwm_dma_start(pwm_handler_t *hpwm, uint8_t *buffer, uint16_t len)
{
    // Ensure there's no ongoing DMA transfer
    //if (hpwm->dma->CNDTR != 0) {
    //    return;
    //}

    // Disable the DMA channel before configuration
    hpwm->dma->CCR &= ~DMA_CCR_EN;

    // Set the peripheral and memory addresses and the number of data to transfer
    hpwm->dma->CPAR  = (uint32_t)hpwm->capture_compare;
    hpwm->dma->CMAR  = (uint32_t)buffer;
    hpwm->dma->CNDTR = len;

    // Enable the DMA channel and configure it to generate an interrupt on transfer completion
    hpwm->dma->CCR |= DMA_CCR_TCIE;
    hpwm->dma->CCR |= DMA_CCR_EN;

    // Configure the timer to generate DMA requests on update events
    hpwm->timer->DIER |= hpwm->dma_trigger;
    hpwm->timer->DIER |= TIM_DIER_UDE;

    // Enable the PWM channel associated with the DMA
    hpwm->timer->CCER &= ~(TIM_CCER_CC1E << (hpwm->channel & 0x1FU));
    hpwm->timer->CCER |= (TIM_CCx_ENABLE << hpwm->channel); // Make sure TIM_CCx_ENABLE is defined to the correct value

    // For timers with a Break feature, enable the main output
    if (IS_TIM_BREAK_INSTANCE(hpwm->timer)) {
        hpwm->timer->BDTR |= TIM_BDTR_MOE;
    }

    // Enable the timer if it's not in a slave mode triggered by an external event
    if (!IS_TIM_SLAVE_INSTANCE(hpwm->timer) || !IS_TIM_SLAVEMODE_TRIGGER_ENABLED(hpwm->timer->SMCR)) {
        hpwm->timer->CR1 |= TIM_CR1_CEN;
    }
    
}


void pwm_dma_start32(pwm_handler_t *hpwm, uint32_t *buffer, uint16_t len)
{
    // Ensure there's no ongoing DMA transfer
    //if (hpwm->dma->CNDTR != 0) {
    //    return;
    //}

    // Disable the DMA channel before configuration
    hpwm->dma->CCR &= ~DMA_CCR_EN;

    // Set the peripheral and memory addresses and the number of data to transfer
    hpwm->dma->CPAR  = (uint32_t)hpwm->capture_compare;
    hpwm->dma->CMAR  = (uint32_t)buffer;
    hpwm->dma->CNDTR = len;

    // Enable the DMA channel and configure it to generate an interrupt on transfer completion
    hpwm->dma->CCR |= DMA_CCR_TCIE;
    hpwm->dma->CCR |= DMA_CCR_EN;

    // Configure the timer to generate DMA requests on update events
    hpwm->timer->DIER |= hpwm->dma_trigger;
    hpwm->timer->DIER |= TIM_DIER_UDE;

    // Enable the PWM channel associated with the DMA
    hpwm->timer->CCER &= ~(TIM_CCER_CC1E << (hpwm->channel & 0x1FU));
    hpwm->timer->CCER |= (TIM_CCx_ENABLE << hpwm->channel); // Make sure TIM_CCx_ENABLE is defined to the correct value

    // For timers with a Break feature, enable the main output
    if (IS_TIM_BREAK_INSTANCE(hpwm->timer)) {
        hpwm->timer->BDTR |= TIM_BDTR_MOE;
    }

    // Enable the timer if it's not in a slave mode triggered by an external event
    if (!IS_TIM_SLAVE_INSTANCE(hpwm->timer) || !IS_TIM_SLAVEMODE_TRIGGER_ENABLED(hpwm->timer->SMCR)) {
        hpwm->timer->CR1 |= TIM_CR1_CEN;
    }
}

/**
 * @brief  This function stops any ongoing DMA transmission.
 * @param  neopixel: The pointer to the instance of the neoPixel manager.
 * @retval None
 */
void pwm_dma_stop(pwm_handler_t *hpwm)
{
    // Disable the DMA channel to stop ongoing transfers
    hpwm->dma->CCR &= ~DMA_CCR_EN;
    
    // Reset the number of data items to transfer to 0 for the DMA channel
    hpwm->dma->CNDTR = 0;
    
    // Disable the DMA request trigger in the timer's DMA/interrupt enable register
    hpwm->timer->DIER &= ~hpwm->dma_trigger;

    // Disable the timer's Capture/Compare output to stop the PWM signal
    // Assuming hpwm->channel is the bit position (multiplied by 4) for the CCER register
    hpwm->timer->CCER &= ~(1UL << (hpwm->channel));

    // Record the last time the DMA transfer was stopped
    hpwm->last_dma_tx_tick = Get_Systick_Cnt();
}

uint8_t pwm_dma_is_busy(pwm_handler_t *hpwm){
	/* Wait at least 2ms since the last DMA transmission */
	return ((hpwm->dma->CNDTR != 0) || ((Get_Systick_Cnt() - hpwm->last_dma_tx_tick) < TIME_FOR_NEXT_STREAM));
}

uint8_t pwm_get_compare(pwm_handler_t *hpwm)
{
	return (uint8_t)(255 * (*((uint32_t *)hpwm->capture_compare)) / PWM_TIMER_PERIOD);
}

/**
 * @brief  This function initializes the timer in PWM mode for use with DMA.
 * @param  hpwm: The pointer to the instance of the PWM handler.
 * @param  pwm_frequency: The frequency of the PWM signal.
 * @param  pwm_duty_cycle: The initial duty cycle of the PWM signal.
 * @retval None
 */
// void pwm_timer_init(pwm_handler_t *hpwm, uint32_t pwm_frequency, uint32_t pwm_duty_cycle) {
//     // Enable timer clock
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Assuming TIM2 is used, modify as necessary for your timer

//     // Set up the timer
//     TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//     TIM_TimeBaseStructure.TIM_Period = (SystemCoreClock / pwm_frequency) - 1;
//     TIM_TimeBaseStructure.TIM_Prescaler = 0; // No prescaler
//     TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//     TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//     TIM_TimeBaseInit(hpwm->timer, &TIM_TimeBaseStructure);

//     // Set up PWM mode for the timer's channel
//     TIM_OCInitTypeDef TIM_OCInitStructure;
//     TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//     TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//     TIM_OCInitStructure.TIM_Pulse = pwm_duty_cycle; // CCRx register value for desired duty cycle
//     TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // Output polarity

//     // Init the PWM channel with the above configuration
//     TIM_OC3Init(hpwm->timer, &TIM_OCInitStructure); // Assuming using channel 3, modify as necessary

//     // Enable the corresponding preload register
//     TIM_ARRPreloadConfig(hpwm->timer, ENABLE);
//     TIM_OC3PreloadConfig(hpwm->timer, TIM_OCPreload_Enable); // Assuming using channel 3, modify as necessary

//     // Enable TIM Update DMA request
//     TIM_DMAConfig(hpwm->timer, TIM_DMABase_CCR3, TIM_DMABurstLength_1Transfer); // Assuming channel 3 for DMA burst
//     TIM_DMACmd(hpwm->timer, TIM_DMA_Update, ENABLE);

//     // Enable the timer
//     TIM_Cmd(hpwm->timer, ENABLE);
// }


