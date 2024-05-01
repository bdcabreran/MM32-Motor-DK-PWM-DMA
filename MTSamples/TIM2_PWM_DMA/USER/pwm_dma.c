/**
 * @file pwm.c
 * @author Bayron Cabrera (bayron.nanez@gmail.com)
 * @brief  This Function Contains a Basic Library to Configure the PWM in the TIM2 
 *         of a Cortex M0 Microcontroller from MM32SPIN028x family.
 * @version 0.1
 * @date 2024-05-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "pwm_dma.h"

// Configuration for the timer and DMA
#define PWM_TIM           TIM2
#define PWM_DMA           DMA1
#define PWM_DMA_CHANNEL   DMA1_Channel1
#define PWM_DMA_IRQ       DMA1_Channel1_IRQn
#define PWM_TIM_IRQ       TIM2_IRQn
#define PWM_TIM_CHANNEL   TIM_Channel_3
#define PWM_GPIO_PORT     GPIOA
#define PWM_GPIO_PIN      GPIO_Pin_2
#define PWM_GPIO_AF       GPIO_AF_2
#define PWM_GPIO_SOURCE   GPIO_PinSource2
#define PWM_GPIO_CLOCK    RCC_AHBENR_GPIOA
#define PWM_TIMER_CLOCK   RCC_APB1ENR_TIM2

// Function prototypes for internal use
static void PWM_GPIO_Init(void);
static void PWM_Timer_Init(void);
static void PWM_DMA_Init_Internal(void);
static void PWM_DMA_IRQ_Init(void);

void PWM_DMA_Init(void) {
    PWM_GPIO_Init();
    PWM_DMA_IRQ_Init();
    PWM_Timer_Init();
    PWM_DMA_Init_Internal();
}

static void PWM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_GPIO_ClockCmd(PWM_GPIO_PORT, ENABLE);
    GPIO_PinAFConfig(PWM_GPIO_PORT, PWM_GPIO_SOURCE, PWM_GPIO_AF);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PWM_GPIO_PORT, &GPIO_InitStruct);
}

static void PWM_Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;

    // Verify the Timer Clock belongs to APB1, update the value if necessary
    RCC_APB1PeriphClockCmd(PWM_TIMER_CLOCK, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Period = PWM_TIMER_PERIOD;
    TIM_TimeBaseStruct.TIM_Prescaler = PWM_TIMER_PRESCALER;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(PWM_TIM, &TIM_TimeBaseStruct);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

    // PWM Channel 3 Configuration
    TIM_OC3Init(PWM_TIM, &TIM_OCInitStruct);
    TIM_OC3PreloadConfig(PWM_TIM, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(PWM_TIM, ENABLE);

    // Enable DMA Request for PWM Channel 3
    TIM_DMACmd(PWM_TIM, TIM_DMA_CC3, ENABLE);

    TIM_Cmd(PWM_TIM, DISABLE);
    TIM_CtrlPWMOutputs(PWM_TIM, DISABLE);
}

static void PWM_DMA_IRQ_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = PWM_TIM_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = PWM_DMA_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

static void PWM_DMA_Init_Internal(void)
{
    DMA_InitTypeDef  DMA_InitStruct;
    DMA_Channel_TypeDef *channel;
    RCC_DMA_ClockCmd(PWM_DMA, ENABLE);

    DMA_DeInit(PWM_DMA_CHANNEL);
    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr    = (u32) & (TIM2->CCR3);
    DMA_InitStruct.DMA_MemoryBaseAddr        = (u32)NULL;
    DMA_InitStruct.DMA_DIR                   = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize            = 0;
    DMA_InitStruct.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority              = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M                   = DMA_M2M_Disable;
    DMA_InitStruct.DMA_Auto_reload           = DMA_Auto_Reload_Disable;
    DMA_Init(PWM_DMA_CHANNEL, &DMA_InitStruct);

    DMA_ITConfig(PWM_DMA_CHANNEL, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE), ENABLE);
}


void PWM_DMA_Start(uint8_t* data, uint16_t len)
{
  exDMA_SetMemoryAddress(DMA1_Channel1, (uint32_t)data); 
  exDMA_SetTransmitLen(DMA1_Channel1, len);

  TIM_SetCompare3(TIM2, 0);
  TIM_SetCounter(TIM2, 0);
  TIM_Cmd(TIM2, ENABLE);
  TIM_CtrlPWMOutputs(TIM2, ENABLE);
  DMA_Cmd(DMA1_Channel1, ENABLE);
}

void PWM_DMA_Stop(void)
{
  TIM_Cmd(TIM2, DISABLE);
  TIM_CtrlPWMOutputs(TIM2, DISABLE);
  DMA_Cmd(DMA1_Channel1, DISABLE);

  TIM_SetCompare3(TIM2, 0);
  TIM_SetCounter(TIM2, 0);
}

