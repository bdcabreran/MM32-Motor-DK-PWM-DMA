#ifndef __P_PARAMETER_CONVERSION_H__
#define __P_PARAMETER_CONVERSION_H__

#include "config.h"
#include "p_parameters.h"
#include "parameter_conversion.h"
#include "p_type.h"

/********************* Timing and Clock Conversions *********************/

#define BUTTON_FILTER_TICKS ms_Conversion(BUTTON_FILTER_TIME, PRODUCT_TASK_FREQUENCY)
#define BUTTON_LONG_PRESS_TICKS ms_Conversion(BUTTON_LONG_PRESS_TIME, PRODUCT_TASK_FREQUENCY)
#define BUTTON_DOUBLE_PRESS_TICKS ms_Conversion(BUTTON_DOUBLE_PRESS_TIME, PRODUCT_TASK_FREQUENCY)

#define FAULT_ACK_TIMEOUT_TICKS ms_Conversion(FAULT_ACK_TIMEOUT, PRODUCT_TASK_FREQUENCY)
#define ERROR_FLASH_TICKS ms_Conversion(ERROR_FLASH_INTERVAL, PRODUCT_TASK_FREQUENCY)
#define BATT_LOW_FLASH_TICKS ms_Conversion(BATT_LOW_FLASH_INTERVAL, PRODUCT_TASK_FREQUENCY)
#define BATT_EMPTY_FLASH_TICKS ms_Conversion(BATT_EMPTY_FLASH_INTERVAL, PRODUCT_TASK_FREQUENCY)
#define BATT_EMPTY_WAIT_TICKS ms_Conversion(BATT_EMPTY_WAIT_TIME, PRODUCT_TASK_FREQUENCY)
#define CABLE_UNPLUGGED_DISPLAY_TICKS ms_Conversion(CABLE_UNPLUGGED_DISPLAY_TIMEOUT, PRODUCT_TASK_FREQUENCY)
#define MODE_CHANGE_FLASH_TICKS ms_Conversion(MODE_CHANGE_FLASH_INTERVAL, PRODUCT_TASK_FREQUENCY)
#define MODE_CHANGE_START_FLASH_TICKS ms_Conversion(MODE_CHANGE_START_FLASH_INTERVAL, PRODUCT_TASK_FREQUENCY)
#define SINGLE_BUTTON_PRESS_TIMEOUT_TICKS ms_Conversion(SINGLE_BUTTON_PRESS_TIMEOUT, PRODUCT_TASK_FREQUENCY)
#define POWER_DISPLAY_TICKS ms_Conversion(POWER_DISPLAY_DURATION, PRODUCT_TASK_FREQUENCY)
#define BATT_EMPTY_SHUTDOWN_TICKS second_Conversion(BATT_EMPTY_SHUTDOWN_TIME, PRODUCT_TASK_FREQUENCY)

#define BUZZER_DURATION_TURN_ON_TICKS ms_Conversion(BUZZER_DURATION_TURN_ON, PRODUCT_TASK_FREQUENCY)
#define BUZZER_DURATION_TURN_OFF_TICKS ms_Conversion(BUZZER_DURATION_TURN_OFF, PRODUCT_TASK_FREQUENCY)
#define BUZZER_DURATION_SPEED_CHANGE_ON_TICKS ms_Conversion(BUZZER_DURATION_SPEED_CHANGE_ON_TIME, PRODUCT_TASK_FREQUENCY)
#define BUZZER_DURATION_SPEED_CHANGE_TURBO_OFF_TICKS ms_Conversion(BUZZER_DURATION_SPEED_CHANGE_TURBO_OFF_TIME, PRODUCT_TASK_FREQUENCY)

#define BUZZER_PWM_PRESCALER (BUZZER_PWM_SCALER - 1)
#define BUZZER_PWM_MAX_PULSE ((uint32_t)MCU_CLOCK / BUZZER_PWM_FREQUENCY_1 / BUZZER_PWM_SCALER)
#define BUZZER_PWM_PERIOD (BUZZER_PWM_MAX_PULSE - 1)
#define BUZZER_DUTY_TICKS (BUZZER_PWM_PERIOD * BUZZER_DUTY / 100)

#define Buzzer_Calculate_PeriodTicks(f) ((uint32_t)MCU_CLOCK / f / BUZZER_PWM_SCALER - 1)

#define POWER_ON_ANIMATION_DURATION ms_Conversion((BUTTON_LONG_PRESS_TIME - SINGLE_BUTTON_PRESS_TIMEOUT) / BSSD_ANIMATION_SEGMENTS, PRODUCT_TASK_FREQUENCY)

#define SLEEP_TICKS second_Conversion(SLEEP_TIME, PRODUCT_TASK_FREQUENCY)
#define ON_TICKS minute_Conversion(ON_TIME, PRODUCT_TASK_FREQUENCY)

#define BATTERY_SUBSYSTEM_RUN_TICKS (PRODUCT_TASK_FREQUENCY / BATTERY_SUBSYSTEM_FREQUENCY)

#define HEATER_SUBSYSTEM_RUN_TICKS (PRODUCT_TASK_FREQUENCY / HEATER_SUBSYSTEM_FREQUENCY)

#define LED_RGYW_SUBSYSTEM_RUN_TICKS (PRODUCT_TASK_FREQUENCY / LED_RGYW_SUBSYSTEM_FREQUENCY)

#define MOTOR_LED_SPEED_LEVEL_3_TICKS ms_Conversion(MOTOR_LED_LEVEL3_LED_CHANGE_PEROID, SPEED_LEVEL_CHANGE_SUBSYSTEM_FREQUENCY)
#define MOTOR_LED_SPEED_LEVEL_4_TICKS ms_Conversion(MOTOR_LED_LEVEL4_LED_CHANGE_PEROID, SPEED_LEVEL_CHANGE_SUBSYSTEM_FREQUENCY)

#define LED_ERROR_FLASH_INTERVAL_TICKS ms_Conversion(LED_RGYW_ERROR_FLASH_INTERVAL, SPEED_LEVEL_CHANGE_SUBSYSTEM_FREQUENCY)

#define SPEED_LEVEL_CHANGE_SUBSYSTEM_RUN_TICKS (PRODUCT_TASK_FREQUENCY / SPEED_LEVEL_CHANGE_SUBSYSTEM_FREQUENCY)

#define BATTERY_CURRENT_CALIB_TICKS ms_Conversion(BATTERY_CURRENT_CALIB_TIME, PRODUCT_TASK_FREQUENCY)

#define CHARGING_INDICATOR_FLASH_ON_TICKS ms_Conversion(CHARGING_INDICATOR_FLASH_ON_TIME, PRODUCT_TASK_FREQUENCY)
#define CHARGING_INDICATOR_FLASH_OFF_TICKS ms_Conversion(CHARGING_INDICATOR_FLASH_OFF_TIME, PRODUCT_TASK_FREQUENCY)

#define DC_IN_DETECT_TO_ACTIVE_TICKS ms_Conversion(DC_IN_DETECT_TO_ACTIVE_TIME, PRODUCT_TASK_FREQUENCY)
#define DC_IN_DETECT_TO_INACTIVE_TICKS ms_Conversion(DC_IN_DETECT_TO_INACTIVE_TIME, PRODUCT_TASK_FREQUENCY)

#define MODE_DETECT_TO_ACTIVE_TICKS ms_Conversion(MODE_DETECT_TO_ACTIVE_TIME, PRODUCT_TASK_FREQUENCY)
#define MODE_DETECT_TO_INACTIVE_TICKS ms_Conversion(MODE_DETECT_TO_INACTIVE_TIME, PRODUCT_TASK_FREQUENCY)

#define TIMER_MODE_DURATION_TICKS (minute_Conversion(TIMER_MODE_DURATION, PRODUCT_TASK_FREQUENCY))
#define MODE_CHANGE_BEEP_DURATION_TICKS (ms_Conversion(MODE_CHANGE_BEEP_DURATION, PRODUCT_TASK_FREQUENCY))

/************************************************************************/
#define LED_RGYW_SUBSYSTEM_RUN_TICKS (PRODUCT_TASK_FREQUENCY / LED_RGYW_SUBSYSTEM_FREQUENCY)

#define LED_RGYW_ERROR_FLASH_INTERVAL_TICKS ms_Conversion(LED_RGYW_ERROR_FLASH_INTERVAL, LED_RGYW_SUBSYSTEM_FREQUENCY)

#define LED_RGYW_BATT_VERYLOW_FLASH_ON_TICKS ms_Conversion(LED_RGYW_BATT_VERYLOW_FLASH_ON_TIME, LED_RGYW_SUBSYSTEM_FREQUENCY)
#define LED_RGYW_BATT_VERYLOW_FLASH_OFF_TICKS ms_Conversion(LED_RGYW_BATT_VERYLOW_FLASH_OFF_TIME, LED_RGYW_SUBSYSTEM_FREQUENCY)

#define LED_RGYW_BATTERY_BREATH_PERIOD_TICKS ms_Conversion(LED_RGYW_BREATH_PERIOD, LED_RGYW_SUBSYSTEM_FREQUENCY)
#define LED_RGYW_BATTERY_BREATH_HOLDIN_TICKS ms_Conversion(LED_RGYW_BREATH_HOLDIN_TIME, LED_RGYW_SUBSYSTEM_FREQUENCY)
#define LED_RGYW_BATTERY_BREATH_HOLDOUT_TICKS ms_Conversion(LED_RGYW_BREATH_HOLDOUT_TIME, LED_RGYW_SUBSYSTEM_FREQUENCY)

#define SPEED_LED_FLASH_DURATION_ON_TICKS ms_Conversion(SPEED_LED_FLASH_DURATION_ON, PRODUCT_TASK_FREQUENCY)
#define SPEED_LED_FLASH_DURATION_OFF_TICKS ms_Conversion(SPEED_LED_FLASH_DURATION_OFF, PRODUCT_TASK_FREQUENCY)

#define LED_RGYW_PWM_SCALER (MCU_CLOCK_MHZ)
#define LED_RGYW_PWM_PRESCALER (LED_RGYW_PWM_SCALER - 1)
#define LED_RGYW_PWM_MAX_PULSE ((uint32_t)MCU_CLOCK / LED_RGYW_PWM_FREQUENCY / LED_RGYW_PWM_SCALER)
#define LED_RGYW_PWM_PERIOD (LED_RGYW_PWM_MAX_PULSE - 1)

#define BATT_DISPLAY_DURATION_TICKS second_Conversion(BATT_DISPLAY_DURATION, PRODUCT_TASK_FREQUENCY)
#define BATT_FULL_LIGHT_OFF_DURATION_TICKS second_Conversion(BATT_FULL_LIGHT_OFF_DURATION, PRODUCT_TASK_FREQUENCY)

#define BATT_DISPLAY_COUNTDOWN_TO_MINIMUM_VALUE 10

/********************* Battery Parameter Conversions *********************/
#define BATT_C_to_A(c) (c * BATTERY_CELL_MAH / 1000.0f)

#define BATT_CellV_To_PackV(v) ((float)BATTERY_S * (float)v)

#define BATT_VOLTAGE_MAX BATT_CellV_To_PackV(4.2f)
#define BATT_VOLTAGE_BASE (BATT_VOLTAGE_MAX)
#define BATT_VOLTAGE_MAX_S16 Unit_Conversion(BATT_VOLTAGE_MAX, BATT_VOLTAGE_BASE)

/* Battery and motor use different base units by convention. */
#define BATT_VOLTAGE_BASE_FACTOR ((float)MOTOR_VMAX / (float)BATT_VOLTAGE_BASE)

#define BATTERY_VOLTAGE BATT_CellV_To_PackV(3.7f)
#define BATT_CHARGE_CURRENT_BASE (BATTERY_CELL_MAH * BATTERY_MAX_CHARGE / 1000.0f)
#define BATT_DISCHARGE_CURRENT_BASE (MOTOR_ISUM_IMAX)
#define BATTERY_CHARGE_PWM_PERIOD (MCU_CLOCK / BATTERY_CHARGE_PWM_FREQUENCY - 1)

#define BATTERY_CHARGE_PWM_MAX_PULSE ((BATTERY_CHARGE_PWM_PERIOD + 1) * BATTERY_CHARGE_PWM_MAX_DUTY)
#define BATTERY_CHARGE_PWM_MIN_PULSE ((BATTERY_CHARGE_PWM_PERIOD + 1) * BATTERY_CHARGE_PWM_MIN_DUTY)

#define BATT_VIN_MIN_S16 Unit_Conversion(BATT_VIN_MIN, BATT_VOLTAGE_BASE)

/************** ADC Measurements ************/
#define BATTERY_VIN_CONVERSION_FACTOR ADC_Voltage_Conversion_Factor(ADC_VREF, BATT_VIN_R1, BATT_VIN_R2, BATT_VOLTAGE_BASE)

#if (BATT_OPAMP_CONFIG == BATT_OPAMP_YES)
#define BATTERY_CURRENT_CONVERSION_FACTOR (ADC_Current_Conversion_Factor(ADC_VREF, BATTERY_CURRENT_R_SHUNT, BATTERY_CURRENT_GAIN, BATT_CHARGE_CURRENT_BASE))
#else
/* Resistor ratio multiplier is used to compensate the offset bias. */
#define BATTERY_CURRENT_CONVERSION_FACTOR (ADC_Current_Conversion_Factor(ADC_VREF, BATTERY_CURRENT_R_SHUNT, BATTERY_CURRENT_GAIN, BATT_CHARGE_CURRENT_BASE) * ((BATTERY_CURRENT_R1 + BATTERY_CURRENT_R2) / BATTERY_CURRENT_R1))
#endif

/*********************************************/

/********* State Machine *******/
#define BATTERY_CURRENT_CALIB_TICKS ms_Conversion(BATTERY_CURRENT_CALIB_TIME, PRODUCT_TASK_FREQUENCY)
#define BATTERY_SUBSYSTEM_RUN_TICKS (PRODUCT_TASK_FREQUENCY / BATTERY_SUBSYSTEM_FREQUENCY)

#define BATT_CHECK_DELAY_TICKS (ms_Conversion(BATT_CHECK_DELAY, BATTERY_SUBSYSTEM_FREQUENCY))
#define BATT_ACTIVATION_TICKS (ms_Conversion(BATT_ACTIVATION_TIMEOUT, BATTERY_SUBSYSTEM_FREQUENCY))
#define BATT_NO_DETECT_THRESHOLD_S16 (Voltage_Conversion(BATT_NO_DETECT_THRESHOLD, BATT_VOLTAGE_BASE))

#define BATT_CURRENT_DESIGNED_OFFSET_S16 ((uint32_t)UINT16_MAX * BATT_CURRENT_DESIGNED_OFFSET / ADC_VREF)

#define BATT_CHECK_DELAY_TICKS (ms_Conversion(BATT_CHECK_DELAY, BATTERY_SUBSYSTEM_FREQUENCY))
#define BATT_ACTIVATION_TIMEOUT_TICKS (ms_Conversion(BATT_ACTIVATION_TIMEOUT, BATTERY_SUBSYSTEM_FREQUENCY))
#define BATT_NO_DETECT_THRESHOLD_S16 (Voltage_Conversion(BATT_NO_DETECT_THRESHOLD, BATT_VOLTAGE_BASE))

#define BATT_ACTIVATION_CURRENT_THRESHOLD_S16 Unit_Conversion(BATT_C_to_A(BATT_ACTIVATION_CURRENT_THRESHOLD), BATT_CHARGE_CURRENT_BASE)

#define BATT_CHARGE_TICKS_LIMIT hour_Conversion(BATT_CHARGE_TIME_LIMIT, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_FINISHED_CHARGE_WAIT_TICKS hour_Conversion(BATT_FINISHED_CHARGE_WAIT_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_CV_CHARGE_TICKS_LIMIT hour_Conversion(BATT_CV_CHARGE_TIME_LIMIT, BATTERY_SUBSYSTEM_FREQUENCY)

#define BATT_PRECHARGE_CURRENT_S16 Unit_Conversion(BATT_C_to_A(BATTERY_PRECHARGE), BATT_CHARGE_CURRENT_BASE)
#define BATT_MAX_CHARGE_CURRENT_S16 Unit_Conversion(BATT_C_to_A(BATTERY_MAX_CHARGE), BATT_CHARGE_CURRENT_BASE)

#define BATT_HAS_VIN_TRIGGER_TICKS ms_Conversion(BATT_HAS_VIN_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_HAS_VIN_THRESHOLD_S16 Unit_Conversion(BATT_HAS_VIN_THRESHOLD, BATT_VOLTAGE_BASE)
#define BATT_NOT_HAVE_VIN_THRESHOLD_S16 Unit_Conversion(BATT_NOT_HAVE_VIN_THRESHOLD, BATT_VOLTAGE_BASE)

#define BATT_PRE_CHARGE_EXIT_TICKS ms_Conversion(BATT_PRE_CHARGE_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_PRE_CHARGE_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_PRE_CHARGE_THRESHOLD), BATT_VOLTAGE_BASE)

#define BATT_CAN_CHARGE_AGAIN_TRIGGER_TICKS ms_Conversion(BATT_CAN_CHARGE_AGAIN_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_CAN_CHARGE_AGAIN_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_CAN_CHARGE_AGAIN_TRIGGER_THRESHOLD), BATT_VOLTAGE_BASE)
#define BATT_CAN_CHARGE_AGAIN_TRIGGER_THRESHOLD_TICKS (ms_Conversion(BATT_CAN_CHARGE_AGAIN_TRIGGER_THRESHOLD_TIME, BATTERY_SUBSYSTEM_FREQUENCY))

#define BATT_CCtoCV_TRIGGER_TICKS ms_Conversion(BATT_CV_CHARGE_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_CV_CHARGE_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_CV_CHARGE_THRESHOLD), BATT_VOLTAGE_BASE)

#define BATT_TAPER_CURRENT_EXIT_TICKS ms_Conversion(BATT_TAPER_CURRENT_EXIT_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_TAPER_CURRENT_THRESHOLD_S16 Unit_Conversion(BATT_C_to_A(BATT_TAPER_CURRENT_THRESHOLD), BATT_CHARGE_CURRENT_BASE)

#define BATT_IS_DISCHARGING_TRIGGER_TICKS ms_Conversion(BATT_IS_DISCHARGING_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_IS_DISCHARGING_TRIGGER_THRESHOLD_S16 (Current_Conversion(BATT_IS_DISCHARGING_TRIGGER_THRESHOLD, MOTOR_ISUM_IMAX))
#define BATT_IS_NOT_DISCHARGING_THRESHOLD_S16 (Current_Conversion(BATT_IS_NOT_DISCHARGING_THRESHOLD, MOTOR_ISUM_IMAX))

#define BATT_EMPTY_TRIGGER_TICKS ms_Conversion(BATT_EMPTY_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_EMPTY_TRIGGER_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_EMPTY_TRIGGER_THRESHOLD), BATT_VOLTAGE_BASE)
#define BATT_EMPTY_RECOVER_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_EMPTY_RECOVER_THRESHOLD), BATT_VOLTAGE_BASE)

#define BATT_EMPTY_2_TRIGGER_TICKS ms_Conversion(BATT_EMPTY_2_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_EMPTY_2_TRIGGER_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_EMPTY_2_TRIGGER_THRESHOLD), BATT_VOLTAGE_BASE)
#define BATT_EMPTY_2_RECOVER_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_EMPTY_2_RECOVER_THRESHOLD), BATT_VOLTAGE_BASE)

#define BATT_LOW_TRIGGER_TICKS ms_Conversion(BATT_LOW_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_LOW_TRIGGER_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_LOW_TRIGGER_THRESHOLD), BATT_VOLTAGE_BASE)
#define BATT_LOW_RECOVER_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_LOW_RECOVER_THRESHOLD), BATT_VOLTAGE_BASE)

#define BATT_FIRST_GET_LEVEL_CHECK_TICKS ms_Conversion(BATT_FIRST_GET_LEVEL_CHECK_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_FIRST_GET_LEVEL_TRIGGER_TICKS ms_Conversion(BATT_FIRST_GET_LEVEL_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_LEVEL_TRIGGER_TICKS ms_Conversion(BATT_CHARGE_LEVEL_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)

#define BATT_CHARGE_LEVEL1_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_CHARGE_LEVEL1_THRESHOLD), BATT_VOLTAGE_BASE)
#define BATT_CHARGE_LEVEL2_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_CHARGE_LEVEL2_THRESHOLD), BATT_VOLTAGE_BASE)
#define BATT_CHARGE_LEVEL3_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_CHARGE_LEVEL3_THRESHOLD), BATT_VOLTAGE_BASE)

#define BATT_DISCHARGE_LEVEL1_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_DISCHARGE_LEVEL1_THRESHOLD), BATT_VOLTAGE_BASE)
#define BATT_DISCHARGE_LEVEL2_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_DISCHARGE_LEVEL2_THRESHOLD), BATT_VOLTAGE_BASE)
#define BATT_DISCHARGE_LEVEL3_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_DISCHARGE_LEVEL3_THRESHOLD), BATT_VOLTAGE_BASE)
/*******************************/

/********** Protections ********/
#define BATT_OC_CHARGE_TRIGGER_TICKS ms_Conversion(BATT_OC_CHARGE_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_OC_CHARGE_THRESHOLD_S16 Unit_Conversion(BATT_C_to_A(BATT_OC_CHARGE_THRESHOLD), BATT_CHARGE_CURRENT_BASE)
#define BATT_OC_CHARGE_RECOVER_THRESHOLD_S16 Unit_Conversion(BATT_C_to_A(BATT_OC_CHARGE_RECOVER_THRESHOLD), BATT_CHARGE_CURRENT_BASE)

#define BATT_OV_TRIGGER_TICKS ms_Conversion(BATT_OV_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_OV_TRIGGER_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_OV_THRESHOLD), BATT_VOLTAGE_BASE)
#define BATT_OV_RECOVER_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_OV_RECOVER_THRESHOLD), BATT_VOLTAGE_BASE)

#define BATT_OV_2_TRIGGER_TICKS ms_Conversion(BATT_OV_2_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_OV_2_TRIGGER_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_OV_2_THRESHOLD), BATT_VOLTAGE_BASE)
#define BATT_OV_2_RECOVER_THRESHOLD_S16 Unit_Conversion(BATT_CellV_To_PackV(BATT_OV_2_RECOVER_THRESHOLD), BATT_VOLTAGE_BASE)


#define BATT_UT_TRIGGER_TICKS ms_Conversion(BATT_UT_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)
#define BATT_OT_TRIGGER_TICKS ms_Conversion(BATT_OT_TRIGGER_TIME, BATTERY_SUBSYSTEM_FREQUENCY)

#define BATT_OT_CHG_THRESHOLD_01DEG (temperature_Conversion(BATT_OT_CHG_THRESHOLD))
#define BATT_OT_CHG_RECOVER_THRESHOLD_01DEG (temperature_Conversion(BATT_OT_CHG_RECOVER_THRESHOLD))

#define BATT_UT_CHG_THRESHOLD_01DEG (temperature_Conversion(BATT_UT_CHG_THRESHOLD))
#define BATT_UT_CHG_RECOVER_THRESHOLD_01DEG (temperature_Conversion(BATT_UT_CHG_RECOVER_THRESHOLD))

#define BATT_OT_DCHG_THRESHOLD_01DEG (temperature_Conversion(BATT_OT_DCHG_THRESHOLD))
#define BATT_OT_DCHG_RECOVER_THRESHOLD_01DEG (temperature_Conversion(BATT_OT_DCHG_RECOVER_THRESHOLD))

#define BATT_UT_DCHG_THRESHOLD_01DEG (temperature_Conversion(BATT_UT_DCHG_THRESHOLD))
#define BATT_UT_DCHG_RECOVER_THRESHOLD_01DEG (temperature_Conversion(BATT_UT_DCHG_RECOVER_THRESHOLD))

/*******************************/

/************************************************************************/

/************************** Heater Parameters **************************/
#define HEATER_TEMPERATURE_SETPOINT_01DEG (temperature_Conversion(HEATER_TEMPERATURE_SETPOINT))

#define HEATER_TEMPERATURE_STARTUP_FAULT_DELAY_TICKS (second_Conversion(HEATER_TEMPERATURE_STARTUP_FAULT_DELAY, HEATER_SUBSYSTEM_FREQUENCY))

#define HEATER_KP_TICKS_PER_01DEG (ms_Conversion(HEATER_KP, HEATER_SUBSYSTEM_FREQUENCY) / 10)
#define HEATER_KD_TICKS_PER_01DEG (ms_Conversion(HEATER_KD * HEATER_SUBSYSTEM_FREQUENCY, HEATER_SUBSYSTEM_FREQUENCY) / 10)

#define HEATER_MIN_ON_TICKS ((uint16_t)ms_Conversion(HEATER_MIN_ON_TIME, HEATER_SUBSYSTEM_FREQUENCY))
#define HEATER_MAX_OFF_TICKS ((uint16_t)ms_Conversion(HEATER_MAX_OFF_TIME, HEATER_SUBSYSTEM_FREQUENCY))

#define HEATER_TEMPERATURE_FAULT_TICKS (ms_Conversion(HEATER_TEMPERATURE_FAULT_TIME, HEATER_SUBSYSTEM_FREQUENCY))
#define HEATER_TEMPERATURE_OK_01DEG (temperature_Conversion(HEATER_TEMPERATURE_OK))
#define HEATER_TEMPERATURE_NOTOK_01DEG (temperature_Conversion(HEATER_TEMPERATURE_NOTOK))

#define HEATER_OT_TRIGGER_TICKS (ms_Conversion(HEATER_OT_TRIGGER_TIME, HEATER_SUBSYSTEM_FREQUENCY))
#define HEATER_OT_THRESHOLD_01DEG (temperature_Conversion(HEATER_OT_THRESHOLD))
#define HEATER_OT_RECOVER_THRESHOLD_01DEG (temperature_Conversion(HEATER_OT_RECOVER_THRESHOLD))

/************************************************************************/

#endif
