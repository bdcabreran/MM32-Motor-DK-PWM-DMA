#include "main.h"
#include "bsp_led_pwm.h"
#include "button.h"
#include "p_tasks.h"
#include "p_config.h"

Button_Handle_t BtnPwr = {
    .FilterTicks = 25,
    .LongPressTicks = 1500,
};

Button_Handle_t BtnSpeed1 = {
    .FilterTicks = 25,
    .LongPressTicks = 1500,
};

Button_Handle_t BtnSpeed2 = {
    .FilterTicks = 25,
    .LongPressTicks = 1500,
};

Button_Handle_t BtnSpeed3 = {
    .FilterTicks = 25,
    .LongPressTicks = 1500,
};

Button_Handle_t BtnTurbo = {
    .FilterTicks = 25,
    .LongPressTicks = 1500,
};

// DETSW_Handle_t DCInDetectSwitch = {
//     .ToActiveDelayTicks = DC_IN_DETECT_TO_ACTIVE_TICKS,
//     .ToInactiveDelayTicks = DC_IN_DETECT_TO_INACTIVE_TICKS,
// };

// BZR_Handle_t Buzzer;

Product_Handle_t Product;

// RGYW LEDs Configurations
LED_Handle_t MainLED;
LED_Transition_Handle_t LEDTransition;

// PWM Configuration for RGYW LEDs
const PWM_RGY_t PWM_RGY = {
    .Red = LED_SetPWM_Red,
    .Green = LED_SetPWM_Green,
    .Yellow = LED_SetPWM_Yellow, 
};

// LED Controller Configuration
const LED_Controller_t LED_Controller = {
    .LedType = LED_TYPE_RGY,
    .PwmConfig = &PWM_RGY,
    .Start = LED_PWM_Start,
    .Stop = LED_PWM_Stop,
    .MaxDutyCycle = LED_RGYW_PWM_MAX_PULSE,   
};

// LED Colors
const RGY_Color_t YellowColor = { .R = 0, .G = 0, .Y = 255};
const RGY_Color_t GreenColor = { .R = 0, .G = 128, .Y = 0};
const RGY_Color_t RedColor = { .R = 255, .G = 0, .Y = 0};


// TODO: this is supposed to be white in future
// If The White LED is handled without PWM, then we need to gently transition to OFF state and then turn on the white LED
RGY_Color_t DefaultColor = { .R = 0, .G = 40, .Y = 128};


// LED Animations
const LED_Animation_Solid_t LED_Solid_BatteryChargeFinished = {
    .color = &GreenColor,
    .executionTimeMs = 0,
};

const LED_Animation_Solid_t LED_Solid_BatteryLow = {
    .color = &RedColor,
    .executionTimeMs = 0,
};

const LED_Animation_Solid_t LED_Solid_BatteryMid = {
    .color = &YellowColor,
    .executionTimeMs = 0,
};

const LED_Animation_Solid_t LED_Solid_BatteryHigh = {
    .color = &GreenColor,
    .executionTimeMs = 0,
};

const LED_Animation_Solid_t LED_Solid_DefaultColor = {
    .color = &DefaultColor,
    .executionTimeMs = 0,
};

const LED_Animation_Flash_t LED_Flash_BatteryVeryLow = {
    .color = &RedColor,
    .onTimeMs = LED_RGYW_BATT_VERYLOW_FLASH_ON_TIME,
    .offTimeMs = LED_RGYW_BATT_VERYLOW_FLASH_OFF_TIME,
    .repeatTimes = -1,
};

const LED_Animation_Flash_t LED_Flash_Error = {
    .color = &RedColor,
    .onTimeMs = LED_RGYW_ERROR_FLASH_ON_TIME,
    .offTimeMs = LED_RGYW_ERROR_FLASH_OFF_TIME,
    .repeatTimes = LED_RGYW_ERROR_FLASH_TIMES,
};

const LED_Animation_Pulse_t LED_Pulse_BatteryCharge_LowAndVeryLow = {
    .color = &RedColor,
    .riseTimeMs = LED_RGYW_PULSE_RISE_TIME, 
    .fallTimeMs = LED_RGYW_PULSE_FALL_TIME,
    .holdOnTimeMs = LED_RGYW_PULSE_HOLD_ON_TIME,
    .holdOffTimeMs = LED_RGYW_PULSE_HOLD_OFF_TIME,
    .repeatTimes = -1,
};

const LED_Animation_Pulse_t LED_Pulse_BatteryCharge_Mid = {
    .color = &YellowColor,
    .riseTimeMs = LED_RGYW_PULSE_RISE_TIME, 
    .fallTimeMs = LED_RGYW_PULSE_FALL_TIME,
    .holdOnTimeMs = LED_RGYW_PULSE_HOLD_ON_TIME,
    .holdOffTimeMs = LED_RGYW_PULSE_HOLD_OFF_TIME,
    .repeatTimes = -1,
};

const LED_Animation_Pulse_t LED_Pulse_BatteryCharge_High = {
    .color = &GreenColor,
    .riseTimeMs = LED_RGYW_PULSE_RISE_TIME, 
    .fallTimeMs = LED_RGYW_PULSE_FALL_TIME,
    .holdOnTimeMs = LED_RGYW_PULSE_HOLD_ON_TIME,
    .holdOffTimeMs = LED_RGYW_PULSE_HOLD_OFF_TIME,
    .repeatTimes = -1,
};

// LED Predefined Transitions for Battery Charge Level, e.g Low -> Mid -> High -> Finished after a clean entry (leds are off)
const LED_Transition_Config_t LEDTransitionMap[LED_TRANSITION_MAP_SIZE] = {
    {&LED_Pulse_BatteryCharge_LowAndVeryLow,  &LED_Pulse_BatteryCharge_Mid,     LED_TRANSITION_AT_CLEAN_ENTRY, LED_RYGW_PULSE_PERIOD },
    {&LED_Pulse_BatteryCharge_Mid,            &LED_Pulse_BatteryCharge_High,    LED_TRANSITION_AT_CLEAN_ENTRY, LED_RYGW_PULSE_PERIOD },
    {&LED_Pulse_BatteryCharge_High,           &LED_Solid_BatteryChargeFinished, LED_TRANSITION_AT_CLEAN_ENTRY, LED_RYGW_PULSE_PERIOD }, 
};

