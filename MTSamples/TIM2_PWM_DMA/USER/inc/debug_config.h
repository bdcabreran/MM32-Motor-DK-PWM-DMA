#ifndef __DEBUG_CONFIG_H__
#define __DEBUG_CONFIG_H__

// #define PWM_TEST
// #define ALIGN_TEST

/* Note: commenting this out will add delay to program execution. */
#define DISABLE_FIRST_BOOT_LED

/* Customer wants to just remain at 0% instead of
    automatically turning off, so we no longer enter this state.*/
// #define BATT_EMPTY_AUTO_OFF_ENABLE

// #define ENABLE_POWER_DISPLAY

// #define TEST_UART_SEND

/* Reduces likelihood of issue where motor side mixes up the responses (returning 0 for battery SOC)

    Note: Setting this to 100, 200 or 500 makes it very easy to see the issue. */
// #define LIMIT_CMD_SEND_DELAY (200U)

// #define DISPLAY_ERROR_CODES_TEST

// #define USE_OLD_LED_PINOUT
// #define TEST_MODAC_COMMANDS

// #define SEND_ONLY_STOP_COMMAND_NO_SOUND

// #define USE_RX2_TX2
#define DISABLE_DC_WAKE /* For debug */

// #define DISABLE_BATTERY_EMPTY

/* LED was still on when PWM disabled, didn't figure out why yet 
   so just changing to GPIO when off for simplicity. */
#define LED_TIMER_GPIO_WHEN_OFF

#define DISABLE_SLEEP_MODE

// #define DISABLE_MODAC

/* Note: if watchdog is enabled, it needs an alarm as the clock
   can't be turned off. We'll have to decide whether we can risk no
   watchdog or see if FAE has a better solution to the alarm. */
// #define DISABLE_SLEEP_ALARM
// #define DISABLE_WATCH_DOG

// #define ENABLE_DEBUG_MODE
// #define DISABLE_SLEEP_MODE
// #define DISABLE_COMM_ERROR

// #define DISABLE_ON_OFF_COMMANDS
// #define SEGMENT_DISPLAY_TEST
// #define SEGMENT_DISPLAY_COUNT_MS (500)

// #define DISPLAY_99_WHEN_WAKING_UP

// #define ENABLE_TURN_ON_OFF_ANIMATION

// #define DEBUG_IO_ENABLE
#ifdef DEBUG_IO_ENABLE

#define DEBUG_IO_PORT GPIOB
#define DEBUG_IO_PIN GPIO_Pin_10

#define DEBUG_IO_SetHigh DEBUG_IO_PORT->BSRR = DEBUG_IO_PIN;
#define DEBUG_IO_SetLow DEBUG_IO_PORT->BRR = DEBUG_IO_PIN;

#define DEBUG_IO_ADC_INTERUPPT (1)
#define DEBUG_IO_BEMF_DETECT (2)
#define DEBUG_IO_BEMF_MISSED (3)
#define DEBUG_IO_TIM2_IRQ (4)
#define DEBUG_IO_TIM3_IRQ (5)
#define DEBUG_IO_SYSTICK_IRQ (6)
#define DEBUG_IO_FEED_DOG (7)
#define DEBUG_IO_BEMF_STATE (8)
#define DEBUG_IO_BEMF_RECOVERED (9)
#define DEBUG_IO_BEMF_SEARCH (10)
#define DEBUG_IO_COMMUTATION_INT (11)
#define DEBUG_IO_SIG (DEBUG_IO_BEMF_DETECT)

#endif

// #define DEBUG_WITH_BR01_HARDWARE

// #define ALIGN_TEST

// #define DISABLE_PRESCALER_CHANGES

// #define DISABLE_GAIN_SHEDULER

// #define DISABLE_BEMF_THRESHOLDS
// #define DISABLE_BEMF_SENSOR_STALL

// #define STAY_IN_START_STATE
// #define RUN_STARTUP_TEST

// #define VOLTAGE_LIMIT (10000)

// #define DISABLE_TEMPERATURE_FAULT
// #define DISABLE_BATTERY_EMPTY_FAULT
// #define DISABLE_BATTERY_SUBSYSTEM

#endif
