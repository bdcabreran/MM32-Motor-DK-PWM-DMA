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
#include "pwm_dma.h"
#include "neopixel.h"
#include "neopixel_animation.h"
#include "bsp_led_pwm.h"
#include "p_config.h"
#include "p_tasks.h"

volatile bool SleepTriggered = false;


#define DEBUG_BUFFER_SIZE 256
char debugBuffer[DEBUG_BUFFER_SIZE];

#define USE_PWM_MODE_2 0
#define ENABLE_DBG_MSG 1

#if ENABLE_DBG_MSG
#define DBG_MSG(fmt, ...) do { \
  snprintf(debugBuffer, DEBUG_BUFFER_SIZE, (fmt), ##__VA_ARGS__); \
  Uart_Put_Buff(debugBuffer, strlen(debugBuffer)); \
} while(0)
#else
#define DBG_MSG(fmt, ...) do { } while(0)
#endif

void blink_led(void);
void print_msg(void);
void send_pwm(void);
void set_pixel_color(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue, uint8_t* led_data);
void updateLEDColor(int led_num);

/** Extern Variables*/
extern uint32_t SystemCoreClock;
volatile uint32_t TimUpdateCnt = 0;
volatile uint8_t dma_transfer_cplt_cnt = 0;
neopixel_t neopixel;
neopixel_animation_t anim;

void print_prescaler_values() {
    // Access RCC register to read APB1 prescaler
    uint32_t apb1_prescaler = RCC->CFGR & RCC_CFGR_PPRE1; // Assuming APB1 is configured using bits RCC_CFGR_PPRE

    // Determine prescaler value
    uint32_t prescaler_value = 1; // Default value
    switch(apb1_prescaler) {
        case RCC_CFGR_PPRE1_DIV1:
            prescaler_value = 1;
            break;
        case RCC_CFGR_PPRE1_DIV2:
            prescaler_value = 2;
            break;
        case RCC_CFGR_PPRE1_DIV4:
            prescaler_value = 4;
            break;
        case RCC_CFGR_PPRE1_DIV8:
            prescaler_value = 8;
            break;
        case RCC_CFGR_PPRE1_DIV16:
            prescaler_value = 16;
            break;
    }

    // Print the prescaler value
    // Example: Assuming UART for output
    DBG_MSG("APB1 Prescaler Value: %u\r\n", prescaler_value);
}

#if 0
/**
 * @brief  Callback function for LED animation Events.
 */
static void LED_Complete_Callback(LED_Animation_Type_t animationType, LED_Status_t status)
{
      switch (status)
    {
    case LED_STATUS_ANIMATION_STARTED:
    DBG_MSG("LED_STATUS_ANIMATION_STARTED\r\n");
        break;
      #if USE_WHITE_LED
          // Turn off White LED
          LED_RGYW_WHITE_OFF;
      #endif 
        break;

    case LED_STATUS_ANIMATION_COMPLETED:
    DBG_MSG("LED_STATUS_ANIMATION_COMPLETED\r\n");
        break;

    case LED_STATUS_ANIMATION_STOPPED:
    DBG_MSG("LED_STATUS_ANIMATION_STOPPED\r\n");
        break;

    case LED_STATUS_ANIMATION_TRANSITION_STARTED:
    DBG_MSG("LED_STATUS_ANIMATION_TRANSITION_STARTED\r\n");
      #if USE_WHITE_LED
          // Turn off White LED
          LED_RGYW_WHITE_OFF;
      #endif 
        break;

    case LED_STATUS_ANIMATION_TRANSITION_COMPLETED:
    {
      DBG_MSG("LED_STATUS_ANIMATION_TRANSITION_COMPLETED\r\n");
      // Once the LED animation is completed, turn on the white LED
      #if USE_WHITE_LED
      if (TurnOnWhiteLEDOnCompletion && LED_Transition_IsLEDOff(&LEDTransition))
      {
        TurnOnWhiteLEDOnCompletion = false;
        LED_RGYW_WHITE_ON;
      }
      #endif 
    }
      break;

    default:
        if (IS_LED_ERROR_STATUS(status))
        {
            DBG_MSG("LED_STATUS_ERROR\r\n");
        }
        break;
    }

}
#endif

int main(void)
{
    Systick_Init(SystemCoreClock / 1000);
    Board_Gpio_Af_Init();
    Drv_Uart_Init(115200);
    Board_Gpio_Init();

    DBG_MSG("MCU Frequency: %d Hz\r\n", SystemCoreClock);		
    // PWM_DMA_Init();

    DBG_MSG("RCC_GetHCLKFreq: %d Hz\r\n", RCC_GetHCLKFreq());
    DBG_MSG("RCC_GetPCLK1Freq: %d Hz\r\n", RCC_GetPCLK1Freq());
    DBG_MSG("RCC_GetPCLK2Freq: %d Hz\r\n", RCC_GetPCLK2Freq());
   print_prescaler_values();

    LED_RGYW_TIM_Init();

    PD_Initialise();
    
    // // // LED Complete Callback is optional and can be used to trigger other events when LED animations are complete.
    // LED_Animation_Init(&MainLED, &LED_Controller, LED_Complete_Callback);
    // LED_Transition_Init(&LEDTransition, &MainLED);

    // // This Mapping is optional and can be used to map the LED transitions to the LED animations.
    // LED_Transition_SetMapping(&LEDTransition, LEDTransitionMap, LED_TRANSITION_MAP_SIZE);

    // LED_Transition_ToSolid(&LEDTransition, &LED_Solid_DefaultColor, LED_TRANSITION_INTERPOLATE, 300);

    DBG_MSG("I'm Alive\r\n");

		uint32_t last_tick = Get_Systick_Cnt(); 

    PD_BtnPwr_OnShortButtonPress();
		
    while(1)
    {

      if (Get_Systick_Cnt() != last_tick)
      {
        // LED_Transition_Update(&LEDTransition, Get_Systick_Cnt());
        PD_RunProductControlTasks();

        last_tick = Get_Systick_Cnt();
      }

      blink_led();
      print_msg();
    }
}

void TransferComplete_Callback()
{
  dma_transfer_cplt_cnt++;
  neopixel.stop_dma();
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


void blink_led(void)
{
  static uint32_t last_tick = 0;
  static uint8_t led_state = Bit_RESET;
  if (Get_Systick_Cnt() - last_tick > 1000)
  {
    GPIO_WriteBit(LED1_PORT, LED1_PIN, led_state);
    led_state = (led_state == Bit_SET) ? Bit_RESET : Bit_SET;
    last_tick = Get_Systick_Cnt();
  }
}


void print_msg(void)
{
  static uint32_t last_tick = 0;
  static uint16_t redCycle = 0;
  if (Get_Systick_Cnt() - last_tick > 20)
  {
    last_tick = Get_Systick_Cnt();
    // redCycle = (redCycle + 1) % 1000;
    // LED_SetPWM_Red(redCycle);
  }
}

void send_pwm(void)
{
  static uint32_t last_tick = 0;
  if (Get_Systick_Cnt() - last_tick > 1000)
  {
    last_tick = Get_Systick_Cnt();

    // Send the command after 5 seconds
    // neopixel_anim_close(&anim, 100);
		neopixel_anim_solid(&anim, 0x00FF00); 
  }
}
