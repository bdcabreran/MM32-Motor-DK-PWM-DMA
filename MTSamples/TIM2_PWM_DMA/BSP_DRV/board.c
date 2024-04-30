/**
 * @file     board.c
 * @author   Motor TEAM
 * @brief    This file provides all the functions for the board level support package.
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
#define _BSP_C_

/** Files includes */
#include "board.h"

/**
 * @addtogroup MM32_Hardware_Driver_Layer
 * @{
 */

/**
 * @addtogroup Bsp
 * @{
 */

/**
 * @brief    : This function describes how to initialize I/O configurations related to the LED.
 * @param    : None
 * @retval   : None
 */
void Board_Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
    RCC_AHBPeriphClockCmd(LED_RCC_CLOCKGPIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin     =  LED1_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(LED1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     =  LED2_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(LED2_PORT, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin     =  LED3_PIN;
    // GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    // GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    // GPIO_Init(LED3_PORT, &GPIO_InitStructure);



}
/**
 * @brief    : This function describes how to initialize I/O configurations related to the UART.
 * @param    : None
 * @retval   : None
 */
void Board_Uart_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHBPeriphClockCmd(UART_RCC_CLOCKGPIO, ENABLE);
    
    /** UART_TXInit */
    GPIO_InitStructure.GPIO_Pin = TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(TX_PORT, &GPIO_InitStructure);
    /** UART_RXInit */
    GPIO_InitStructure.GPIO_Pin = RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(RX_PORT, &GPIO_InitStructure);
}

/**
 * @brief    : This function describes how to initialize I/O configurations related to the UART AF.
 * @param    : None
 * @retval   : None
 */
void Board_Uart_Af_Init(void)
{
    RCC_AHBPeriphClockCmd(UART_RCC_CLOCKGPIO, ENABLE);
    
    GPIO_PinAFConfig(TX_PORT,TX_SOURCE, TX_AF);
    GPIO_PinAFConfig(RX_PORT,RX_SOURCE, RX_AF);
}
/**
 * @brief    : This function is configure all the I/O about this board.
 * @param    : None
 * @retval   : None
 */
void Board_Gpio_Init(void)
{
    Board_Led_Init();
    Board_Uart_Init();
}

/**
 * @brief    : This function is configure all the I/O AF about this board.
 * @param    : None
 * @retval   : None
 */
void Board_Gpio_Af_Init(void)
{
    Board_Uart_Af_Init();
}
/**
  * @}
*/

/**
  * @}
*/
