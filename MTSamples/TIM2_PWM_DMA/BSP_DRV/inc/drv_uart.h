/** 
 * @file     drv_uart.h
 * @author   Motor TEAM
 * @brief    This file provides all the driver functions prototypes for the UART.
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
#ifndef __DRV_UART_H
#define __DRV_UART_H

/** Files includes */
#include "HAL_device.h"
#include "HAL_conf.h"

/** 
 * @addtogroup MM32_Hardware_Driver_Layer
 * @{
 */

/** 
 * @addtogroup Drv_UART
 * @{
 */

extern void Drv_Uart_Init(uint32_t u32BaudRrate);
extern void Uart_Put_Char(int8_t u8Char);
extern void Uart_Put_Buff(int8_t *pBuff, uint32_t u32Len);

/**
  * @}
*/

/**
  * @}
*/

#endif
