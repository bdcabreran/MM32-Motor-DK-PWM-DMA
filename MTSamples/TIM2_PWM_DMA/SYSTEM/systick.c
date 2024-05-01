/** 
 * @file     systick.c
 * @author   Motor TEAM
 * @brief    This file provides config and functions for the systick.
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
#define _SYSTICK_C_

/** Files includes */

#include "systick.h"

/** 
 * @addtogroup MM32_System_Layer
 * @{
 */

/** 
 * @addtogroup Systick
 * @{
 */

static volatile uint32_t _u32Systicks = 0;

/**
 * @brief    : This function will be used to initialize SysTick.
 * @param    : ticks
 * SystemFrequency / 1000    1ms raise interrupt
 * SystemFrequency / 100000	10us raise interrupt
 * SystemFrequency / 1000000 1us raise interrupt
 * @retval   : None
 */
void Systick_Init(uint32_t ticks)
{
    /** Initialization Systick interrupt */
    NVIC_SetPriority(SysTick_IRQn, 0);
    SysTick_Config(ticks);
}

/**
 * @brief    : Counter increment function.
 * @param    : None
 * @retval   : None
 */
static void Inc_Systicks(void)
{
    _u32Systicks ++;
}
/**
 * @brief    : Get counter function.
 * @param    : None
 * @retval   : None
 */
uint32_t Get_Systick_Cnt(void)
{
    return _u32Systicks;
}
/**
 * @brief    : Get the value of the counter.
 * @param    : None
 * @retval   : None
 */
uint32_t Get_Systick_Val(void)
{
    return SysTick->VAL;
}

/**
 * @brief    : Clean the systick overflow flag.
 * @param    : None
 * @retval   : None
 */
void Clear_Over_Flag(void)
{
    SysTick->CTRL;
}

/**
 * @brief    : Get the systick overflow flag.
 * @param    : None
 * @retval   : None
 */
uint32_t Get_Over_Flag(void)
{
    return SysTick->CTRL & (1 << 16);
}

/**
 * @brief    : Systick delay function.
 * @param    : Delay
 * The unit is the time of a systick interruption.
 * @retval   : None
 */
void Systick_Delay(volatile uint32_t Delay)
{
    uint32_t tickstart = 0;
    tickstart = Get_Systick_Cnt();
    while ((Get_Systick_Cnt() - tickstart) < Delay) {
    }
}

/**
 * @brief    : Suspend the systick.
 * @param    : None
 * @retval   : None
 */
void Suspend_Systicks(void)
{
    CLEAR_BIT(SysTick->CTRL,SysTick_CTRL_TICKINT_Msk);
}

/**
 * @brief    : Resume the systick.
 * @param    : None
 * @retval   : None
 */
void Resume_Systicks(void)
{
    SET_BIT(SysTick->CTRL,SysTick_CTRL_TICKINT_Msk);
}
/**
 * @brief    : Systick interrupt service function.
 * @param    : None
 * @retval   : None
 */
void SysTick_Handler(void)
{
    Clear_Over_Flag();
    Inc_Systicks();
    /* todo*/
}

/**
  * @}
*/

/**
  * @}
*/
