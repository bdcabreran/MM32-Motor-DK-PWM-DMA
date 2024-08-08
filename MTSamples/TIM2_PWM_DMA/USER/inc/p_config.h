#ifndef __P_CONFIG_H__
#define __P_CONFIG_H__

#include "p_type.h"
#include "p_tasks.h"
#include "button.h"
// #include "led_rgb.h"
#include "led_transition_manager.h"
#include "led_animation.h"


#define LED_RGYW_ERROR_FLASH_TIMES (5U)  /* Number of times to flash LED for error. */
#define LED_RGYW_ERROR_FLASH_ON_TIME (200U)  /* (ms) On-time for LED flash sequence. */
#define LED_RGYW_ERROR_FLASH_OFF_TIME (150U) /* (ms) Off-time for LED flash sequence. */

#define LED_RGYW_BATT_VERYLOW_FLASH_ON_TIME (1000U) /* (ms) */
#define LED_RGYW_BATT_VERYLOW_FLASH_OFF_TIME (1000U) /* (ms) */

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

extern Button_Handle_t BtnPwr;
extern Button_Handle_t BtnSpeed1;
extern Button_Handle_t BtnSpeed2;
extern Button_Handle_t BtnSpeed3;
extern Button_Handle_t BtnTurbo;

// extern DETSW_Handle_t DCInDetectSwitch;

// extern BZR_Handle_t Buzzer;

extern Product_Handle_t Product;

// LED Animation Configuration 
extern LED_Handle_t MainLED;
extern const LED_Controller_t LED_Controller;
extern const RGY_Color_t YellowColor;
extern const RGY_Color_t GreenColor;
extern const RGY_Color_t RedColor;

// LED Animations Configuration
extern const LED_Animation_Solid_t LED_Solid_BatteryChargeFinished;
extern const LED_Animation_Solid_t LED_Solid_BatteryLow;
extern const LED_Animation_Solid_t LED_Solid_BatteryMid;
extern const LED_Animation_Solid_t LED_Solid_BatteryHigh;
extern const LED_Animation_Solid_t LED_Solid_DefaultColor;
extern const LED_Animation_Flash_t LED_Flash_BatteryVeryLow;
extern const LED_Animation_Flash_t LED_Flash_Error;
extern const LED_Animation_Pulse_t LED_Pulse_BatteryCharge_LowAndVeryLow;
extern const LED_Animation_Pulse_t LED_Pulse_BatteryCharge_Mid;
extern const LED_Animation_Pulse_t LED_Pulse_BatteryCharge_High;

// LED Transitions Configuration
extern LED_Transition_Handle_t LEDTransition;
#define LED_TRANSITION_MAP_SIZE 3
extern const LED_Transition_Config_t LEDTransitionMap[LED_TRANSITION_MAP_SIZE];


#endif
