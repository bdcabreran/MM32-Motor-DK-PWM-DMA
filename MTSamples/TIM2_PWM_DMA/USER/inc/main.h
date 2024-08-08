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


/** 
 * @addtogroup MM32_User_Layer
 * @{
 */

/** 
 * @addtogroup User_Main
 * @{
 */


extern uint8_t u8Tim1UpdateFlag;
/**  defind in startup_mm32spin028x.c  */
extern uint32_t SystemCoreClock;

/**
  * @}
*/

/**
  * @}
*/

#endif
