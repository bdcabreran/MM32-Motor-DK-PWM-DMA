/**
 * @file     main.c
 * @author   Motor TEAM
 * @brief    This file provides the main functions and test samples.
 *
 * @attention
 *
 * THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
 * CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
 * TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
 * HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
 * CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
 *
 * <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2>
 */

/** Define to prevent recursive inclusion */
#define _MAIN_C_

/** Files includes */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hal_conf.h"
#include "hal_device.h"
#include "systick.h"
#include "board.h"
#include "drv_led.h"
#include "drv_uart.h"
#include "hal_tim.h"
#include "hal_dma.h"

#define DEBUG_BUFFER_SIZE 256
char debugBuffer[DEBUG_BUFFER_SIZE];

#define ENABLE_DBG_MSG 1

#if ENABLE_DBG_MSG
#define DBG_MSG(fmt, ...) do { \
  snprintf(debugBuffer, DEBUG_BUFFER_SIZE, (fmt), ##__VA_ARGS__); \
  Uart_Put_Buff(debugBuffer, strlen(debugBuffer)); \
} while(0)
#else
#define DBG_MSG(fmt, ...) do { } while(0)
#endif

#define DATA_LEN  (6)
u8 data[DATA_LEN] = {0};

void TIM2_PWM_Init(u16 arr, u16 psc);
void TIM2_GPIO_Init(void);
void blink_led(void);
void TIM2_IRQ_Init(void);
void print_msg(void);
void send_pwm(void);
void TIM2_DMA_Init(void);

/** Extern Variables*/
extern uint32_t SystemCoreClock;
volatile uint32_t TimUpdateCnt = 0;

volatile uint8_t dma_transfer_cplt = 0;


int main(void)
{
    Systick_Init(SystemCoreClock / 1000);
    Board_Gpio_Af_Init();
    Drv_Uart_Init(115200);
    Board_Gpio_Init();


    DBG_MSG("MCU Frequency: %d Hz\r\n", SystemCoreClock);		
    
    TIM2_GPIO_Init();
    TIM2_IRQ_Init();
    uint32_t pwm_freq = 800000;
    uint32_t arr = ((SystemCoreClock / pwm_freq) - 1);
    TIM2_PWM_Init(arr, 0);
    TIM2_DMA_Init();

    DBG_MSG("TIM2 ARR: %d\r\n", READ_REG(TIM2->ARR));		
    TIM_SetCompare3(TIM2, 0);
    for (size_t i = 0; i < DATA_LEN; i++)
    {
      //data[i] = READ_REG(TIM2->ARR)/2;
      data[i] = 30;
    }
    DBG_MSG("TIM2 CCR3: %d\r\n", READ_REG(TIM2->CCR3));
    DBG_MSG("data[0]: %d\r\n", data[0]);  
    

    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    TIM_SetCounter(TIM2, 0);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    
		uint32_t last_tick = Get_Systick_Cnt(); 
		
    while(1)
    {
        blink_led();
        print_msg();
        send_pwm();

    }
}

void TransferComplete_Callback()
{
  dma_transfer_cplt++;
  // TIM_Cmd(TIM2, DISABLE);
  // DMA_Cmd(DMA1_Channel1, DISABLE);

  TIM_CtrlPWMOutputs(TIM2, DISABLE);
  TIM_Cmd(TIM2, DISABLE);
  DMA_Cmd(DMA1_Channel1, DISABLE);

  TIM_SetCompare3(TIM2, 0);
  TIM_SetCounter(TIM2, 0);
}

void DMA1_Channel1_IRQHandler(void) {
    // Check for DMA half transfer complete interrupt
    if (DMA_GetITStatus(DMA1_IT_HT1)) {
        DMA_ClearITPendingBit(DMA1_IT_HT1);

    }

    // Check for DMA transfer complete interrupt
    if (DMA_GetITStatus(DMA1_IT_TC1)) {
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        			
        TransferComplete_Callback();
    }

    // Check for DMA transfer error interrupt
    if (DMA_GetITStatus(DMA1_IT_TE1)) {
        DMA_ClearITPendingBit(DMA1_IT_TE1);
    }
}


void TimerUpdate_Callback(void)
{
  TimUpdateCnt++;
}

void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        TimerUpdate_Callback();
    }

    if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
   {
       TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
   }

}

void TIM2_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_GPIO_ClockCmd(GPIOA, ENABLE);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_2);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void TIM2_PWM_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
    TIM_OCInitTypeDef  TIM_OCInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM2, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Period    = arr;
    TIM_TimeBaseStruct.TIM_Prescaler = psc;
    //Setting Clock Segmentation
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    ///TIM Upward Counting Mode
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);

    TIM_OCStructInit(&TIM_OCInitStruct);
    //Select Timer Mode: TIM Pulse Width Modulation Mode 2
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    //Setting the Pulse Value of the Capture Comparison Register to be Loaded
    TIM_OCInitStruct.TIM_Pulse = 0;
    //Output polarity: TIM output is more polar
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC3Init(TIM2, &TIM_OCInitStruct);

    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);

    //Enable the update interrupt
    //TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    //TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    //Enable the DMA request
    TIM_DMACmd(TIM2, TIM_DMA_CC3, ENABLE);

    TIM_CtrlPWMOutputs(TIM2, DISABLE);
    TIM_Cmd(TIM2, DISABLE);
}

void TIM2_IRQ_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


void TIM2_DMA_Init(void)
{
    DMA_InitTypeDef  DMA_InitStruct;
    DMA_Channel_TypeDef *channel;

    channel = DMA1_Channel1;
    RCC_DMA_ClockCmd(DMA1, ENABLE);

    DMA_DeInit(channel);
    DMA_StructInit(&DMA_InitStruct);
    //Transfer register address
    DMA_InitStruct.DMA_PeripheralBaseAddr    = (u32) & (TIM2->CCR3);
    //Transfer memory address
    DMA_InitStruct.DMA_MemoryBaseAddr        = (u32)data;
    //Transfer direction, from memory to register
    DMA_InitStruct.DMA_DIR                   = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize            = DATA_LEN;
    DMA_InitStruct.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    //Transfer completed memory address increment
    DMA_InitStruct.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority              = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M                   = DMA_M2M_Disable;
    DMA_InitStruct.DMA_Auto_reload = DMA_Auto_Reload_Disable;
    DMA_Init(channel, &DMA_InitStruct);

    DMA_ITConfig(channel, DMA_IT_TC, ENABLE);
}

void blink_led(void)
{
  static uint32_t last_tick = 0;
  static uint8_t led_state = Bit_RESET;
  if (Get_Systick_Cnt() - last_tick > 1000)
  {
    GPIO_WriteBit(LED1_PORT, LED1_PIN, led_state);
    led_state = (led_state == Bit_SET) ? Bit_RESET : Bit_SET;
    last_tick = Get_Systick_Cnt();
    // DBG_MSG("Blink\r\n");
  }
}


void print_msg(void)
{
  static uint32_t last_tick = 0;
  if (Get_Systick_Cnt() - last_tick > 1000)
  {
    last_tick = Get_Systick_Cnt();
    // DBG_MSG("TIM2 CNT: %d\r\n", READ_REG(TIM2->CNT));
    // DBG_MSG("TIM2 ARR: %d\r\n", READ_REG(TIM2->ARR));
    // DBG_MSG("TIM2 CCR3: %d\r\n", READ_REG(TIM2->CCR3));
    // DBG_MSG("TIM Update Cnt: %d\r\n", TimUpdateCnt);
    DBG_MSG("dma_transfer_cplt: %d\r\n", dma_transfer_cplt);
  }
}

void send_pwm(void)
{
  static uint32_t last_tick = 0;
  if (Get_Systick_Cnt() - last_tick > 500)
  {
    last_tick = Get_Systick_Cnt();

    // update data 
    static uint8_t arr = 1;
    if (arr < READ_REG(TIM2->ARR) - 1) {
      for (size_t i = 0; i < DATA_LEN; i++) {
        data[i] = arr;
      }
    }
    else {
      arr = 1;
    }
    arr += 1;

    exDMA_SetMemoryAddress(DMA1_Channel1, (uint32_t)data); // Set the memory address
    exDMA_SetTransmitLen(DMA1_Channel1, DATA_LEN); // Set the transmit length

    DMA_Cmd(DMA1_Channel1, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_SetCompare3(TIM2, 0);
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);

    DBG_MSG("arr:  %d, dma_transfer_cplt: %d\r\n", arr, dma_transfer_cplt);
  }
}

/**
  * @}
*/

/**
  * @}
*/
