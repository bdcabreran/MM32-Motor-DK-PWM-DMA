#ifndef __P_CONFIG_H__
#define __P_CONFIG_H__

#include "p_type.h"
#include "p_tasks.h"
#include "button.h"
#include "detect_switch.h"
#include "buzzer.h"
#include "debug_config.h"

#define USE_NEW_LED_LIBRARY (1)

#if USE_NEW_LED_LIBRARY
#include "led_animation.h"
#include "led_transition_manager.h"
#else 
#include "led_rgb.h"
#endif 


extern Button_Handle_t BtnPwr;
extern Button_Handle_t BtnSpeed1;
extern Button_Handle_t BtnSpeed2;
extern Button_Handle_t BtnSpeed3;
extern Button_Handle_t BtnTurbo;

extern DETSW_Handle_t DCInDetectSwitch;

extern BZR_Handle_t Buzzer;

extern Product_Handle_t Product;

#if USE_NEW_LED_LIBRARY
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
extern const LED_Animation_Flash_t LED_Flash_BatteryEmpty;
extern LED_Animation_Flash_t LED_Flash_Error;
extern const LED_Animation_Pulse_t LED_Pulse_BatteryCharge_LowAndVeryLow;
extern const LED_Animation_Pulse_t LED_Pulse_BatteryCharge_Mid;
extern const LED_Animation_Pulse_t LED_Pulse_BatteryCharge_High;

// LED Transitions Configuration
extern LED_Transition_Handle_t LEDTransition;
#define LED_TRANSITION_MAP_SIZE 3
extern const LED_Transition_Config_t LEDTransitionMap[LED_TRANSITION_MAP_SIZE];
#else 
extern RGBLED_Handle_t MainLED;
extern RGBLED_Solid_Handle_t LEDBatteryChargeFinished;
extern RGBLED_Solid_Handle_t LEDSolid_BatteryLow;
extern RGBLED_Solid_Handle_t LEDSolid_BatteryMid;
extern RGBLED_Solid_Handle_t LEDSolid_BatteryHigh;
extern RGBLED_Solid_Handle_t LEDSolid_DefaultColor;
extern RGBLED_Flash_Handle_t LEDFlash_BatteryVeryLow;
extern RGBLED_Flash_Handle_t LEDErrorFlash;
extern RGBLED_Breath_Handle_t LEDBatteryChargeBreath_LowandVeryLow;
extern RGBLED_Breath_Handle_t LEDBatteryChargeBreath_Mid;
extern RGBLED_Breath_Handle_t LEDBatteryChargeBreath_High;
extern RGBColor_Handle_t YellowColor;
extern RGBColor_Handle_t GreenColor;
extern RGBColor_Handle_t RedColor;
#endif


#endif
