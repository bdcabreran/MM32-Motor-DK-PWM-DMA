/** 
 * @file     drv_led.c
 * @author   Motor TEAM
 * @brief    This file provides all the driver functions for the led.
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
#define _DRV_LED_C_

/** Files includes */
#include "board.h"
/** 
 * @addtogroup MM32_Hardware_Driver_Layer
 * @{
 */

/** 
 * @addtogroup Drv_LED
 * @{
 */

void Led1_On(void)
{
    LED1_PORT->BSRR = LED1_PIN;
}

void Led1_Off(void)
{
    LED1_PORT->BRR = LED1_PIN;
}

void Led1_Toggle(void)
{
    LED1_PORT->ODR ^= LED1_PIN;
}

void Led2_On(void)
{
    LED2_PORT->BSRR = LED2_PIN;
}

void Led2_Off(void)
{
    LED2_PORT->BRR = LED2_PIN;
}

void Led2_Toggle(void)
{
    LED2_PORT->ODR ^= LED2_PIN;
}

/**
  * @}
*/

/**
  * @}
*/
