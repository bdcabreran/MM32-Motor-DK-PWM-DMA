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
#include "ws2812.h"
#include "hal_tim.h"
#include "hal_dma.h"
#include "pwm.h"


#define DEBUG_BUFFER_SIZE 256
char debugBuffer[DEBUG_BUFFER_SIZE];

#define DBG_MSG(fmt, ...) do { \
  snprintf(debugBuffer, DEBUG_BUFFER_SIZE, (fmt), ##__VA_ARGS__); \
  Uart_Put_Buff(debugBuffer, strlen(debugBuffer)); \
} while(0)

#define USE_DMA 1

extern uint8_t led_data[WS2812_NUM_LEDS * WS2812_BIT_PER_LED]; // 24 bits per LED

uint8_t captured_cc3[10] = {0}; 
pwm_handler_t pwm_neopixel = {TIM2, &TIM2->CCR3, TIM_Channel_3, TIM_DIER_CC3DE, DMA1_Channel1};


uint16_t u16DMAData; /** DMA transfer data */
uint8_t u8Tim1UpdateFlag = 0;/** Set in TIM1 update interrupt,reset in main while */
uint8_t u8TransferCplt = 0;
uint32_t UpdateEventCnt = 0;
uint8_t print_once = 0;
volatile uint32_t pwm_pulse_count = 0; // Counter for the number of PWM pulses

extern uint32_t SystemCoreClock;
/**
 * @addtogroup MM32_User_Layer
 * @{
 */

/**
 * @addtogroup User_Main
 * @{
 */
int8_t i8UartBuffer[] = "hello world\r\n";
/**

 * @brief    : This function is the main entry.
 * @param    : None
 * @retval   : None
 */
int main(void)
{
    Systick_Init(SystemCoreClock / 1000);
    Board_Gpio_Af_Init();
    Drv_Uart_Init(115200);
    Board_Gpio_Init();

    Systick_Delay(2);

    uint8_t led_state = Bit_RESET;
  
    DBG_MSG("MCU Frequency: %d Hz\r\n", SystemCoreClock);		
    DBG_MSG("led data addr = 0x%X\r\n", &led_data[0]);

    
		ws2812Init();

    DBG_MSG("TIM2->ARR = %d\r\n", TIM2->ARR);

		//WS2812_SetPixelColor(0, 0, 0, 0, led_data);
		//WS2812_SetPixelColor32(0, 0, 0, 0, led_data);

    for (size_t i = 0; i < WS2812_BIT_PER_LED; i++)
    {
      led_data[i] = 80;
    }
		
		//pwm_start(&pwm_neopixel, 50);
	  //pwm_dma_start(&pwm_neopixel, led_data, sizeof(led_data));
		//pwm_dma_start32(&pwm_neopixel, led_data, sizeof(led_data));

    //TIM_Cmd(TIM2, ENABLE);
    //DMA_Cmd(DMA1_Channel1, ENABLE);
		
		Start_WS2812_PWM_DMA();

		uint32_t last_tick = Get_Systick_Cnt(); 
		
    while(1)
    {

				#if 0
        if (u8TransferCplt)
        {
          u8TransferCplt = 0;
          //WS2812SetPixelColor(1, 255, 0, 0);  // red
					DBG_MSG("Update event Counter = %d\r\n", UpdateEventCnt);
          Systick_Delay(5);
        }
				#endif

        #if 1

        // // blink led
        if(Get_Systick_Cnt() - last_tick > 1000)
        {
            GPIO_WriteBit(LED1_PORT, LED1_PIN, led_state);
            led_state = (led_state == Bit_SET) ? Bit_RESET : Bit_SET;
            last_tick = Get_Systick_Cnt();

            static uint8_t r = 0;
            static uint8_t g = 0;
            static uint8_t b = 0;
            //WS2812_SetPixelColor(0, r, 0, 0, led_data);
            //WS2812_SetPixelColor32(0, r, 0, 0, led_data);
            r = (r + 1) % 255;
						
					  //DBG_MSG("Start DMA\r\n");
					   //DBG_MSG("MCU Frequency: %d Hz\r\n", SystemCoreClock);		

						Start_WS2812_PWM_DMA();

            
						//TIM_Cmd(TIM2, ENABLE);
						//DMA_Cmd(DMA1_Channel1, ENABLE);

						//pwm_start(&pwm_neopixel, 50);

						//pwm_dma_start(&pwm_neopixel, led_data, sizeof(led_data));
            //pwm_dma_start32(&pwm_neopixel, led_data, sizeof(led_data));
            //DBG_MSG("r = %d, g = %d, b = %d\r\n", r, g, b);
            if (print_once)
            {
              DBG_MSG("Pulse Count = %d\r\n", pwm_pulse_count);
              print_once = 1;
							UpdateEventCnt = 0;
              TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
              TIM_CCxCmd(TIM2, TIM_Channel_3, TIM_CCx_Enable);
							TIM_Cmd(TIM2, ENABLE);
            }
					
        }
        #endif
    }
}



/**
  * @brief  DMA transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated after DMA transfer
  * @retval None
  */
void TransferComplete_Callback()
{
  // pwm_dma_stop(&pwm_neopixel);
	Stop_WS2812_PWM_DMA();
}

void DMA1_Channel1_IRQHandler(void) {
    // Check for DMA half transfer complete interrupt
    if (DMA_GetITStatus(DMA1_IT_HT1)) {
        DMA_ClearITPendingBit(DMA1_IT_HT1);

			  #if 0
				Stop_WS2812_PWM_DMA();
        DBG_MSG("HT\r\n");
        DBG_MSG("DMA DC = %d\r\n", DMA_GetCurrDataCounter(DMA1_Channel1));
        DBG_MSG("DMA CMAR = 0x%x\r\n", DMA1_Channel1->CMAR);
        #endif 
    }

    // Check for DMA transfer complete interrupt
    if (DMA_GetITStatus(DMA1_IT_TC1)) {
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        
        u8TransferCplt = 1;
			
				#if 0
				Stop_WS2812_PWM_DMA();
        DBG_MSG("TC\r\n");
        DBG_MSG("DMA DC = %d\r\n", DMA_GetCurrDataCounter(DMA1_Channel1));
        DBG_MSG("DMA CMAR = 0x%x\r\n", DMA1_Channel1->CMAR);
			  #else 
			  //Stop_WS2812_PWM_DMA();
        #endif 
        TransferComplete_Callback();

    }

    // Check for DMA transfer error interrupt
    if (DMA_GetITStatus(DMA1_IT_TE1)) {
        DMA_ClearITPendingBit(DMA1_IT_TE1);
        DBG_MSG("Transfer Error\r\n");
        // Error handling
    }
}


void TimerUpdate_Callback(void)
{
  pwm_pulse_count++;
  
  if (pwm_pulse_count >= 5) {
      // // Disable the TIM2 Update interrupt
      TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);

      // // Disable PWM output (you can reset the counter or stop the timer based on your requirement)
      TIM_CCxCmd(TIM2, TIM_Channel_3, TIM_CCx_Disable);

      // Reset the counter
      pwm_pulse_count = 0;

      // Optionally, stop the timer
      TIM_Cmd(TIM2, DISABLE);
  }
}

void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        // DBG_MSG("TIM_IT_Update\r\n");
        //u8Tim1UpdateFlag = 1;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        TimerUpdate_Callback();

    }

    if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
    {
        //u8Tim1UpdateFlag = 1;
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
        //TimerUpdate_Callback();
    }

}

/**
  * @}
*/

/**
  * @}
*/
