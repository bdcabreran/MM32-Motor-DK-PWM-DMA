/** 
 * @file     main.h
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
#ifndef __MAIN_H
#define __MAIN_H

/** Files includes */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


/** 
 * @addtogroup MM32_User_Layer
 * @{
 */

/** 
 * @addtogroup User_Main
 * @{
 */
#define TURBOL_LED_PORT GPIOB
#define TURBOL_LED_PIN GPIO_Pin_1

#define SPEED1_LED_PORT GPIOB
#define SPEED1_LED_PIN GPIO_Pin_4

#define SPEED2_LED_PORT GPIOB
#define SPEED2_LED_PIN GPIO_Pin_5

#define SPEED3_LED_PORT GPIOB
#define SPEED3_LED_PIN GPIO_Pin_0

// #define TURBOL_LED_ON GPIO_SetBits(TURBOL_LED_PORT, TURBOL_LED_PIN)
// #define TURBOL_LED_OFF GPIO_ResetBits(TURBOL_LED_PORT, TURBOL_LED_PIN)
// #define SPEED1_LED_ON GPIO_SetBits(SPEED1_LED_PORT, SPEED1_LED_PIN)
// #define SPEED1_LED_OFF GPIO_ResetBits(SPEED1_LED_PORT, SPEED1_LED_PIN)
// #define SPEED2_LED_ON GPIO_SetBits(SPEED2_LED_PORT, SPEED2_LED_PIN)
// #define SPEED2_LED_OFF GPIO_ResetBits(SPEED2_LED_PORT, SPEED2_LED_PIN)
// #define SPEED3_LED_ON GPIO_SetBits(SPEED3_LED_PORT, SPEED3_LED_PIN)
// #define SPEED3_LED_OFF GPIO_ResetBits(SPEED3_LED_PORT, SPEED3_LED_PIN)

#define TURBOL_LED_ON  /* Do nothing */
#define TURBOL_LED_OFF /* Do nothing */
#define SPEED1_LED_ON  /* Do nothing */
#define SPEED1_LED_OFF /* Do nothing */
#define SPEED2_LED_ON  /* Do nothing */
#define SPEED2_LED_OFF /* Do nothing */
#define SPEED3_LED_ON  /* Do nothing */
#define SPEED3_LED_OFF /* Do nothing */

extern uint8_t u8Tim1UpdateFlag;
/**  defind in startup_mm32spin028x.c  */
extern uint32_t SystemCoreClock;


extern volatile bool SleepTriggered;

/**
  * @}
*/

/**
  * @}
*/

#endif
