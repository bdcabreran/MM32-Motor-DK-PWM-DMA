/** 
 * @file     board.h
 * @author   Motor TEAM
 * @brief    This file provides all the functions prototypes for the board level support package.
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

#ifndef __BSP_H
#define __BSP_H

/** Files includes */
#include "HAL_conf.h"
#include "HAL_device.h"

/** 
 * @addtogroup MM32_Hardware_Driver_Layer
 * @{
 */

/** 
 * @addtogroup Bsp
 * @{
 */


/** LED interface */
#define LED_RCC_CLOCKGPIO           (RCC_AHBENR_GPIOC | RCC_AHBENR_GPIOD | RCC_AHBENR_GPIOA) 

#define LED1_PORT                   GPIOD
#define LED1_PIN                    GPIO_Pin_3

#define LED2_PORT                   GPIOC
#define LED2_PIN                    GPIO_Pin_5

#define LED3_PORT                   GPIOA
#define LED3_PIN                    GPIO_Pin_2

/** UART interface */
#define UART_RCC_CLOCKGPIO          (RCC_AHBENR_GPIOB)

#define TX_PORT                     GPIOB
#define TX_PIN                      GPIO_Pin_9
#define TX_SOURCE                   GPIO_PinSource9
#define TX_AF                       GPIO_AF_0

#define RX_PORT                     GPIOB
#define RX_PIN                      GPIO_Pin_8
#define RX_SOURCE                   GPIO_PinSource8
#define RX_AF                       GPIO_AF_0

extern void Board_Gpio_Init(void);
extern void Board_Gpio_Af_Init(void);


/**
  * @}
*/

/**
  * @}
*/


#endif
