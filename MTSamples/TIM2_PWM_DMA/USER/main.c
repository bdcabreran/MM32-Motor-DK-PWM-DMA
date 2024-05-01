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


int main(void)
{
    Systick_Init(SystemCoreClock / 1000);
    Board_Gpio_Af_Init();
    Drv_Uart_Init(115200);
    Board_Gpio_Init();

    DBG_MSG("MCU Frequency: %d Hz\r\n", SystemCoreClock);		
    PWM_DMA_Init();

    neopixel_init(&neopixel, PWM_DMA_Start, PWM_DMA_Stop);
    neopixel_anim_init(&anim, &neopixel);    
    
    //neopixel_anim_blink(&anim, 100, 0);

    // neopixel_set_pixel_color(&neopixel, 0, 255, 0, 0);
    // neopixel_anim_fade_in(&anim, 100, 100);

    // neopixel_set_pixel_color(&neopixel, 0, 255, 0, 0);
    // neopixel_anim_fade_out(&anim, 100, 100);

    // neopixel_anim_rainbow(&anim, 500);

    // neopixel_anim_rainbow_cycle(&anim, 50);

    // neopixel_anim_color_wipe(&anim, 100, 0xAABBCC);

    //neopixel_anim_theater_chase(&anim, 100, 0x00FF00);

    //neopixel_anim_theater_chase_rainbow(&anim, 100);

    // neopixel_set_pixel_color_rgb(&neopixel, 0, 0x00FF00); // Green
    // neopixel_update(&neopixel);

    neopixel_anim_rainbow_water(&anim, 100);

		uint32_t last_tick = Get_Systick_Cnt(); 
		
    while(1)
    {
      blink_led();
      print_msg();
      //send_pwm();
      neopixel_anim_update(&anim);
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
    // DBG_MSG("dma_transfer_cplt: %d\r\n", dma_transfer_cplt_cnt);
  }
}

void send_pwm(void)
{
  static uint32_t last_tick = 0;
  if (Get_Systick_Cnt() - last_tick > 500)
  {
    last_tick = Get_Systick_Cnt();

    #if 0
    static uint8_t counter = 0;
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    switch (counter % 3) {
      case 0:
        red = 255;
        break;
      case 1:
        green = 255;
        break;
      case 2:
        blue = 255;
        break;
    }
    counter++;
    set_pixel_color(0, red, green, blue, data);
    #endif 

    // GRB
    uint8_t green = 0;
    uint8_t red = 0;
    uint8_t blue = 0;

    //neopixel_set_pixel_color(&neopixel, 5, 255, 0, 0);
    // neopixel_set_pixel_color_rgb(&neopixel, 6, 0x00FF00); // Green
    //neopixel_update(&neopixel);

    // static uint8_t brightness = 100;

    // if (brightness > 0) {
    //   brightness--;
    // } else {
    //   brightness = 100;
    // }

    // neopixel_set_brightness(&neopixel, brightness);
    // neopixel_update(&neopixel);
    // DBG_MSG("brightness: %d\r\n", brightness);


    //neopixel_set_pixel_color_rgb(&neopixel, 0, 0x00FFFF); // Green
    //neopixel_update(&neopixel);


    //DBG_MSG("dma_transfer_cplt_cnt: %d\r\n",dma_transfer_cplt_cnt);
  }
}

// void updateLEDColor(int led_num) {
//     static uint8_t green = 0;
//     static uint8_t red = 0;
//     static uint8_t blue = 0;
//     static int colorStage = 0;  // 0 = Green, 1 = Red, 2 = Blue

//     //DBG_MSG("g: %d, r: %d, b: %d, cS: %d\r\n", green, red, blue, colorStage);

//     // Check which color to increment
//     switch (colorStage) {
//         case 0:  // Green cycle
//             if (green < 255) {
//                 green++;
//                 set_pixel_color(led_num, red, green, blue, data);
//             } else {
//                 green = 0;
//                 colorStage = 1;
//             }
//             break;
//         case 1:  // Red cycle
//             if (red < 255) {
//                 red++;
//                 set_pixel_color(led_num, red, green, blue, data);
//             } else {
//                 red = 0;
//                 colorStage = 2;
//             }
//             break;
//         case 2:  // Blue cycle
//             if (blue < 255) {
//                 blue++;
//                 set_pixel_color(led_num, red, green, blue, data);
//             } else {
//                 blue = 0;
//                 colorStage = 0;
//             }
//             break;
//     }
// }