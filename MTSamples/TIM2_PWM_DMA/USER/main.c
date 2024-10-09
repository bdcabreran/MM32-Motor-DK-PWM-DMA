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
#include "bsp_led_pwm.h"
#include "p_config.h"
#include "p_tasks.h"
#include "mci_mock.h"

volatile bool SleepTriggered = false;

// Enable if you want to test LED transitions
#define LED_TRANSITION_TEST (0)


#define DEBUG_BUFFER_SIZE 256
char debugBuffer[DEBUG_BUFFER_SIZE];

#define USE_PWM_MODE_2 0
#define ENABLE_DBG_MSG_MAIN 1

#if ENABLE_DBG_MSG_MAIN
#define DBG_MSG_MAIN(fmt, ...) do { \
  snprintf(debugBuffer, DEBUG_BUFFER_SIZE, (fmt), ##__VA_ARGS__); \
  Uart_Put_Buff(debugBuffer, strlen(debugBuffer)); \
} while(0)
#else
#define DBG_MSG_MAIN(fmt, ...) do { } while(0)
#endif

void blink_led(void);

/** Extern Variables*/
extern uint32_t SystemCoreClock;

typedef enum 
{
  CHARGING_STATE_IDLE,
  CHARGING_STATE_CHARGING,
  CHARGING_STATE_CHARGED,
  CHARGING_STATE_DISCHARGING,
  CHARGING_STATE_ERROR
} charging_state_t;

void charging_state_machine(void);

#if USE_NEW_LED_LIBRARY
void LED_Transitions_Test(void)
{
  LED_Animation_Init(&MainLED, &LED_Controller, LED_Complete_Callback);
  LED_Transition_Init(&LEDTransition, &MainLED);

  LED_Transition_SetMapping(&LEDTransition, LEDTransitionMap, LED_TRANSITION_MAP_SIZE);

  DBG_MSG_MAIN("LED Transition Test\r\n");
}

void LED_Transition_Test_Execute (void)
{
  static uint8_t anim_index = 0;
  static uint32_t last_tick = 0;

  if (Get_Systick_Cnt() - last_tick > 5000)
  {
    last_tick = Get_Systick_Cnt();

    DBG_MSG_MAIN("Transition %d\r\n", anim_index);
    switch (anim_index)
    {
    case 0: 
      LED_Transition_ToSolid(&LEDTransition, &LED_Solid_DefaultColor, LED_TRANSITION_INTERPOLATE, 300);
      break;
    case 1:
      LED_Transition_ToFlash(&LEDTransition, &LED_Flash_Error, LED_TRANSITION_INTERPOLATE, 300);
      break;
    case 2:
      LED_Transition_ToPulse(&LEDTransition, &LED_Pulse_BatteryCharge_LowAndVeryLow, LED_TRANSITION_INTERPOLATE, 300);
      break;
    case 3:
      LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryChargeFinished, LED_TRANSITION_INTERPOLATE, 300);
			break;
		case 4:
      LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, 300);
      break;
    
    default:
      break;
    }

    anim_index = (anim_index + 1) % 5;
  }

  LED_Transition_Update(&LEDTransition,  Get_Systick_Cnt());
}
#endif 

// Constants for debounce time and press durations (in milliseconds)
#define DEBOUNCE_TIME_MS 50
#define SHORT_PRESS_TIME_MS 350
#define LONG_PRESS_TIME_MS 800

// Variables to track button press state
static uint32_t button_press_start_time = 0;
static uint32_t last_button_check_time = 0;
static bool button_was_pressed = false;

// Function prototypes for short and long press callbacks
void on_short_press();
void on_long_press();

int is_power_btn_pressed_debounce(void)
{
    uint32_t current_time = Get_Systick_Cnt();

    // Check if sufficient time has passed since the last button state check
    if (current_time - last_button_check_time < DEBOUNCE_TIME_MS)
    {
        return 0;  // Debouncing, don't process button press yet
    }

    last_button_check_time = current_time;

    int button_pressed = is_power_btn_pressed();  // Read the actual button state

    if (button_pressed && !button_was_pressed)
    {
        // Button has just been pressed
        button_press_start_time = current_time;
        button_was_pressed = true;
    }
    else if (!button_pressed && button_was_pressed)
    {
        // Button has just been released
        uint32_t press_duration = current_time - button_press_start_time;
        button_was_pressed = false;

        if (press_duration >= LONG_PRESS_TIME_MS)
        {
            on_long_press();  // Long press detected
        }
        else if (press_duration >= SHORT_PRESS_TIME_MS)
        {
            on_short_press();  // Short press detected
        }
    }

    return button_pressed;
}

void on_short_press()
{
    DBG_MSG_MAIN("Short press detected.\r\n");
    // Add your short press handling logic here
    PD_BtnPwr_OnShortButtonPress();  // Example callback function
}

void on_long_press()
{
    DBG_MSG_MAIN("Long press detected.\r\n");
    // Add your long press handling logic here
    PD_BtnPwr_OnLongButtonPress();  // Example callback function
}


void board_power_btn_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
    RCC_AHBPeriphClockCmd(LED_RCC_CLOCKGPIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin     =  POWER_BUTTON_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;
    GPIO_Init(POWER_BUTTON_PORT, &GPIO_InitStructure);

}

int is_power_btn_pressed()
{
    return !GPIO_ReadInputDataBit(POWER_BUTTON_PORT, POWER_BUTTON_PIN);
}

int main(void)
{
    Systick_Init(SystemCoreClock / 1000);
    Board_Gpio_Af_Init();
    Drv_Uart_Init(115200);
    Board_Gpio_Init();
    board_power_btn_init();


    DBG_MSG_MAIN("MCU Frequency: %d Hz\r\n", SystemCoreClock);		

    LED_RGYW_TIM_Init();


    #if LED_TRANSITION_TEST
    LED_Transitions_Test();
    #else 
    PD_Initialise();
    #endif 
    MCI_SetMockBatteryDetected(true);
    MCI_SetMockBatteryPercentageInitialised(true);

    Mci.BatteryDetectedReceived = true;

		uint32_t last_tick = Get_Systick_Cnt(); 
		
    while(1)
    {

      #if LED_TRANSITION_TEST
      LED_Transition_Test_Execute();
      #else
      if (Get_Systick_Cnt() != last_tick)
      {
        PD_RunProductControlTasks();
        last_tick = Get_Systick_Cnt();
      }
      charging_state_machine();
      #endif 

      // is_power_btn_pressed_debounce();

      blink_led();
    }
}


void blink_led(void)
{
  static uint32_t last_tick = 0;
  static uint8_t led_state = Bit_RESET;
  if (Get_Systick_Cnt() - last_tick > 1000)
  {
    GPIO_WriteBit(LED2_PORT, LED2_PIN, led_state);
    // GPIO_WriteBit(LED_RGYW_WHITE_PORT, LED_RGYW_WHITE_PIN, led_state);
  
    led_state = (led_state == Bit_SET) ? Bit_RESET : Bit_SET;
    
    // if (led_state == Bit_SET)
    // {
    //   DBG_MSG_MAIN("LED ON\r\n");
    //   LED_RGYW_WHITE_ON;
    // }
    // else
    // {
    //   DBG_MSG_MAIN("LED OFF\r\n");
    //   LED_RGYW_WHITE_OFF;
    // }
    last_tick = Get_Systick_Cnt();
  }
}

static charging_state_t charging_state = CHARGING_STATE_IDLE;
uint16_t battery = 0;

uint32_t period_battery_change = 500;

void charging_state_machine(void)
{
    static uint32_t last_tick = 0;
    static uint32_t counter = 0;
    

    if (Get_Systick_Cnt() - last_tick > period_battery_change)
    {
        last_tick = Get_Systick_Cnt();

        switch (charging_state)
        {
            case CHARGING_STATE_IDLE:
                // MCI_SetMockCablePlugged(false);

                counter++;
                if (counter > 10)
                {
                    counter = 0;
                    charging_state = CHARGING_STATE_CHARGING;
                    period_battery_change = 200;
                    DBG_MSG_MAIN("Charging Started, Cable Plugged\r\n");
                    DBG_MSG_MAIN("Long Press Btn PWR\r\n");
                }
                break;

            case CHARGING_STATE_CHARGING:
                MCI_SetMockCablePlugged(true);

                if (battery < 100)
                {
                    battery++;
                }

                if (battery == 100)
                {
                    DBG_MSG_MAIN("Charging Completed\r\n");
                    counter++;
                    if (counter > 10)
                    {
                        counter = 0;
                        charging_state = CHARGING_STATE_DISCHARGING;
                        period_battery_change = 1000;
                        DBG_MSG_MAIN("Discharging Started, Cable Unplugged\r\n");
                    }
                }
                else
                {
                    DBG_MSG_MAIN("Battery: %d%%\r\n", battery);
                    MCI_SetMockBatteryPercentage(battery); 
                }
                break;

            case CHARGING_STATE_DISCHARGING:
                MCI_SetMockCablePlugged(false);

                if (battery > 30)
                {
                    battery--;
 
                    // if (battery == 60)
                    // {
                    //     DBG_MSG_MAIN("Btn PWR Long Press\r\n");
                    //     PD_BtnPwr_OnLongButtonPress();
                    // }

                    // if (battery == 90 ||battery == 83 || battery == 65 || battery == 50 ||battery == 40 || battery == 33 ||battery == 20 || battery == 9)
                    // {
                    //     DBG_MSG_MAIN("Btn PWR Short Press\r\n");
                    //     PD_BtnPwr_OnShortButtonPress();
                    // }

                    // if (battery == 20)
                    // {
                    //     DBG_MSG_MAIN("Btn PWR Long Press\r\n");
                    //     PD_BtnPwr_OnLongButtonPress();
                    // } 
                }

                if (battery == 0)
                {
                    charging_state = CHARGING_STATE_IDLE;
                    period_battery_change = 200;
                    DBG_MSG_MAIN("Battery Empty\r\n");
                }
                else
                {
                    DBG_MSG_MAIN("Battery: %d%%\r\n", battery);
                    MCI_SetMockBatteryPercentage(battery); 
                }
                break;

            default:
                charging_state = CHARGING_STATE_ERROR;
                DBG_MSG_MAIN("Error in Charging State\r\n");
                break;
        }
    }
}

