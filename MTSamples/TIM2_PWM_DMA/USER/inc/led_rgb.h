#ifndef __RGB_LED_H__
#define __RGB_LED_H__

#include "p_type.h"
#include "pwm_led.h"

#define RGBLED_NO_CMD 0x00
#define RGBLED_OFF_CMD 0x01
#define RGBLED_SOLID_CMD 0x02
#define RGBLED_FLASH_CMD 0x04
#define RGBLED_BREATH_CMD 0x08

#define RGBLED_COLOR_RESOLUTION (256U)
#define RGBLED_COLOR_RESOLUTION_BITS (8U)

#define RGBLED_BREATH_RESOLUTION ((uint32_t)65536U)
#define RGBLED_BREATH_RESOLUTION_BITS (16U)
#define RGBLED_BREATH_TICKS_MAX_BITS (8U)

typedef enum
{
  RGBLED_OFF = 0, /*!< Persistent state. LED is off and all timers are paused. */

  RGBLED_SOLID, /*!< Persistent state. Solid brightness. */

  RGBLED_FLASH, /*!< Timed state. LED flashes a configurable number of times
                  then moves to next command. Default is OFF if none given. */

  RGBLED_BREATH, /*!< Persistent state. LED is breathing with configurable period
                    and brightnesses. */

} RGBLEDStates_t;

/** @brief Dummy handle used as parent type for all command data handles. */
typedef struct
{
  uint8_t ID; /* Dummy variable for arm compiler v5 support. */
} RGBLED_Cmd_Handle_t;

typedef struct
{
  uint8_t R;
  uint8_t G;
  uint8_t B;
} RGBColor_Handle_t;

typedef struct
{
  uint16_t R;
  uint16_t G;
  uint16_t B;
} RGBColor_PWM_Handle_t;

typedef enum
{
  RGBLEDFlashType_ON = 0, /*!< LED flashes on then off. Finishing in off state. */
  RGBLEDFlashType_OFF,    /*!< LED flashes off then on. Finishing in on state. */
} RGBLEDFlashType_t;

typedef enum
{
  RGBLEDBreathState_IN = 0, /*!< LED breathing in. */
  RGBLEDBreathState_HOLDIN, /*!< LED holding breath after inhale. */
  RGBLEDBreathState_OUT,    /*!< LED breathing out. */
  RGBLEDBreathState_HOLDOUT, /*!< LED holding breath after exhale. */
} RGBLEDBreathState_t;

typedef struct
{
  RGBLEDBreathState_t State;
  uint32_t Value;
  uint16_t ValueInc;
  uint16_t InhaleExhaleTicks;
  uint16_t HoldInTicks;
  uint16_t HoldOutTicks;
  uint8_t Counter;
  uint8_t InhaleExhaleIncrements;
} RGBLED_Breath_InternalHandle_t;

typedef struct
{
  RGBLED_Cmd_Handle_t _Super;
  RGBColor_Handle_t *Color;
} RGBLED_Solid_Handle_t;

typedef struct
{
  RGBLED_Cmd_Handle_t _Super;
  RGBColor_Handle_t *Color;
  RGBLEDFlashType_t Type;
  int8_t Times;
  uint16_t OnTicks;
  uint16_t OffTicks;
} RGBLED_Flash_Handle_t;

typedef struct
{
  RGBLED_Cmd_Handle_t _Super;
  RGBColor_Handle_t *Color;
  uint16_t PeriodTicks;
  uint16_t HoldInTicks;
  uint16_t HoldOutTicks;
} RGBLED_Breath_Handle_t;

typedef struct
{
  RGBLEDStates_t Now;
  RGBLEDStates_t Prev;
  bool Configured;
  uint32_t Counter;
} RGBLED_State_Handle_t;

/** @brief Pointer on functions used for timer configuration. */
typedef void (*RGBLED_TimerConfig)(void);

typedef struct 
{
  RGBLED_TimerConfig EnablePWM;
  RGBLED_TimerConfig DisablePWM;

  volatile uint32_t *CCR_Red; 
  volatile uint32_t *CCR_Green;
  volatile uint32_t *CCR_Blue;
} RGBLED_Timer_Handle_t;

typedef struct
{
  /******************* Configuration Parameters *******************/
  RGBLED_Timer_Handle_t TimerInterface; /*!< Timer interface for PWM. */

  uint32_t MaxPulse; /*!< Max pulse for PWM period. */

  LED_Active_t ActiveConfig;
  /***************************************************************/

  RGBLED_State_Handle_t State;

  uint8_t CommandNow;
  uint8_t CommandNext;

  RGBLED_Cmd_Handle_t *CommandNow_Handle;
  RGBLED_Cmd_Handle_t *CommandNext_Handle;

  RGBColor_PWM_Handle_t PWMValue;

  RGBLED_Breath_InternalHandle_t Breath;

} RGBLED_Handle_t;

void RGBLED_Init(RGBLED_Handle_t *this);
void RGBLED_Reset(RGBLED_Handle_t *this);

void RGBLED_Update(RGBLED_Handle_t *this);

/* LED Command Interface. */
void RGBLED_Command_Off(RGBLED_Handle_t *this, bool ForceNow);
void RGBLED_Command_Solid(RGBLED_Handle_t *this, RGBLED_Solid_Handle_t *CmdHandle, bool ForceNow);
void RGBLED_Command_Flash(RGBLED_Handle_t *this, RGBLED_Flash_Handle_t *CmdHandle, bool ForceNow);
void RGBLED_Command_Breath(RGBLED_Handle_t *this, RGBLED_Breath_Handle_t *CmdHandle, bool ForceNow);
void RGBLED_ClearBufferedCommands(RGBLED_Handle_t *this);

#endif
