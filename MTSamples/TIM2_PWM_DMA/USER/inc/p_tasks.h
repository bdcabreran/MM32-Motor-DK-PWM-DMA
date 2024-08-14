#ifndef __P_TASKS_H__
#define __P_TASKS_H__

#include "p_type.h"

/** @brief Product commands
 * Allowed 16 maximum, priority of commands will be LSB to MSB
 */
#define UI_NO_CMD ((uint16_t)0x0000)
#define UI_TOUCH_PWR_SHORT ((uint16_t)0x0001)
#define UI_TOUCH_SPEED1_SHORT ((uint16_t)0x0002)
#define UI_TOUCH_SPEED2_SHORT ((uint16_t)0x0004)
#define UI_TOUCH_SPEED3_SHORT ((uint16_t)0x0008)
#define UI_TOUCH_TURBO_SHORT ((uint16_t)0x0010)
#define UI_TOUCH_PWR_LONG ((uint16_t)0x0020)
#define UI_TOUCH_SPEED1_LONG ((uint16_t)0x0040)
#define UI_TOUCH_SPEED2_LONG ((uint16_t)0x0080)
#define UI_TOUCH_SPEED3_LONG ((uint16_t)0x0100)
#define UI_TOUCH_TURBO_LONG ((uint16_t)0x0200)
#define UI_BTN_LONG_3 ((uint16_t)0x0400)     /**< @brief Button 3 long.*/
#define UI_BTN_DOUBLE_3 ((uint16_t)0x0800)   /**< @brief Button 3 double press.*/
#define UI_BTN_DOWN ((uint16_t)0x1000)  /**< @brief Special command 1.*/
// #define UI_SPECIAL_CMD_2 ((uint16_t)0x2000)  /**< @brief Special command 2.*/
// #define UI_SPECIAL_CMD_3 ((uint16_t)0x4000)  /**< @brief Special command 3.*/
// #define UI_SPECIAL_CMD_4 ((uint16_t)0x8000)  /**< @brief Special command 4.*/

#define MODE_MAX (3U)

/* Touch Input buttons */
#define TOUCH_NOTHING_PRESSED ((uint8_t)0x00)
#define TOUCH_PWR ((uint8_t)0x01)
#define TOUCH_TURBO ((uint8_t)0x10)
#define TOUCH_SPEED1 ((uint8_t)0x02)
#define TOUCH_SPEED2 ((uint8_t)0x04)
#define TOUCH_SPEED3 ((uint8_t)0x08)

// #define TOUCH_PWR ((uint8_t)0x05)
// #define TOUCH_PWR ((uint8_t)0x06)

typedef enum
{
  P_IDLE = 0, /**< Product in idle state, waiting for user action.
                 Can move to ON or SLEEP from here. */

  P_START, /**< User has requested function. Perform initialization
              to ensure we are in correct state for ON mode. */

  // P_CABLEIN, /**< Cable is plugged in for charging. Will remain in this
                // state always if cable is plugged in. */

  P_ON, /**< Product in in on state. Can move to M_IDLE or M_FAULT_NOW
           from here. */

  P_SLEEP, /**< Product in sleep state, can move to M_IDLE from here. */

#ifdef BATT_EMPTY_AUTO_OFF_ENABLE
  P_BATT_EMPTY,
#endif

  P_FAULT_NOW,

  P_FAULT_ACKNOWLEDGE,

  P_FAULT_OVER,

#ifdef ENABLE_POWER_DISPLAY
  P_POWER_DISPLAY,
#endif

} ProductStates_t;


typedef enum
{
  SPEED_SETTING_1 = 0,
  SPEED_SETTING_2,
  SPEED_SETTING_3,
  SPEED_SETTING_TURBO,
} SpeedSetting_t;

typedef enum
{
  P_CABLEIN_IDLE = 0,
  P_CABLEIN_CHARGING,
  P_CABLEIN_FINISHED,
} ProductCableIn_SubStates_t;

typedef struct
{
  ProductStates_t Now;
  ProductStates_t Previous;
  uint32_t Counter;
  bool Configured;
} ProductState_Handle_t;

typedef enum
{
  BATTERY_LEVEL_EMPTY,
  BATTERY_LEVEL_1,
  BATTERY_LEVEL_2,
  BATTERY_LEVEL_3,
  BATTERY_LEVEL_4,
  BATTERY_LEVEL_FULL,
} BATTLevel_State_t;

typedef enum
{
  PRODUCT_MODE_NORMAL = 0,
  PRODUCT_MODE_TIMER,
} ProductMode_t;

typedef struct
{
  ProductState_Handle_t State;
  uint16_t CurrentFaults;
  uint16_t NackedFaults;
  bool InitComplete;
  int8_t ErrorCodeIndex;
  uint8_t TouchID;
  SpeedSetting_t SpeedSetting;
  ProductMode_t NewMode;
  ProductMode_t Mode;
  bool TimerModeOn;
  uint32_t TimerModeCountdown;
  uint8_t PrevSpeedIndex;
  BATTLevel_State_t BatteryLevel;
} Product_Handle_t;

typedef enum
{
  UICmd_NONE = 0,
  UICmd_MassageButtonPressed_Short,
  UICmd_MassageButtonPressed_Long,
} UICmds_t;

void PD_RunProductControlTasks(void);
void PD_Initialise(void);
void PD_UpdateCommunicationFaultStatus(bool IsFault);
void PD_ProcessTouchButtons(void);
void PD_GetTouchID(uint8_t TouchID);


void PD_BtnPwr_OnShortButtonPress(void);
void PD_BtnPwr_OnLongButtonPress(void);
void LED_Complete_Callback(LED_Animation_Type_t animationType, LED_Status_t status);


#endif
