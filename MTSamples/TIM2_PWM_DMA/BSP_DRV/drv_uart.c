/** 
 * @file     drv_uart.c
 * @author   Motor TEAM
 * @brief    This file provides all the driver functions for the UART.
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
#define _DRV_UART_C_

/** Files includes */
#include "drv_uart.h"

/**
 * @addtogroup MM32_Hardware_Driver_Layer
 * @{
 */

/**
 * @addtogroup Drv_UART
 * @{
 */

/**
 * @brief  This function will initialize the UART.
 * @param  u32BaudRrate: Configuring UART baud rate.
 * @retval : None
 */
void Drv_Uart_Init(uint32_t u32BaudRrate)
{
    UART_InitTypeDef UART_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_UART1, ENABLE);
    
    UART_StructInit(&UART_InitStructure);
    /**
     * Baud rate set to u32BaudRrate
     * 8-bit mode
     * 1 stop bit
     * No parity
     * No hardware flow control
     * Enable receive and send
     */
    UART_InitStructure.UART_BaudRate = u32BaudRrate;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;	
    UART_Init(UART1, &UART_InitStructure);
 
    UART_Cmd(UART1, ENABLE);
}

/**
* @brief  This function will send a character.
* @param u8Char: This character will be sent.
* @retval : None
*/
void Uart_Put_Char(int8_t u8Char)
{
    while(UART_GetFlagStatus(UART1, UART_IT_TXIEN) == RESET);
	UART_SendData(UART1, u8Char);  
}

/**
* @brief  This function will send a string.
* @param pBuff: The string of this address will be sent.
* @param u32Len: The length of the string to be sent.
* @retval : None
*/
void Uart_Put_Buff(int8_t *pBuff, uint32_t u32Len)
{
	while(u32Len--)
	{
		Uart_Put_Char(*pBuff);
		pBuff++;
	}
}
/**
  * @}
*/

/**
  * @}
*/
