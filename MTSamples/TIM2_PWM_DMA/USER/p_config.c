#include "main.h"
#include "p_config.h"
#include "debug_config.h"
#include "p_parameter_conversion.h"
#include "bsp_led_pwm.h"

Button_Handle_t BtnPwr = {
    .FilterTicks = BUTTON_FILTER_TICKS,
    .LongPressTicks = BUTTON_LONG_PRESS_TICKS,
};

Button_Handle_t BtnSpeed1 = {
    .FilterTicks = BUTTON_FILTER_TICKS,
    .LongPressTicks = BUTTON_LONG_PRESS_TICKS,
};

Button_Handle_t BtnSpeed2 = {
    .FilterTicks = BUTTON_FILTER_TICKS,
    .LongPressTicks = BUTTON_LONG_PRESS_TICKS,
};

Button_Handle_t BtnSpeed3 = {
    .FilterTicks = BUTTON_FILTER_TICKS,
    .LongPressTicks = BUTTON_LONG_PRESS_TICKS,
};

Button_Handle_t BtnTurbo = {
    .FilterTicks = BUTTON_FILTER_TICKS,
    .LongPressTicks = BUTTON_LONG_PRESS_TICKS,
};

DETSW_Handle_t DCInDetectSwitch = {
    .ToActiveDelayTicks = DC_IN_DETECT_TO_ACTIVE_TICKS,
    .ToInactiveDelayTicks = DC_IN_DETECT_TO_INACTIVE_TICKS,
};

BZR_Handle_t Buzzer;

Product_Handle_t Product;

#define USING_RGB_LED (1)


#if USE_NEW_LED_LIBRARY

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
#if USING_RGB_LED
const RGY_Color_t YellowColor = { .R = 255, .G = 255, .Y = 0};
#else 
const RGY_Color_t YellowColor = { .R = 0, .G = 0, .Y = 255 };
#endif 

const RGY_Color_t GreenColor = { .R = 0, .G = 128, .Y = 0};
const RGY_Color_t RedColor = { .R = 255, .G = 0, .Y = 0};


// TODO: this is supposed to be white in future
// If The White LED is handled without PWM, then we need to gently transition to OFF state and then turn on the white LED
#if USING_RGB_LED
RGY_Color_t DefaultColor = { .R = 128, .G = 168, .Y = 0 };
#else 
RGY_Color_t DefaultColor = { .R = 0, .G = 40, .Y = 128};
#endif


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

const LED_Animation_Flash_t LED_Flash_BatteryEmpty = {
    .color = &RedColor,
    .onTimeMs = BATT_EMPTY_FLASH_INTERVAL,
    .offTimeMs = BATT_EMPTY_FLASH_INTERVAL,
    .repeatTimes = BATT_EMPTY_FLASH_TIMES,
};

LED_Animation_Flash_t LED_Flash_Error = {
    .color = &RedColor,
    .onTimeMs = LED_RGYW_ERROR_FLASH_INTERVAL,
    .offTimeMs = LED_RGYW_ERROR_FLASH_INTERVAL,
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
#else 

RGBLED_Handle_t MainLED = {
    .TimerInterface = {
        .EnablePWM = &LED_PWM_Start,
        .DisablePWM = &LED_PWM_Stop,
        .CCR_Red = &LED_RGYW_TIMER_CC_RED,
        .CCR_Green = &LED_RGYW_TIMER_CC_GREEN,
        .CCR_Blue = &LED_RGYW_TIMER_CC_YELLOW,
    },
    .MaxPulse = LED_RGYW_PWM_MAX_PULSE,
};

// RGBColor_Handle_t WhiteColor = {.R = 128, .G = 128, .B = 128};
/// TODO: note that we should probably change module to tri-color LED
/// in future as this one is RGY but we obviously want to use the same
/// software module to control it.
RGBColor_Handle_t RedColor = {.R = 255, .G = 0, .B = 0};
RGBColor_Handle_t GreenColor = {.R = 0, .G = 128, .B = 0};

#if USING_RGB_LED
RGBColor_Handle_t YellowColor = { .R = 255, .G = 255, .B = 0 };
#else
RGBColor_Handle_t YellowColor = {.R = 0, .G = 0, .B = 255};
#endif

RGBLED_Solid_Handle_t LEDBatteryChargeFinished = {.Color = &GreenColor};
RGBLED_Solid_Handle_t LEDSolid_BatteryLow = {.Color = &RedColor};
RGBLED_Solid_Handle_t LEDSolid_BatteryMid = {.Color = &YellowColor};
RGBLED_Solid_Handle_t LEDSolid_BatteryHigh = {.Color = &GreenColor};

/// TODO: this is supposed to be white in future
#if USING_RGB_LED
RGBColor_Handle_t DefaultColor = { .R = 128, .G = 168, .B = 0 };
#else 
RGBColor_Handle_t DefaultColor = {.R = 0, .G = 40, .B = 128};
#endif 

RGBLED_Solid_Handle_t LEDSolid_DefaultColor = {.Color = &DefaultColor};

RGBLED_Flash_Handle_t LEDFlash_BatteryVeryLow = {
    .Color = &RedColor,
    .Type = RGBLEDFlashType_ON,
    .Times = -1, /* Negative number = false indefinitely. */
    .OnTicks = LED_RGYW_BATT_VERYLOW_FLASH_ON_TICKS,
    .OffTicks = LED_RGYW_BATT_VERYLOW_FLASH_OFF_TICKS,
};

RGBLED_Flash_Handle_t BattEmptyFlash = {
    .Color = &RedColor,
    .Type = RGBLEDFlashType_ON,
    .Times = BATT_EMPTY_FLASH_TIMES,
    .OnTicks = BATT_EMPTY_FLASH_TICKS,
    .OffTicks = BATT_EMPTY_FLASH_TICKS,
};

RGBLED_Flash_Handle_t LEDErrorFlash = {
    .Color = &RedColor,
    .Type = RGBLEDFlashType_ON,
    .Times = LED_RGYW_ERROR_FLASH_TIMES,
    .OnTicks = LED_RGYW_ERROR_FLASH_INTERVAL_TICKS,
    .OffTicks = LED_RGYW_ERROR_FLASH_INTERVAL_TICKS,
};

RGBLED_Breath_Handle_t LEDBatteryChargeBreath_LowandVeryLow = {
    .Color = &RedColor,
    .PeriodTicks = ms_Conversion(LED_RGYW_BREATH_PERIOD, LED_RGYW_PWM_FREQUENCY),
    .HoldInTicks = ms_Conversion(LED_RGYW_BREATH_HOLDIN_TIME, LED_RGYW_PWM_FREQUENCY),
    .HoldOutTicks = ms_Conversion(LED_RGYW_BREATH_HOLDOUT_TIME, LED_RGYW_PWM_FREQUENCY),
};

RGBLED_Breath_Handle_t LEDBatteryChargeBreath_Mid = {
    .Color = &YellowColor,
    .PeriodTicks = ms_Conversion(LED_RGYW_BREATH_PERIOD, LED_RGYW_PWM_FREQUENCY),
    .HoldInTicks = ms_Conversion(LED_RGYW_BREATH_HOLDIN_TIME, LED_RGYW_PWM_FREQUENCY),
    .HoldOutTicks = ms_Conversion(LED_RGYW_BREATH_HOLDOUT_TIME, LED_RGYW_PWM_FREQUENCY),
};

RGBLED_Breath_Handle_t LEDBatteryChargeBreath_High = {
    .Color = &GreenColor,
    .PeriodTicks = ms_Conversion(LED_RGYW_BREATH_PERIOD, LED_RGYW_PWM_FREQUENCY),
    .HoldInTicks = ms_Conversion(LED_RGYW_BREATH_HOLDIN_TIME, LED_RGYW_PWM_FREQUENCY),
    .HoldOutTicks = ms_Conversion(LED_RGYW_BREATH_HOLDOUT_TIME, LED_RGYW_PWM_FREQUENCY),
};
#endif 


