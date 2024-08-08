/**
 * @file bsp_led_pwm.c
 * @author Bayron Cabrera (bayron.nanez@gmail.com)
 * @brief LED PWM BSP
 * @version 0.1
 * @date 2024-07-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "bsp_led_pwm.h"

// #define LED_TIMER_GPIO_WHEN_OFF

// Function prototypes
void LED_Config_TIM(void);
void LED_Config_GPIO(void);
void LED_Config_OC(TIM_OCInitTypeDef* TIM_OCInitStructure);
void LED_PWM_Stop(void);

void LED_RGYW_TIM_Init(void)
{
    LED_Config_TIM();
    LED_Config_GPIO();

    // Start in off by Default
    LED_PWM_Stop();
}

void LED_Config_TIM(void)
{
    TIM_TimeBaseInitTypeDef TIM_StructInit;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    TIM_TimeBaseStructInit(&TIM_StructInit);
    
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM2EN, ENABLE);

    TIM_StructInit.TIM_Prescaler = LED_RGYW_PWM_PRESCALER;
    TIM_StructInit.TIM_Period = LED_RGYW_PWM_PERIOD;
    TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_StructInit.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_StructInit.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_StructInit);

    LED_Config_OC(&TIM_OCInitStructure);
}

void LED_Config_OC(TIM_OCInitTypeDef* TIM_OCInitStructure)
{
    TIM_OCStructInit(TIM_OCInitStructure);
    
#if (LED_RGYW_COMMON_CONFIG == LED_RGYW_COMMON_CATHODE)
    TIM_OCInitStructure->TIM_OCMode = TIM_OCMode_PWM1;
#else
    TIM_OCInitStructure->TIM_OCMode = TIM_OCMode_PWM2;
#endif
    TIM_OCInitStructure->TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure->TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure->TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure->TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure->TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure->TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OCInitStructure->TIM_Pulse = 0;

    // Initialize TIM OC channels
    TIM_OC1Init(TIM2, TIM_OCInitStructure); // Red
    TIM_OC2Init(TIM2, TIM_OCInitStructure); // Green
    TIM_OC3Init(TIM2, TIM_OCInitStructure); // Yellow
}

void LED_Config_GPIO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Configure Simple GPIO for White LED
    GPIO_InitStructure.GPIO_Pin = LED_RGYW_WHITE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(LED_RGYW_WHITE_PORT, &GPIO_InitStructure);

    // Configure Alternate Function for PWM
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    RCC_GPIO_ClockCmd(LED_RGYW_RED_PORT, ENABLE);
    GPIO_PinAFConfig(LED_RGYW_RED_PORT, LED_RGYW_RED_PIN_SRC, LED_RGYW_RED_PIN_AF);
    GPIO_InitStructure.GPIO_Pin = LED_RGYW_RED_PIN;
    GPIO_Init(LED_RGYW_RED_PORT, &GPIO_InitStructure);

    RCC_GPIO_ClockCmd(LED_RGYW_GREEN_PORT, ENABLE);
    GPIO_PinAFConfig(LED_RGYW_GREEN_PORT, LED_RGYW_GREEN_PIN_SRC, LED_RGYW_GREEN_PIN_AF);
    GPIO_InitStructure.GPIO_Pin = LED_RGYW_GREEN_PIN;
    GPIO_Init(LED_RGYW_GREEN_PORT, &GPIO_InitStructure);

    RCC_GPIO_ClockCmd(LED_RGYW_YELLOW_PORT, ENABLE);
    GPIO_PinAFConfig(LED_RGYW_YELLOW_PORT, LED_RGYW_YELLOW_PIN_SRC, LED_RGYW_YELLOW_PIN_AF);
    GPIO_InitStructure.GPIO_Pin = LED_RGYW_YELLOW_PIN;
    GPIO_Init(LED_RGYW_YELLOW_PORT, &GPIO_InitStructure);

}


// Function to set the PWM duty cycle for Red LED
void LED_SetPWM_Red(uint16_t DutyCycle)
{
    DutyCycle = (DutyCycle > LED_RGYW_PWM_PERIOD) ? LED_RGYW_PWM_PERIOD : DutyCycle;
    TIM_SetCompare1(TIM2, DutyCycle);  
}

// Function to set the PWM duty cycle for Green LED
void LED_SetPWM_Green(uint16_t DutyCycle)
{
    DutyCycle = (DutyCycle > LED_RGYW_PWM_PERIOD) ? LED_RGYW_PWM_PERIOD : DutyCycle;
    TIM_SetCompare2(TIM2, DutyCycle);  
}

// Function to set the PWM duty cycle for Yellow LED
void LED_SetPWM_Yellow(uint16_t DutyCycle)
{
    DutyCycle = (DutyCycle > LED_RGYW_PWM_PERIOD) ? LED_RGYW_PWM_PERIOD : DutyCycle;
    TIM_SetCompare3(TIM2, DutyCycle);  
}

// Function to start the PWM
void LED_PWM_Start(void)
{
    // Enable the timer
    TIM_Cmd(TIM2, ENABLE);

    // Enable PWM output for each channel
    TIM_CCxCmd(TIM2, TIM_Channel_1, TIM_CCx_Enable); // Green
    TIM_CCxCmd(TIM2, TIM_Channel_2, TIM_CCx_Enable); // Yellow
    TIM_CCxCmd(TIM2, TIM_Channel_3, TIM_CCx_Enable); // Red

    // Optionally, enable main output if needed
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
}

// Function to stop the PWM
void LED_PWM_Stop(void)
{
    // Set PWM duty cycle to maximum to ensure the outputs are HIGH
    LED_SetPWM_Red(0);    // Assuming setting to 0 effectively turns LEDs off
    LED_SetPWM_Green(0);
    LED_SetPWM_Yellow(0);

    // Disable the timer
    TIM_Cmd(TIM2, DISABLE);

    // Optionally, disable main output if it was enabled
    TIM_CtrlPWMOutputs(TIM2, DISABLE);
}
