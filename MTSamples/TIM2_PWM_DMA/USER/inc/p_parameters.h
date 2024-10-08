#ifndef __P_PARAMETERS_H__
#define __P_PARAMETERS_H__

#include "config.h"
#include "debug_config.h"

/************************** Timing Parameters **************************/
#define PRODUCT_TASK_FREQUENCY (SYSTICK_FREQUENCY / 1U)

#ifdef DEBUG_SLEEP_MODE
#define SLEEP_TIME (2U) /* (s) Number of seconds to wait in IDLE state before going to deep sleep. */
#else
#define SLEEP_TIME (5U) /* (s) Number of seconds to wait in IDLE state before going to deep sleep. */
#endif

/* Once 0% state has been reached, unit will shutdown automatically 
   after this duration. */
#define BATT_EMPTY_SHUTDOWN_TIME (10U) /* (s) */

#define ON_TIME (60U) /* (m) Number of minutes before product turns off automatically. */

#define BATT_FULL_LIGHT_OFF_DURATION (3U)

#define UART_BAUD_RATE (38400)

#define CABLE_UNPLUGGED_DISPLAY_TIMEOUT (1000U) /* (ms) */

#define ERROR_FLASH_INTERVAL (500U) /* (ms) */
#define ERROR_FLASH_TIMES (3U) /* */
/* Faul ack timeout needs to be timed to when last error flash turns off
   Error flash starts in off state and will do 4x off flashes*/
#define FAULT_ACK_TIMEOUT ((ERROR_FLASH_INTERVAL * 2) * ERROR_FLASH_TIMES) /* (ms) */

#define BATT_LOW_FLASH_INTERVAL (500U) /* (ms) */
#define BATT_LOW_FLASH_TIMES (-1) /* -1 = flash constantly */
#define BATT_LOW_PERCENTAGE (9U) /* */
#define BATT_EMPTY_FLASH_INTERVAL (200U) /* (ms) */
#define BATT_EMPTY_FLASH_TIMES (3U) /* */
#define BATT_EMPTY_WAIT_TIME ((BATT_EMPTY_FLASH_INTERVAL * 2) * BATT_EMPTY_FLASH_TIMES) /* (ms) */

#define BATT_DISPLAY_DURATION (3U)

#define BATT_EMPTY_FLASH_TIMES_DISCHARGE (10U) /* */
#define BATT_EMPTY_WAIT_TIME_DISCHARGE ((BATT_EMPTY_FLASH_INTERVAL * 2) * BATT_EMPTY_FLASH_TIMES_DISCHARGE) /* (ms) */

#define MODE_CHANGE_FLASH_INTERVAL (200U)
#define MODE_CHANGE_FLASH_TIMES (3U)

#define POWER_DISPLAY_DURATION (3000U) /* (ms) */

#define BUZZER_PWM_SCALER (MCU_CLOCK_MHZ) 
#define BUZZER_DURATION_TURN_ON (400U) /* (ms) */
#define BUZZER_DURATION_TURN_OFF (400U) /* (ms) */
#define BUZZER_DURATION_SPEED_CHANGE_ON_TIME (200U) /* (ms) */
#define BUZZER_DURATION_SPEED_CHANGE_TURBO_OFF_TIME (50U) /* (ms) */
#define BUZZER_DUTY (50U) /* (%) */

#define BUZZER_PWM_FREQUENCY_1 (2093) /* Note C7 */
#define BUZZER_PWM_FREQUENCY_2 (2349) /* Note D7 */
#define BUZZER_PWM_FREQUENCY_3 (2637) /* Note E7 */

/* Beeps when pressing turbo button. */
#define BUZZER_TURBO_TIMES (2)
#define BUZZER_DEFAULT_TIMES (1)

#define TIMER_MODE_DURATION (10U) /* minutes */
#define MODE_CHANGE_BEEP_DURATION (400U) /* ms */
#define MODE_CHANGE_BEEP_TIMES (1U)

/************************************************************************/

/************************** LED Parameters **************************/
#define LED_RGYW_SUBSYSTEM_FREQUENCY (PRODUCT_TASK_FREQUENCY) /* (Hz) RGB LED update frequency. */

#define LED_RGYW_PWM_FREQUENCY (1000U)       /* (Hz) PWM frequency for LED control. */

#define LED_RGYW_ERROR_FLASH_TIMES (5U)  /* Number of times to flash LED for error. */
#define LED_RGYW_ERROR_FLASH_ON_TIME (200U)  /* (ms) On-time for LED flash sequence. */
#define LED_RGYW_ERROR_FLASH_OFF_TIME (150U) /* (ms) Off-time for LED flash sequence. */
#define LED_RGYW_ERROR_FLASH_INTERVAL (200U)  /* (ms) On/Off-time for LED flash sequence. */

#define LED_RGYW_BATT_VERYLOW_FLASH_ON_TIME (100U) /* (ms) */
#define LED_RGYW_BATT_VERYLOW_FLASH_OFF_TIME (100U) /* (ms) */

#define LED_RGYW_BREATH_PERIOD (5000U)      /*(ms)*/
#define LED_RGYW_BREATH_HOLDIN_TIME (200U)  /*(ms)*/
#define LED_RGYW_BREATH_HOLDOUT_TIME (100U) /*(ms)*/

#define LED_RGYW_PULSE_RISE_TIME (2300U) /* (ms) */
#define LED_RGYW_PULSE_FALL_TIME (2300U) /* (ms) */
#define LED_RGYW_PULSE_HOLD_ON_TIME (200U) /* (ms) */
#define LED_RGYW_PULSE_HOLD_OFF_TIME (100U) /* (ms) */
#define LED_RYGW_PULSE_PERIOD (LED_RGYW_PULSE_RISE_TIME + LED_RGYW_PULSE_HOLD_ON_TIME + + LED_RGYW_PULSE_FALL_TIME + LED_RGYW_PULSE_HOLD_OFF_TIME) /* (ms) */

#define SPEED_LED_FLASH_DURATION_ON (750U) /* (ms) */
#define SPEED_LED_FLASH_DURATION_OFF (250U) /* (ms) */

/************************************************************************/

/************************** Button Parameters **************************/
#define BUTTON_FILTER_TIME (25U)        /* (ms) Debounce time for buttons. */
#define SINGLE_BUTTON_PRESS_TIMEOUT (350U) /* (ms) Wait for single press before 
                                              showing power on/off animation*/
#define BUTTON_DOUBLE_PRESS_TIME (100U)
#ifndef ENABLE_DEBUG_MODE
#define BUTTON_LONG_PRESS_TIME (1500U)  /* (ms) Button long press. */
#else
#define BUTTON_LONG_PRESS_TIME (750U)  /* (ms) Button long press. */
#endif

#define DC_IN_DETECT_TO_ACTIVE_TIME (300U)
#define DC_IN_DETECT_TO_INACTIVE_TIME (300U)

#define MODE_DETECT_TO_ACTIVE_TIME (50U)
#define MODE_DETECT_TO_INACTIVE_TIME (MODE_DETECT_TO_ACTIVE_TIME)

/************************************************************************/

/*********************** Battery Level parameters ***********************/

/* Battery Percentage Levels. */
#define BATT_LEVEL1_THRESHOLD (10U) /* (%) */
#define BATT_LEVEL2_THRESHOLD (25U) /* (%) */
#define BATT_LEVEL3_THRESHOLD (50U) /* (%) */
#define BATT_LEVEL4_THRESHOLD (99U) /* (%) */

/***********************************************************************/

#endif
