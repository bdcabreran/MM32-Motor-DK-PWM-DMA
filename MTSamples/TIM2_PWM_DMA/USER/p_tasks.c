#include "main.h"
#include "p_tasks.h"
#include "p_config.h"
#include "p_parameter_conversion.h"
#include "debug_config.h"
#include "mci_mock.h"
#include "bsp_led_pwm.h"

#define PTASK_DBG_ENABLE 1

#if PTASK_DBG_ENABLE

#define DEBUG_BUFFER_SIZE 256
static char debugBuffer[DEBUG_BUFFER_SIZE];
static const char* TAG = "PTASK";

#define DBG_MSG(fmt, ...) do { \
  uint32_t timestemp = Get_Systick_Cnt(); \
  snprintf(debugBuffer, DEBUG_BUFFER_SIZE, "(%d)%s: " fmt, timestemp, TAG, ##__VA_ARGS__); \
  Uart_Put_Buff(debugBuffer, strlen(debugBuffer)); \
} while(0)
#else
#define DBG_MSG(fmt, ...) do { } while(0)
#endif



// Define if you want White LED to be used, otherwise Default Color (RGY Color) will be used.
#define USE_WHITE_LED (1)

#if defined(ENABLE_DEBUG_MODE)
uint8_t DEBUGGER_SHORT_PRESS = 0;
uint8_t DEBUGGER_DOUBLE_PRESS = 0;
uint8_t DEBUGGER_LONG_PRESS = 0;
uint8_t DEBUGGER_LED_TURBO = 0;
uint8_t DEBUGGER_LED_SPEED1 = 0;
uint8_t DEBUGGER_LED_SPEED2 = 0;
uint8_t DEBUGGER_LED_SPEED3 = 0;
bool DEBUGGER_IS_LED_TURBO_ON = false;
bool DEBUGGER_IS_LED_SPEED1_ON = false;
bool DEBUGGER_IS_LED_SPEED2_ON = false;
bool DEBUGGER_IS_LED_SPEED3_ON = false;
#endif
/// TODO: if battery has overvoltage condition, turn green LED on to say battery is full?

static volatile uint32_t BattEmptyShutdownCounter = ((uint32_t)0);
static volatile uint16_t FaultResetCounter = ((uint16_t)0);
static volatile uint16_t ChargingIndicatorCounter = ((uint16_t)0);
static volatile uint16_t WaitForSinglePressTimeout = ((uint16_t)0);
static volatile uint16_t BatteryFullLEDCounter = ((uint16_t)0);
static volatile bool SinglePressTimeExceeded = false;
static volatile bool ChargingIndicatorOn = false;
static volatile bool ChargingFinished = false;
static volatile bool BattEmptyTimerStarted = false;
static BATTLevel_State_t PreviousBatteryLevel = 99;
BZR_States_t BuzzerStatePrev = BZR_OFF;
static volatile uint16_t LEDFlashCounter = ((uint16_t)0);
static volatile bool LEDFlashState = false;
static volatile bool SendDefaultLEDCommand = true;

ProductCableIn_SubStates_t CableInSubState = P_CABLEIN_IDLE;

static uint16_t UICommand = UI_NO_CMD;

static bool BatteryIsEmpty = false;
static bool BatteryIsCharging = false;
static bool BatteryIsFinishedCharging = false;
static bool CommunicationFaultOccurred = false;
int8_t BatteryDisplayPercent = -1;
static bool TurnOnWhiteLEDOnCompletion = false; 

bool ShowBatteryLife = false;
bool ShowBatteryLifeConfigured = false;
uint16_t ShowBatteryLifeTimer = 0;

int8_t SpeedMode = -1;

#define LED_RGYW_INTERPOLATION_TIME_MS 300
#define LED_RGYW_TRANSITION_IMMEDIATELY 0

static void PD_ProcessDebuggerCommands(void);

static void PD_UserInput(void);
static void PD_RunStateMachine(void);

void PD_BtnPwr_OnShortButtonPress(void);
void PD_BtnPwr_OnLongButtonPress(void);
static void PD_BtnSpeed1_OnShortButtonPress(void);
static void PD_BtnSpeed1_OnLongButtonPress(void);
static void PD_BtnSpeed2_OnShortButtonPress(void);
static void PD_BtnSpeed2_OnLongButtonPress(void);
static void PD_BtnSpeed3_OnShortButtonPress(void);
static void PD_BtnSpeed3_OnLongButtonPress(void);
static void PD_BtnTurbo_OnShortButtonPress(void);
static void PD_BtnTurbo_OnLongButtonPress(void);
void PD_RunSpeedChangeIndication(uint8_t Index);
static void PD_RunBuzzer(void);
static void PD_RunSubsystem_RGBLED(void);
static void PD_SetBatteryLevel(uint8_t Percentage);
static void PD_SetBatteryLED_Discharging(BATTLevel_State_t Level, bool ForceSendCmd);
static void PD_SetBatteryLED_Charging(BATTLevel_State_t Level, bool ForceSendCmd);
static void PD_RunPowerModeStateMachine(void);
static void PD_AllLEDsOff(void);

bool PD_CheckErrorCodes(void);
void PD_RunWaitForSinglePress(void);

static uint16_t PD_UICommandScheduler(void);

static void PD_ClearAllBufferedCommands(void);

#if USE_NEW_LED_LIBRARY
//void LED_Complete_Callback(LED_Animation_Type_t animationType, LED_Status_t status, void *AnimationPtr);
#endif

void PD_Initialise(void)
{
  Product.InitComplete = false;

  BTN_Init(&BtnPwr);
  BTN_RegisterCallback_ShortPress(&BtnPwr, &PD_BtnPwr_OnShortButtonPress);
  BTN_RegisterCallback_LongPress(&BtnPwr, &PD_BtnPwr_OnLongButtonPress);

  BTN_Init(&BtnSpeed1);
  BTN_RegisterCallback_ShortPress(&BtnSpeed1, &PD_BtnSpeed1_OnShortButtonPress);
  BTN_RegisterCallback_LongPress(&BtnSpeed1, &PD_BtnSpeed1_OnLongButtonPress);

  BTN_Init(&BtnSpeed2);
  BTN_RegisterCallback_ShortPress(&BtnSpeed2, &PD_BtnSpeed2_OnShortButtonPress);
  BTN_RegisterCallback_LongPress(&BtnSpeed2, &PD_BtnSpeed2_OnLongButtonPress);

  BTN_Init(&BtnSpeed3);
  BTN_RegisterCallback_ShortPress(&BtnSpeed3, &PD_BtnSpeed3_OnShortButtonPress);
  BTN_RegisterCallback_LongPress(&BtnSpeed3, &PD_BtnSpeed3_OnLongButtonPress);

  BTN_Init(&BtnTurbo);
  BTN_RegisterCallback_ShortPress(&BtnTurbo, &PD_BtnTurbo_OnShortButtonPress);
  BTN_RegisterCallback_LongPress(&BtnTurbo, &PD_BtnTurbo_OnLongButtonPress);

  DETSW_Init(&DCInDetectSwitch);

#if USE_NEW_LED_LIBRARY
  DBG_MSG("Using new LED library 10/09/2024.\n");
  // LED Complete Callback is optional and can be used to trigger other events when LED animations are complete.
  LED_Animation_Init(&MainLED, &LED_Controller, LED_Complete_Callback);
  LED_Transition_Init(&LEDTransition, &MainLED);

  // This Mapping is optional and can be used to map the LED transitions to the LED animations.
  LED_Transition_SetMapping(&LEDTransition, LEDTransitionMap, LED_TRANSITION_MAP_SIZE);
#else 
  DBG_MSG("Using old LED library.\n");
  RGBLED_Init(&MainLED);
#endif 

  BZR_Init(&Buzzer);
  Buzzer_Disable();

  /* Initialize to idle state. */
  Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};

  Product.InitComplete = true;
}

/**
 * @brief  Runs all product control tasks.
 */
void PD_RunProductControlTasks(void)
{
  if (Product.InitComplete != true)
  {
    /* Do nothing. */
  }
  else
  {
    /* Check for user input before running state machine. */
    PD_UserInput();

    /* Run product state machine. */
    PD_RunStateMachine();
    PD_RunPowerModeStateMachine();

    PD_RunBuzzer();

    PD_RunSubsystem_RGBLED();

    /* Decrement any counters if necessary. */
    if (Product.State.Counter > 0)
    {
      Product.State.Counter--;
    }
  }
}

static void PD_RunBuzzer(void)
{
  BZR_ReturnData_t BuzzerCommand;
  BZR_Run(&Buzzer, &BuzzerCommand);

  if (BuzzerStatePrev != BuzzerCommand.State)
  {
    if (BuzzerCommand.State == BZR_ON)
    {
      Buzzer_Enable(BuzzerCommand.Frequency);
    }
    else
    {
      Buzzer_Disable();
    }
    BuzzerStatePrev = BuzzerCommand.State;
  }
}

static void PD_UserInput(void)
{
  bool IsButtonDown, IsDCInHigh;
  bool Mode1High, Mode2High;
  int8_t NewSpeedMode;

  /* Load information from the touch sensors into the buttons structs, and update them. */
  PD_ProcessTouchButtons();

  // IsButtonDown = BUTTON_READREG & BUTTON_PIN ? false : true;
  // BTN_Update(&Button, IsButtonDown);

  // IsDCInHigh = DC_PLUG_IN_READREG & DC_PLUG_IN_PIN ? true : false;
  // MCI_IsCablePlugged() = DETSW_Update(&DCInDetectSwitch, IsDCInHigh);

  PD_ProcessDebuggerCommands();
}

static void PD_RunSubsystem_RGBLED(void)
{
  static uint8_t LEDUpdateCounter = 0;
  static uint32_t tick = 0;

  if (++LEDUpdateCounter >= LED_RGYW_SUBSYSTEM_RUN_TICKS)
  {
    #if USE_NEW_LED_LIBRARY
    LED_Transition_Update(&LEDTransition, tick);
    #else 
    RGBLED_Update(&MainLED);
    #endif

    LEDUpdateCounter = 0;
    tick++; // update tick every 1ms

  }
}

static void PD_BtnPwr_OnShortButtonPress(void)
{
  DBG_MSG("Power button short press.\n");
  UICommand |= UI_TOUCH_PWR_SHORT;
}

static void PD_BtnPwr_OnLongButtonPress(void)
{
  DBG_MSG("Power button long press.\n");
  UICommand |= UI_TOUCH_PWR_LONG;
}

static void PD_BtnSpeed1_OnShortButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED1_SHORT;
}

static void PD_BtnSpeed1_OnLongButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED1_LONG;
}

static void PD_BtnSpeed2_OnShortButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED2_SHORT;
}

static void PD_BtnSpeed2_OnLongButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED2_LONG;
}

static void PD_BtnSpeed3_OnShortButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED3_SHORT;
}

static void PD_BtnSpeed3_OnLongButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED3_LONG;
}

static void PD_BtnTurbo_OnShortButtonPress(void)
{
  UICommand |= UI_TOUCH_TURBO_SHORT;
}

static void PD_BtnTurbo_OnLongButtonPress(void)
{
  UICommand |= UI_TOUCH_TURBO_LONG;
}

static void PD_ClearAllBufferedCommands(void)
{
  UICommand = UI_NO_CMD;
}

/**
 * @brief  Retrieves a single UI command in LSB priority order.
 */
/// TODO: this might be better moved to product core.
static uint16_t PD_UICommandScheduler(void)
{
  /* Only look for commands if there are any. */
  if (UICommand != 0x00)
  {
    for (uint16_t b = 0x0001; b <= 0x8000; b <<= 1)
    {
      if (UICommand & b)
      {
        /* UI command found. */

        /* Acknowledge that UI Command has been received. */
        UICommand &= ~b;

        /* Return immediately once the first UI command bit has been found. */
        return b;
      }
    }
  }

  /* No command found. */
  return 0x0000;
}

static int8_t PD_GetFaultIndex(uint8_t FaultCode)
{
  /* Error codes on display are zero-indexed. */
  int8_t Index = 0;

  if (FaultCode == MC_NO_FAULTS)
  {
    return -1;
  }
  else
  {
    for (uint8_t b = 0x01; b <= 0x80; b <<= 1)
    {
      if (FaultCode & b)
      {
        /* Return immediately once the first fault code has been found. */
        return Index;
      }
      else
      {
        /* Increment error code index. */
        Index++;
      }
    }
  }
  return -1;
}

/**
 * @brief  Runs product state machine.
 */
static void PD_RunStateMachine(void)
{
  uint16_t CMD = PD_UICommandScheduler();

  if (MCI_IsBatteryPercentageInitialised() == true)
  {
    BatteryDisplayPercent = MCI_GetBatteryPercentage();
    /// TODO: could change to UART commands in future.
    if (BatteryDisplayPercent == (uint8_t)100)
    {
      DBG_MSG("Battery is full.\n");
      ChargingFinished = true;
    }
    else
    {
      ChargingFinished = false;
    }

#ifndef DISABLE_BATTERY_EMPTY
    if (BatteryDisplayPercent == (uint8_t)0)
    {
      DBG_MSG("Battery is empty.\n");
      BatteryIsEmpty = true;
    }
    else
    {
      BatteryIsEmpty = false;
    }
#endif

    PD_SetBatteryLevel(BatteryDisplayPercent);
  }
  else
  {
    BatteryDisplayPercent = -1;
  }

  switch (Product.State.Now)
  {
  case P_IDLE:
  {
    if (Product.State.Configured == false)
    {
      MCI_StopMotor();
      Product.ErrorCodeIndex = -1;
      MCI_AcknowledgeFaults();
      Product.State.Configured = true;
      SPEED1_LED_OFF;
      SPEED2_LED_OFF;
      SPEED3_LED_OFF;
      TURBOL_LED_OFF;

      LEDFlashState = false;
      LEDFlashCounter = 0;
      Product.Mode = PRODUCT_MODE_NORMAL;
      Product.NewMode = PRODUCT_MODE_NORMAL;

      BatteryFullLEDCounter = BATT_FULL_LIGHT_OFF_DURATION_TICKS;

      if (MCI_IsBatteryDetected() == true)
      {
        if (MCI_IsCablePlugged() == true)
        {
          if (ChargingFinished == true)
          {
            DBG_MSG("Charging finished, Turning Off LEDs\n");
#if USE_NEW_LED_LIBRARY
            /* Turn off LED as charging is done. */
            if (LED_Transition_IsLEDOff(&LEDTransition) == false)
            {
              LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
            }
#else 
            /* Turn off LED as charging is done. */
            RGBLED_ClearBufferedCommands(&MainLED);
            RGBLED_Command_Off(&MainLED, true);
            WHITE_LED_OFF;
#endif 
          }
          else
          {
            /* Do nothing, leave charging LED logic in other state machine. */
          }
        }

        // Cable not plugged 
        else
        {
          DBG_MSG("Cable not plugged, Turning Off LEDs\n");
#if USE_NEW_LED_LIBRARY
          /* If cable not plugged, turn off. */
          if (LED_Transition_IsLEDOff(&LEDTransition) == false)
          {
            LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
          }
#else 
          /* If cable not plugged, turn off. */
          RGBLED_ClearBufferedCommands(&MainLED);
          RGBLED_Command_Off(&MainLED, true);
          WHITE_LED_OFF;
#endif
        }
      }
      // not battery detected 
      else
      {
        DBG_MSG("No battery detected, Turning Off LEDs\n");
#if USE_NEW_LED_LIBRARY
      /* If no battery detected, turn off. */
      if (LED_Transition_IsLEDOff(&LEDTransition) == false)
      {
        LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS); // Turn off LED.
      }
#else 
        /* If no battery detected, turn off. */
        RGBLED_ClearBufferedCommands(&MainLED);
        RGBLED_Command_Off(&MainLED, true);
        WHITE_LED_OFF;
#endif 
      }
    }

    if (Mci.CablePluggedPrev != Mci.CablePlugged)
    {
      /* Reset sleep ticks whenever cable is unplugged or replugged. */
      Product.State.Counter = SLEEP_TICKS;
    }
    Mci.CablePluggedPrev = Mci.CablePlugged;

    if (Mci.BatteryDetectedPrev != Mci.BatteryDetected)
    {
      /* Reset sleep ticks whenever battery is unplugged or replugged. */
      Product.State.Counter = SLEEP_TICKS;
    }
    Mci.BatteryDetectedPrev = Mci.BatteryDetected;

#ifdef ENABLE_POWER_DISPLAY
    if (CMD == UI_BTN_SHORT_1 && SinglePressTimeExceeded != true)
    {
      /* Short button press detected. */
      Product.State = (ProductState_Handle_t){P_POWER_DISPLAY, Product.State.Now, POWER_DISPLAY_TICKS, false};
      break; /* Break immediately so as not to run other code below. */
    }
    else
    {
    }
#endif
    if (CMD != UI_NO_CMD)
    {
      /* Any UI command will reset sleep timer. */
      Product.State.Counter = SLEEP_TICKS;

      if (CMD == UI_TOUCH_PWR_LONG)
      {
        if (PD_CheckErrorCodes() == true)
        {
          /* Device has fault, move to fault state. */
          Product.State = (ProductState_Handle_t){P_FAULT_NOW, Product.State.Now, 0, false};
        }
        else if (MCI_IsBatteryDetected() == true && BatteryIsEmpty == true)
        {
          /* Battery is empty, move to empty state. */
          Product.State = (ProductState_Handle_t){P_BATT_EMPTY, Product.State.Now, BATT_EMPTY_WAIT_TICKS, false};
        }
        else
        {
          DBG_MSG("Long button press detected, moving to P_START\n");
          /* Long button press recieved. Prepare to move to P_START if requirements are met. */
          Product.State = (ProductState_Handle_t){P_START, Product.State.Now, 0, false};
        }
        SleepTriggered = false;
        break; /* break immediately. */
      }
      else if (CMD == UI_TOUCH_PWR_SHORT)
      {
        if (Mci.BatteryDetectedReceived != true)
        {
          /* Not received information from motor yet, delay command to next cycle. */
          UICommand |= UI_TOUCH_PWR_SHORT;
        }
        else
        {
          if (MCI_IsBatteryDetected() == true)
          {
            /* Show battery life on LED. */
            ShowBatteryLife = true;
            ShowBatteryLifeConfigured = false;
            DBG_MSG("Short button press detected, showing battery life\n");
          }
          else
          {
            DBG_MSG("Short button press detected, but no battery detected\n");
          }
        }
      }
    }
    else
    {
#ifndef DISABLE_SLEEP_MODE
      /* No UI command, check if product should enter sleep state. */
      /* If state counter reaches zero, product goes to sleep. */
      if (Product.State.Counter == 0)
      {
        if (MCI_IsBatteryDetected() == true)
        {
          if (MCI_IsCablePlugged() == true)
          {
            if (ChargingFinished == true)
            {
              /// TODO: logic for going to sleep when battery is full should be implemented.
              ///       would need to wake up periodically to check if trickle charge required
              ///       and then go back to sleep after full again. ALl this would happen without 
              ///       giving any indication to the user.
            }
            else
            {
              /* Do nothing, still charging the battery. */
            }
          }
          else /* Cable not plugged. */
          {
            /* Go to sleep. */
            PD_ClearAllBufferedCommands();
            Product.State = (ProductState_Handle_t){P_SLEEP, Product.State.Now, 0, false};
            break; /* break immediately. */
          }
        }
        else /* No battery. */
        {
#ifndef DISABLE_SLEEP_WHEN_CABLE_PLUGGED_IN
          /* Go to sleep. */
          PD_ClearAllBufferedCommands();
          Product.State = (ProductState_Handle_t){P_SLEEP, Product.State.Now, 0, false};
          break; /* break immediately. */
#endif
        }
      }
#endif
    }
  }
  break;

  case P_START:
  {
    if (MCI_StartMotor() == true)
    {
      ShowBatteryLife = false;
      SendDefaultLEDCommand = true;

#if USE_NEW_LED_LIBRARY
      if (LED_Transition_IsLEDOff(&LEDTransition))
#else 
      if (MainLED.State.Now == RGBLED_OFF)
#endif
      {
        DBG_MSG("P_Start, LED is off, setting LED color\n");

        if (MCI_IsCablePlugged() == true)
        {
          if (ChargingFinished == true)
          {
            DBG_MSG("P_Start, Cable plugged, Charging finished, Setting Battery Charge Finished LED\n");
            #if USE_NEW_LED_LIBRARY
            LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryChargeFinished, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
            #else 
            RGBLED_Command_Solid(&MainLED, &LEDBatteryChargeFinished, true);
            #endif 

          }
          else
          {
            DBG_MSG("P_Start, Cable plugged, Charging, Setting Battery Charge LED\n");
            PD_SetBatteryLED_Charging(Product.BatteryLevel, true);
          }
        }
        else // Cable Not Plugged
        {
          DBG_MSG("P_Start, Cable not plugged, Setting LED Default Color \n");
#if USE_NEW_LED_LIBRARY
  #if USE_WHITE_LED
          if (LED_Transition_IsLEDOff(&LEDTransition) == false)
          {
              LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_IMMINENT, 0);
          }

          if (Product.BatteryLevel > BATTERY_LEVEL_1)
          {
            TurnOnWhiteLEDOnCompletion = true; // handled on LED Callback
          }
  #else
          LED_Transition_ToSolid(&LEDTransition, &LED_Solid_DefaultColor, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
  #endif 
#else 
          RGBLED_Command_Off(&MainLED, true);
          if (Product.BatteryLevel > BATTERY_LEVEL_1)
          {
            WHITE_LED_ON;
          }
#endif 
        }
      }
      else
      {
        /* LED is still on, retain state. */
      }

      DBG_MSG("P_Start, Motor started, moving to P_ON\n");
      Mci.CablePluggedPrev = Mci.CablePlugged;
      Mci.BatteryDetectedPrev = Mci.BatteryDetected;

      BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_1, BUZZER_DEFAULT_TIMES, BUZZER_DURATION_TURN_ON_TICKS, 0);
      Product.State = (ProductState_Handle_t){P_ON, Product.State.Now, ON_TICKS, false};
    }
    else
    {
      /* Nothing to do. */
    }
  }
  break;

  case P_ON:
  {
    static bool BattLowStatePrev = false;
    bool BattLowState = false;
    uint8_t MotorFaults;

    if (Product.State.Configured != true)
    {
      BattEmptyTimerStarted = false;
      Product.State.Configured = true;
      Product.SpeedSetting = SPEED_SETTING_1;
      Product.PrevSpeedIndex = SPEED_SETTING_1;
      Mci.SpeedSetting = SPEED_SETTING_1;
      Mci.PrevMode = SPEED_SETTING_1;
    }

    if (Mci.BatteryDetectedPrev != Mci.BatteryDetected)
    {
      /* Battery has been removed or plugged in whilst in P_ON state, turn off. */
      (void)MCI_StopMotor();
      Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
      break; /* break immediately. */
    }
    Mci.BatteryDetectedPrev = Mci.BatteryDetected;

    if (MCI_IsBatteryDetected() == true)
    {
      if (BatteryIsEmpty == true)
      {
        DBG_MSG("P_ON, Battery detected, but empty, moving to P_IDLE\n");
        Product.State = (ProductState_Handle_t){P_BATT_EMPTY, Product.State.Now, BATT_EMPTY_WAIT_TICKS, false};
        (void)MCI_StopMotor();
        break; /* break immediately. */
      }
    }

    if (PD_CheckErrorCodes() == true)
    {
      DBG_MSG("P_ON, Error codes detected, moving to P_FAULT_NOW\n");
      PD_ClearAllBufferedCommands();
      Product.State = (ProductState_Handle_t){P_FAULT_NOW, Product.State.Now, 0, false};
      break; /* break immediately. */
    }

    if (CMD != UI_NO_CMD || MCI_IsCablePlugged() == true)
    {
      /* Reset product auto-shutdown timer on any UI interaction
         or if the cable is plugged in. */
      Product.State.Counter = ON_TICKS;
    }

    if (Mci.CablePluggedPrev == false)
    {
      if (Mci.CablePlugged == true)
      {
        /* Cable has been plugged during on state, turn off immediately to protect power
           supply from overcurrent. */
        Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
        MCI_StopMotor();
        break;
      }
    }

    Mci.CablePluggedPrev = Mci.CablePlugged;

    if (MCI_IsBatteryDetected() == true)
    {
      if (Product.BatteryLevel == BATTERY_LEVEL_1)
      {
        if (ShowBatteryLife == false)
        {
          ShowBatteryLife = true;
          ShowBatteryLifeConfigured = false;
        }

        /* Keep the flashing time uninterrupted */
        if (ShowBatteryLifeTimer <= BATT_DISPLAY_COUNTDOWN_TO_MINIMUM_VALUE)
        {
          ShowBatteryLifeTimer = BATT_DISPLAY_DURATION_TICKS;
        }
      }
    }

    if (CMD == UI_TOUCH_PWR_LONG || Product.State.Counter == 0)
    {
      if (CMD == UI_TOUCH_PWR_LONG)
      {
        /* Only buzz if it was user interaction. */
        BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_1, BUZZER_DEFAULT_TIMES, BUZZER_DURATION_TURN_OFF_TICKS, 0);
      }

      MCI_StopMotor();

      Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
      DBG_MSG("P_ON, Long button press detected, moving to P_IDLE\n");
    }
    else if (CMD == UI_TOUCH_PWR_SHORT)
    {
      if ((MCI_IsBatteryDetected() == true) && (Product.BatteryLevel != BATTERY_LEVEL_1))
      {
        /* Show battery life for 3 seconds. */
        ShowBatteryLife = true;
        ShowBatteryLifeConfigured = false;
        DBG_MSG("P_ON, Short button press detected, showing battery life\n");
      }
    }
    else if (CMD == UI_TOUCH_SPEED1_SHORT)
    {
      Product.SpeedSetting = SPEED_SETTING_1;
      BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_1, BUZZER_DEFAULT_TIMES, BUZZER_DURATION_SPEED_CHANGE_ON_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_SPEED1_LONG)
    {
      Product.SpeedSetting = SPEED_SETTING_1;
      Product.NewMode = !Product.Mode; /* Alternate between the two modes. */
      BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_1, MODE_CHANGE_BEEP_TIMES, MODE_CHANGE_BEEP_DURATION_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_SPEED2_SHORT)
    {
      Product.SpeedSetting = SPEED_SETTING_2;
      BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_2, BUZZER_DEFAULT_TIMES, BUZZER_DURATION_SPEED_CHANGE_ON_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_SPEED2_LONG)
    {
      Product.SpeedSetting = SPEED_SETTING_2;
      Product.NewMode = !Product.Mode; /* Alternate between the two modes. */
      BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_2, MODE_CHANGE_BEEP_TIMES, MODE_CHANGE_BEEP_DURATION_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_SPEED3_SHORT)
    {
      Product.SpeedSetting = SPEED_SETTING_3;
      BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_3, BUZZER_DEFAULT_TIMES, BUZZER_DURATION_SPEED_CHANGE_ON_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_SPEED3_LONG)
    {
      Product.SpeedSetting = SPEED_SETTING_3;
      Product.NewMode = !Product.Mode; /* Alternate between the two modes. */
      BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_3, MODE_CHANGE_BEEP_TIMES, MODE_CHANGE_BEEP_DURATION_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_TURBO_SHORT)
    {
      Product.SpeedSetting = SPEED_SETTING_TURBO;
      BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_3, BUZZER_TURBO_TIMES, BUZZER_DURATION_SPEED_CHANGE_ON_TICKS, BUZZER_DURATION_SPEED_CHANGE_TURBO_OFF_TICKS);
    }
    else if (CMD == UI_TOUCH_TURBO_LONG)
    {
      Product.SpeedSetting = SPEED_SETTING_TURBO;
      Product.NewMode = !Product.Mode; /* Alternate between the two modes. */
      BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_3, BUZZER_TURBO_TIMES, MODE_CHANGE_BEEP_DURATION_TICKS, BUZZER_DURATION_SPEED_CHANGE_TURBO_OFF_TIME);
    }
    else
    {
      /* Do nothing */
    }

    PD_RunSpeedChangeIndication(Product.SpeedSetting);
    MCI_SetSpeedSetting(Product.SpeedSetting);

    switch (Product.Mode)
    {

    case PRODUCT_MODE_NORMAL:
    {
      if (Product.NewMode == PRODUCT_MODE_TIMER)
      {
        /* Move to timer mode. */
        Product.Mode = PRODUCT_MODE_TIMER;
        Product.TimerModeCountdown = TIMER_MODE_DURATION_TICKS;
      }
    }
    break;

    case PRODUCT_MODE_TIMER:
    {
      if (Product.TimerModeCountdown > 0)
      {
        Product.TimerModeCountdown--;

        if (Product.NewMode == PRODUCT_MODE_NORMAL)
        {
          /* Move to normal mode. */
          Product.Mode = PRODUCT_MODE_NORMAL;
        }
      }
      else
      {
        /* Timer finished, shutdown product. */
        MCI_StopMotor();
        Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
        break;
      }
    }
    break;

    default:
      break;
    }
  }
  break;

  case P_SLEEP:
  {
    if (SleepTriggered == true)
    {
      if (CMD != UI_NO_CMD)
      {
        /* Force boot */
        SleepTriggered = false;
        UICommand |= CMD;
        Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
        break;
      }
    }
    else
    {
#ifdef DISABLE_SLEEP_MODE
      if (CMD != UI_NO_CMD)
      {
        /* Load another command so it can be properly dealt with in P_IDLE state. */
        UICommand |= CMD;
        Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
        break;
      }
#else
      SleepTriggered = true;
#endif
    }
  }
  break;

  case P_BATT_EMPTY:
  {

    if (Product.State.Configured == false)
    {
      int8_t FlashTimes;
      uint32_t FlashWaitTime;

      /* Turn all LEDs off. */
      PD_AllLEDsOff();

      FlashWaitTime = BATT_EMPTY_WAIT_TIME;

      #if USE_NEW_LED_LIBRARY

      /* Display fault code on display. */
      if (LED_Transition_IsBusy(&LEDTransition))
      {
        DBG_MSG("P_FAULT_NOW, LED Transition is busy, stopping\n");
        LED_Transition_Stop(&LEDTransition);
      }

      LED_Transition_ToFlash(&LEDTransition, &LED_Flash_BatteryEmpty, LED_TRANSITION_IMMINENT, 0);
      #else 
      FlashTimes = BATT_EMPTY_FLASH_TIMES;
      BattEmptyFlash.Times = FlashTimes;
      RGBLED_Command_Flash(&MainLED, &BattEmptyFlash, true);
      #endif 

      BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_1, BATT_EMPTY_FLASH_TIMES, BATT_EMPTY_FLASH_TICKS, BATT_EMPTY_FLASH_TICKS);

      Product.State.Counter = FlashWaitTime;
      Product.State.Configured = true;
    }

    if (Product.State.Counter == 0)
    {
      Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
    }
  }
  break;

  case P_FAULT_NOW:
  {
    if (Product.State.Configured == false)
    {
      PD_AllLEDsOff();
      uint8_t Flashes = Product.ErrorCodeIndex + (BATT_EMPTY_FLASH_TIMES + 1); /* Start error flashes at 1 more than batt empty flashes. */

      /* Set buzzer to same configuration as error flash. */
      BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_1, Flashes, LED_RGYW_ERROR_FLASH_INTERVAL_TICKS, LED_RGYW_ERROR_FLASH_INTERVAL_TICKS);

      DBG_MSG("P_FAULT_NOW, Showing LED Error Animation\n");
#if USE_NEW_LED_LIBRARY

      LED_Flash_Error.repeatTimes = Flashes;
      
      /* Display fault code on display. */
      if (LED_Transition_IsBusy(&LEDTransition))
      {
        DBG_MSG("P_FAULT_NOW, LED Transition is busy, stopping\n");
        LED_Transition_Stop(&LEDTransition);
      }

      LED_Transition_ToFlash(&LEDTransition, &LED_Flash_Error, LED_TRANSITION_IMMINENT, 0);      
#else
      /* Dispay fault code on display. */
      LEDErrorFlash.Times = Flashes;
      RGBLED_Command_Flash(&MainLED, &LEDErrorFlash, true);
#endif

      Product.State.Configured = true;
    }

    MCI_AcknowledgeFaults();
    Product.State = (ProductState_Handle_t){P_FAULT_ACKNOWLEDGE, Product.State.Now, FAULT_ACK_TIMEOUT_TICKS, false};
    DBG_MSG("P_FAULT_NOW, Error code detected, moving to P_FAULT_ACKNOWLEDGE\n");

    break;
  }

  case P_FAULT_ACKNOWLEDGE:
  {
    if (Product.State.Counter == 0)
    {
      MCI_ResetFaultState();
      Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
      DBG_MSG("P_FAULT_ACKNOWLEDGE, Moving to P_IDLE\n");
    }
    break;
  }

  default:
  {
    break;
  }
  }
}

static void PD_AllLEDsOff(void)
{
  WHITE_LED_OFF;
  SPEED1_LED_OFF;
  SPEED2_LED_OFF;
  SPEED3_LED_OFF;
  TURBOL_LED_OFF;

  #if USE_NEW_LED_LIBRARY
  if (LED_Transition_IsLEDOff(&LEDTransition) == false)
  {
    LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
  }
  #else 
  RGBLED_Command_Off(&MainLED, true);
  RGBLED_ClearBufferedCommands(&MainLED);
  #endif
}

void PD_RunSpeedChangeIndication(uint8_t Index)
{
  switch (Product.Mode)
  {
  case PRODUCT_MODE_NORMAL:
  {
    switch (Index)
    {
    case SPEED_SETTING_1:
    {
      SPEED1_LED_ON;
      SPEED2_LED_OFF;
      SPEED3_LED_OFF;
      TURBOL_LED_OFF;
    }
    break;

    case SPEED_SETTING_2:
    {
      SPEED1_LED_ON;
      SPEED2_LED_ON;
      SPEED3_LED_OFF;
      TURBOL_LED_OFF;
    }
    break;

    case SPEED_SETTING_3:
    {
      SPEED1_LED_ON;
      SPEED2_LED_ON;
      SPEED3_LED_ON;
      TURBOL_LED_OFF;
    }
    break;

    case SPEED_SETTING_TURBO:
    {
      SPEED1_LED_ON;
      SPEED2_LED_ON;
      SPEED3_LED_ON;
      TURBOL_LED_ON;
    }
    break;

    default:
      break;
    }
  }
  break;

  case PRODUCT_MODE_TIMER:
  {
    if (LEDFlashCounter > 0)
    {
      LEDFlashCounter--;
    }
    else
    {
      LEDFlashState = !LEDFlashState;
      if (LEDFlashState == true)
      {
        LEDFlashCounter = SPEED_LED_FLASH_DURATION_ON_TICKS;
      }
      else
      {
        LEDFlashCounter = SPEED_LED_FLASH_DURATION_OFF_TICKS;
      }
    }

    switch (Index)
    {
    case SPEED_SETTING_1:
    {
      if (LEDFlashState)
      {
        SPEED1_LED_ON;
      }
      else
      {
        SPEED1_LED_OFF;
      }
      SPEED2_LED_OFF;
      SPEED3_LED_OFF;
      TURBOL_LED_OFF;
    }
    break;

    case SPEED_SETTING_2:
    {
      SPEED1_LED_ON;
      if (LEDFlashState)
      {
        SPEED2_LED_ON;
      }
      else
      {
        SPEED2_LED_OFF;
      }
      SPEED3_LED_OFF;
      TURBOL_LED_OFF;
    }
    break;

    case SPEED_SETTING_3:
    {
      SPEED1_LED_ON;
      SPEED2_LED_ON;
      if (LEDFlashState)
      {
        SPEED3_LED_ON;
      }
      else
      {
        SPEED3_LED_OFF;
      }
      TURBOL_LED_OFF;
    }
    break;

    case SPEED_SETTING_TURBO:
    {
      SPEED1_LED_ON;
      SPEED2_LED_ON;
      SPEED3_LED_ON;
      if (LEDFlashState)
      {
        TURBOL_LED_ON;
      }
      else
      {
        TURBOL_LED_OFF;
      }
    }
    break;

    default:
      break;
    }
  }
  break;

  default:
    break;
  }

  Product.PrevSpeedIndex = Index;
}

static void PD_RunPowerModeStateMachine(void)
{
  if (Mci.BatteryDetectedReceived != true)
  {
    /* Don't run state machine if not recevied battery info yet. */
    return;
  }

  if (MCI_IsBatteryDetected() == true)
  {
    if (MCI_IsCablePlugged() == true)
    {
      /* Assume it's charging. */
      BatteryIsCharging = true;
      SendDefaultLEDCommand = true;
      // ShowBatteryLife = false;

      switch (CableInSubState)
      {
      case P_CABLEIN_IDLE:
      {
        if (ChargingFinished == true)
        {
          CableInSubState = P_CABLEIN_FINISHED;
          BatteryFullLEDCounter = BATT_FULL_LIGHT_OFF_DURATION_TICKS;
          DBG_MSG("P_CABLEIN_IDLE, Charging finished, moving to P_CABLEIN_FINISHED\n");
          
          DBG_MSG("P_CABLEIN_IDLE, Displaying LED Battery Charge Finished \n");
          #if USE_NEW_LED_LIBRARY
          LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryChargeFinished, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
          #else 
          RGBLED_Command_Solid(&MainLED, &LEDBatteryChargeFinished, true);
          #endif

        }
        else /* Charging ongoing */
        {
          if (BatteryIsCharging == true)
          {
            CableInSubState = P_CABLEIN_CHARGING;
            DBG_MSG("P_CABLEIN_IDLE, Charging, moving to P_CABLEIN_CHARGING\n");
            DBG_MSG("P_CABLEIN_IDLE, Displaying LED Battery Charging \n");
            PD_SetBatteryLED_Charging(Product.BatteryLevel, true);
          }
        }
      }
      break;

      case P_CABLEIN_CHARGING:
      {
        #if USE_NEW_LED_LIBRARY
        if (LED_Transition_IsLEDOff(&LEDTransition))
        {
          /* If LED is off for any reason, force set it. */
          PD_SetBatteryLED_Charging(Product.BatteryLevel, true);
        }
        #else 
        if (MainLED.State.Now == RGBLED_OFF)
        {
          /* If LED is off for any reason, force set it. */
          PD_SetBatteryLED_Charging(Product.BatteryLevel, true);
        }
        #endif 

        PD_SetBatteryLED_Charging(Product.BatteryLevel, false);

        if (ChargingFinished == true)
        {
          CableInSubState = P_CABLEIN_FINISHED;
          BatteryFullLEDCounter = BATT_FULL_LIGHT_OFF_DURATION_TICKS;
          DBG_MSG("P_CABLEIN_CHARGING, Charging finished, moving to P_CABLEIN_FINISHED\n");
          DBG_MSG("P_CABLEIN_CHARGING, Displaying LED Battery Charge Finished \n");
          #if USE_NEW_LED_LIBRARY
          LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryChargeFinished, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
          #else 
          RGBLED_Command_Solid(&MainLED, &LEDBatteryChargeFinished, false);
          #endif 

        }
        else
        {
          if (BatteryIsCharging == true)
          {
            /* Do nothing. */
          }
          else
          { /* This will never happen as BatteryIsCharging is
               hard-coded based on cable state. */
            CableInSubState = P_CABLEIN_IDLE;
#if USE_NEW_LED_LIBRARY
            // transition to off if not already off
            if (LED_Transition_IsLEDOff(&LEDTransition) == false)
            {
              LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
            }
#else
            RGBLED_Command_Off(&MainLED, false);
            WHITE_LED_OFF;
#endif 

          }
        }
      }
      break;

      case P_CABLEIN_FINISHED:
      {
        if (ChargingFinished == true)
        {
          /* Do nothing. */
          if (Product.State.Now == P_ON)
          {
            DBG_MSG("P_CABLEIN_FINISHED, P_ON do nothing\n");
            /* Do nothing. */
          }
          else
          {
            if (ShowBatteryLife == true)
            {
              if (ShowBatteryLifeConfigured != true)
              {
                /* Force one command. */
                DBG_MSG("P_CABLEIN_FINISHED, Show Battery life, Displaying LED Discharging \n");
                PD_SetBatteryLED_Discharging(Product.BatteryLevel, true);
                ShowBatteryLifeTimer = BATT_DISPLAY_DURATION_TICKS;
                ShowBatteryLifeConfigured = true;
              }

              if (ShowBatteryLifeTimer > 0)
              {
                ShowBatteryLifeTimer--;
              }
              else
              {
                DBG_MSG("P_CABLEIN_FINISHED, Show Battery life time elapsed, Turning Off LED\n");
                /* Turn back to off. */
#if USE_NEW_LED_LIBRARY
                if (LED_Transition_IsLEDOff(&LEDTransition) == false)
                {
                  LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);

                }
#else 
                RGBLED_Command_Off(&MainLED, true);
                WHITE_LED_OFF;
#endif 

                ShowBatteryLife = false;
                BatteryFullLEDCounter = 0;
              }
            } 
            else /* ShowBatteryLife == false */
            {
              /* When product turned off, should turn off LED after duration. */
              if (BatteryFullLEDCounter > 0)
              {
                BatteryFullLEDCounter--;
              }
              else
              {
                DBG_MSG("P_CABLEIN_FINISHED, Battery Full LED time elapsed, Turning Off LED\n");
                #if USE_NEW_LED_LIBRARY
                /* Turn it off. */
                if (LED_Transition_IsLEDOff(&LEDTransition) == false)
                {
                  LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
                }
                #else 
                /* Turn it off. */
                if (MainLED.State.Now != RGBLED_OFF)
                {
                  RGBLED_Command_Off(&MainLED, true);
                  WHITE_LED_OFF;
                }
                #endif 
                
              }
            }
          }
        }
        else
        {
          DBG_MSG("P_CABLEIN_FINISHED, Charging not finished, Displaying LED Charging \n");
          PD_SetBatteryLED_Charging(Product.BatteryLevel, true);
          CableInSubState = P_CABLEIN_CHARGING;
        }
      }
      break;

      default:
        break;
      }
    }
    else /* Cable not plugged in. */
    {
      CableInSubState = P_CABLEIN_IDLE;
      BatteryIsCharging = false;
      if (ShowBatteryLife == true)
      {
        if (ShowBatteryLifeConfigured != true)
        {
          DBG_MSG("P_CABLEIN, Cable not plugged in, Show Battery life, Displaying LED Discharging \n");
          /* Force one command. */
          PD_SetBatteryLED_Discharging(Product.BatteryLevel, true);
          ShowBatteryLifeTimer = BATT_DISPLAY_DURATION_TICKS;
          ShowBatteryLifeConfigured = true;
        }

        if (ShowBatteryLifeTimer > 0)
        {
          ShowBatteryLifeTimer--;
        }
        else
        {
          if (Product.State.Now == P_ON || Product.State.Now == P_START)
          {
            DBG_MSG("P_CABLEIN, Cable not plugged in, Show Battery life time elapsed\n");
            DBG_MSG("P_CABLEIN, Cable not plugged in, LED display default color\n");
#if USE_NEW_LED_LIBRARY
  #if USE_WHITE_LED

            // if LED not off then turn it off 
            if (LED_Transition_IsLEDOff(&LEDTransition) == false)
            {
              LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
            }
            TurnOnWhiteLEDOnCompletion = true; // handled on LED Callback
  #else
            LED_Transition_ToSolid(&LEDTransition, &LED_Solid_DefaultColor, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
  #endif 
#else 
            /* Turn back to white. */
            RGBLED_Command_Off(&MainLED, true);
            WHITE_LED_ON;
#endif 
          }
          else /*Product State is not ON */
          {
            DBG_MSG("P_CABLEIN, Cable not plugged in, Show Battery life time elapsed, Turning Off LED\n");
#if USE_NEW_LED_LIBRARY

            if (LED_Transition_IsLEDOff(&LEDTransition) == false)
            {
              LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
            }
#else
            RGBLED_Command_Off(&MainLED, true);
            WHITE_LED_OFF;
#endif 
          }
          ShowBatteryLife = false;
        }
      }
      else /*ShowBatteryLife == false */
      {
        if (Product.State.Now == P_ON || Product.State.Now == P_START)
        {
          if (SendDefaultLEDCommand == true)
          {
            DBG_MSG("P_CABLEIN, Cable not plugged in, ShowBatteryLife false,  LED display default color\n");
#if USE_NEW_LED_LIBRARY
  #if USE_WHITE_LED
            if (LED_Transition_IsLEDOff(&LEDTransition) == false) 
            {
              LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
            }
            TurnOnWhiteLEDOnCompletion = true; // handled on LED Callback
  #else
            LED_Transition_ToSolid(&LEDTransition, &LED_Solid_DefaultColor, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
  #endif 
#else 
            /// TODO: calling this LED command on repeat causes MCU data in Ozone to crash
            /// Also makes MCU not be able to be flashed. Have to do a power cycle.
            RGBLED_Command_Off(&MainLED, true);
            WHITE_LED_ON;
#endif 
            SendDefaultLEDCommand = false;
          }
        }
        else
        {
          if (Product.State.Now == P_BATT_EMPTY || 
              Product.State.Now == P_FAULT_NOW || 
              Product.State.Now == P_FAULT_ACKNOWLEDGE)
          {
            /* Do nothing, LED controlled from those states. */
          }
          else
          {
            /* In any other state, LED should be off. */
            // DBG_MSG("P_CABLEIN, Cable not plugged in, ShowBatteryLife false,  LED display off\n");
#if USE_NEW_LED_LIBRARY
            // if LED not off then turn it off
            if (LED_Transition_IsLEDOff(&LEDTransition) == false)
            {
              LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
            }
#else 
            if (MainLED.State.Now != RGBLED_OFF)
            {
              RGBLED_Command_Off(&MainLED, true);
              WHITE_LED_OFF;
            }
#endif 
          }
        }
      }
    }
  }
  else /* No battery detected. */
  {
    CableInSubState = P_CABLEIN_IDLE;
    BatteryIsCharging = false;
    if (Product.State.Now == P_ON)
    {
#if USE_NEW_LED_LIBRARY
  #if USE_WHITE_LED
      if (LED_Transition_IsLEDOff(&LEDTransition) == false)
      {
        LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_IMMINENT, 0);
      }
      TurnOnWhiteLEDOnCompletion = true; // handled on LED Callback
  #endif 
#else 
      RGBLED_Command_Off(&MainLED, true);
      WHITE_LED_ON;
#endif

    }
    else
    {
      if (Product.State.Now == P_IDLE)
      {
        /* Turn RGB LED off. 
           This situation can occur if battery was charging and 
           then the battery is removed. */
        #if USE_NEW_LED_LIBRARY

        if (!LED_Transition_IsLEDOff(&LEDTransition))
        {
          LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
        }
        
        #else 
        if (MainLED.State.Now != RGBLED_OFF)
        {
          /// TODO: calling this LED command on repeat causes MCU data in Ozone to crash
          /// Also makes MCU not be able to be flashed. Have to do a power cycle.
          RGBLED_Command_Off(&MainLED, true);
          WHITE_LED_OFF;
        }
        #endif 
      }
    }
  }
}

void PD_RunWaitForSinglePress(void)
{
  if (WaitForSinglePressTimeout > 1)
  {
    WaitForSinglePressTimeout--;
  }
  else if (WaitForSinglePressTimeout == 1)
  {
    SinglePressTimeExceeded = true;
    WaitForSinglePressTimeout--;
  }
  else
  {
    WaitForSinglePressTimeout = SINGLE_BUTTON_PRESS_TIMEOUT_TICKS;
  }
}

void PD_UpdateCommunicationFaultStatus(bool IsFault)
{
  CommunicationFaultOccurred = IsFault;
  if (CommunicationFaultOccurred == true)
  {
    MCI_Reset();
  }
}

bool PD_CheckErrorCodes(void)
{
  uint8_t MotorFaults;

  MotorFaults = MCI_GetNackedFaults();

#ifndef DISABLE_COMM_ERROR
  if (CommunicationFaultOccurred == true)
  {
    MotorFaults |= MC_FAULT_COMM;
  }
#endif

  Product.ErrorCodeIndex = PD_GetFaultIndex(MotorFaults);

  if (Product.ErrorCodeIndex != -1)
  {
    return true;
  }

  return false;
}

void PD_ProcessTouchButtons(void)
{
  bool BtnPwrPressed = false;
  bool BtnSpeed1Pressed = false;
  bool BtnSpeed2Pressed = false;
  bool BtnSpeed3Pressed = false;
  bool BtnTurboPressed = false;

  BtnPwrPressed = !GPIO_ReadInputDataBit(POWER_BUTTON_PORT, POWER_BUTTON_PIN);

  switch (Product.TouchID)
  {
  case TOUCH_PWR:
  {
    BtnPwrPressed = true;
  }
  break;

  case TOUCH_SPEED1:
  {
    BtnSpeed1Pressed = true;
  }
  break;

  case TOUCH_SPEED2:
  {
    BtnSpeed2Pressed = true;
  }
  break;

  case TOUCH_SPEED3:
  {
    BtnSpeed3Pressed = true;
  }
  break;

  case TOUCH_TURBO:
  {
    BtnTurboPressed = true;
  }
  break;

  default:
    break;
  }

  /* Update buttons, so we can handle short presses and long presses. */
  BTN_Update(&BtnPwr, BtnPwrPressed);
  BTN_Update(&BtnSpeed1, BtnSpeed1Pressed);
  BTN_Update(&BtnSpeed2, BtnSpeed2Pressed);
  BTN_Update(&BtnSpeed3, BtnSpeed3Pressed);
  BTN_Update(&BtnTurbo, BtnTurboPressed);
}

void PD_GetTouchID(uint8_t TouchID)
{
  Product.TouchID = TouchID;
}

static void PD_SetBatteryLevel(uint8_t Percentage)
{
  BATTLevel_State_t Level;

  if (Percentage == 0)
  {
    Level = BATTERY_LEVEL_EMPTY;
  }
  else if (Percentage < BATT_LEVEL1_THRESHOLD)
  {
    Level = BATTERY_LEVEL_1;
  }
  else if (Percentage < BATT_LEVEL2_THRESHOLD)
  {
    Level = BATTERY_LEVEL_2;
  }
  else if (Percentage < BATT_LEVEL3_THRESHOLD)
  {
    Level = BATTERY_LEVEL_3;
  }
  else if (Percentage < BATT_LEVEL4_THRESHOLD)
  {
    Level = BATTERY_LEVEL_4;
  }
  else
  {
    Level = BATTERY_LEVEL_FULL;
  }

  Product.BatteryLevel = Level;
}

/* Note: since all states are either solid or flashing, we could set Force = true
   or Force = false for all of the RGBLED function calls. This is because the LED is
   either on or off so the transition can be forced between each state. However, setting
   Force = false is likely better as if there is an LED error flash occuring, it will wait
   until this is finished before starting the new LED command. */
static void PD_SetBatteryLED_Discharging(BATTLevel_State_t Level, bool ForceSendCmd)
{
  /* If level has changed or force is set to true, update LED command. */
  if (PreviousBatteryLevel != Level || ForceSendCmd == true)
  {
    WHITE_LED_OFF;
    switch (Level)
    {
    case BATTERY_LEVEL_EMPTY:
    case BATTERY_LEVEL_1:
    {
#if USE_NEW_LED_LIBRARY
      /* Display fault code on display. */
      if (LED_Transition_IsBusy(&LEDTransition))
      {
        DBG_MSG("P_FAULT_NOW, LED Transition is busy, stopping\n");
        LED_Transition_Stop(&LEDTransition);
      }

      LED_Transition_ToFlash(&LEDTransition, &LED_Flash_BatteryVeryLow, LED_TRANSITION_IMMINENT, 0);
#else 
      RGBLED_Command_Flash(&MainLED, &LEDFlash_BatteryVeryLow, false);
#endif 
    }
    break;
    
    case BATTERY_LEVEL_2:
    {
#if USE_NEW_LED_LIBRARY
      LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryLow, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
#else 
      RGBLED_Command_Solid(&MainLED, &LEDSolid_BatteryLow, false);
#endif 

    }
    break;
    
    case BATTERY_LEVEL_3:
    {
#if USE_NEW_LED_LIBRARY
      LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryMid, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
#else 
      RGBLED_Command_Solid(&MainLED, &LEDSolid_BatteryMid, false);
#endif         
    }
    break;
    
    case BATTERY_LEVEL_4:
    {
#if USE_NEW_LED_LIBRARY
      LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryHigh, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
#else 
      RGBLED_Command_Solid(&MainLED, &LEDSolid_BatteryHigh, false);
#endif 
    }
    break;

    case BATTERY_LEVEL_FULL:
    {
#if USE_NEW_LED_LIBRARY
      LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryHigh, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
#else 
      RGBLED_Command_Solid(&MainLED, &LEDSolid_BatteryHigh, false);
#endif 
    }
    break;

    default:
#if USE_NEW_LED_LIBRARY
      if (LED_Transition_IsLEDOff(&LEDTransition) == false)
      {
        LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS); // Turn off LED.
      }
#else 
      RGBLED_Command_Off(&MainLED, true);
      WHITE_LED_OFF;
#endif 

      break;
    }
  }
  else
  {
    /* No change in level, do nothing. */
  }

  PreviousBatteryLevel = Level;
}

/* Note: Since breath effect is a smooth light transition, we never have Force = true
   for the RGBLED_Command_Breath() function call. This ensures that when the battery level
   changes, the breath color transition only happens when the LED is off. */
static void PD_SetBatteryLED_Charging(BATTLevel_State_t Level, bool ForceSendCmd)
{
  /* If level has changed or force is set to true, update LED command. */
  if (PreviousBatteryLevel != Level || ForceSendCmd == true)
  {
    WHITE_LED_OFF;
    switch (Level)
    {
    case BATTERY_LEVEL_EMPTY:
    case BATTERY_LEVEL_1:
    {
#if USE_NEW_LED_LIBRARY
      LED_Transition_ExecuteWithMap(&LEDTransition, &LED_Pulse_BatteryCharge_LowAndVeryLow, LED_ANIMATION_TYPE_PULSE);
#else 
      RGBLED_Command_Breath(&MainLED, &LEDBatteryChargeBreath_LowandVeryLow, true);
#endif
    }
    break;
    
    case BATTERY_LEVEL_2:
    {
#if USE_NEW_LED_LIBRARY
      LED_Transition_ExecuteWithMap(&LEDTransition, &LED_Pulse_BatteryCharge_LowAndVeryLow, LED_ANIMATION_TYPE_PULSE);
#else 
      RGBLED_Command_Breath(&MainLED, &LEDBatteryChargeBreath_LowandVeryLow, true);
#endif        
    }
    break;
    
    case BATTERY_LEVEL_3:
    {
#if USE_NEW_LED_LIBRARY
      LED_Transition_ExecuteWithMap(&LEDTransition, &LED_Pulse_BatteryCharge_Mid, LED_ANIMATION_TYPE_PULSE);
#else 
      RGBLED_Command_Breath(&MainLED, &LEDBatteryChargeBreath_Mid, true);
#endif        

    }
    break;
    
    case BATTERY_LEVEL_4:
    {
#if USE_NEW_LED_LIBRARY
      LED_Transition_ExecuteWithMap(&LEDTransition, &LED_Pulse_BatteryCharge_High, LED_ANIMATION_TYPE_PULSE);
#else 
      RGBLED_Command_Breath(&MainLED, &LEDBatteryChargeBreath_High, true);      
#endif        

    }
    break;

    case BATTERY_LEVEL_FULL:
    {
#if USE_NEW_LED_LIBRARY
      LED_Transition_ExecuteWithMap(&LEDTransition, &LED_Solid_BatteryHigh, LED_ANIMATION_TYPE_SOLID);
#else 
      RGBLED_Command_Solid(&MainLED, &LEDSolid_BatteryHigh, true);
#endif        
    }
    break;

    default:
#if USE_NEW_LED_LIBRARY
      if (LED_Transition_IsLEDOff(&LEDTransition) == false)
      {
        LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS); // Turn off LED.
      }
#else 
      RGBLED_Command_Off(&MainLED, true);
      WHITE_LED_OFF;
#endif     
      break;
    }
  }
  else
  {
    /* No change in level, do nothing. */
  }

  PreviousBatteryLevel = Level;
}

static void PD_ProcessDebuggerCommands(void)
{
#if defined(ENABLE_DEBUG_MODE)
  if (DEBUGGER_SHORT_PRESS != 0)
  {
    UICommand |= UI_TOUCH_PWR_SHORT;
    DEBUGGER_SHORT_PRESS = 0;
  }

  if (DEBUGGER_LONG_PRESS != 0)
  {
    UICommand |= UI_TOUCH_PWR_LONG;
    DEBUGGER_LONG_PRESS = 0;
  }

  if (DEBUGGER_LED_TURBO != 0)
  {
    if (DEBUGGER_IS_LED_TURBO_ON)
    {
      TURBOL_LED_OFF;
      DEBUGGER_IS_LED_TURBO_ON = false;
    }
    else
    {
      TURBOL_LED_ON;
      DEBUGGER_IS_LED_TURBO_ON = true;
    }
    DEBUGGER_LED_TURBO = 0;
  }

  if (DEBUGGER_LED_SPEED1 != 0)
  {
    if (DEBUGGER_IS_LED_SPEED1_ON)
    {
      SPEED1_LED_OFF;
      DEBUGGER_IS_LED_SPEED1_ON = false;
    }
    else
    {
      SPEED1_LED_ON;
      DEBUGGER_IS_LED_SPEED1_ON = true;
    }
    DEBUGGER_LED_SPEED1 = 0;
  }

  if (DEBUGGER_LED_SPEED2 != 0)
  {
    if (DEBUGGER_IS_LED_SPEED2_ON)
    {
      SPEED2_LED_OFF;
      DEBUGGER_IS_LED_SPEED2_ON = false;
    }
    else
    {
      SPEED2_LED_ON;
      DEBUGGER_IS_LED_SPEED2_ON = true;
    }
    DEBUGGER_LED_SPEED2 = 0;
  }

  if (DEBUGGER_LED_SPEED3 != 0)
  {
    if (DEBUGGER_IS_LED_SPEED3_ON)
    {
      SPEED3_LED_OFF;
      DEBUGGER_IS_LED_SPEED3_ON = false;
    }
    else
    {
      SPEED3_LED_ON;
      DEBUGGER_IS_LED_SPEED3_ON = true;
    }
    DEBUGGER_LED_SPEED3 = 0;
  }
#endif
}


#if USE_NEW_LED_LIBRARY

const char *LED_Animation_PrintName(void *AnimationPtr);

/**
 * @brief  Callback function for LED animation Events.
 */
void LED_Complete_Callback(LED_Animation_Type_t animationType, LED_Status_t status, void *AnimationPtr)
{
      switch (status)
    {
    case LED_STATUS_ANIMATION_STARTED:
    DBG_MSG("LED_STATUS_ANIMATION_STARTED -> %s\n", LED_Animation_PrintName(AnimationPtr));
        break;

    case LED_STATUS_ANIMATION_COMPLETED:
    {
      DBG_MSG("LED_STATUS_ANIMATION_COMPLETED -> %s\n", LED_Animation_PrintName(AnimationPtr));
      // Once the LED animation is completed, turn on the white LED if flag is set
      #if USE_WHITE_LED
      if (TurnOnWhiteLEDOnCompletion)
      {
        TurnOnWhiteLEDOnCompletion = false;
        LED_RGYW_WHITE_ON;
      }
      #endif 
    }
    break;

    case LED_STATUS_ANIMATION_STOPPED:
    DBG_MSG("LED_STATUS_ANIMATION_STOPPED -> %s\n", LED_Animation_PrintName(AnimationPtr));
        break;

    case LED_STATUS_ANIMATION_TRANSITION_STARTED:
    {
      DBG_MSG("LED_STATUS_ANIMATION_TRANSITION_STARTED -> %s\n", LED_Animation_PrintName(AnimationPtr));
      #if USE_WHITE_LED
          // Turn off White LED to prevent it from being on during transition
          LED_RGYW_WHITE_OFF;
      #endif 
    }
    break;

    case LED_STATUS_ANIMATION_TRANSITION_COMPLETED:
    {
      DBG_MSG("LED_STATUS_ANIMATION_TRANSITION_COMPLETED -> %s\n", LED_Animation_PrintName(AnimationPtr));
    }
      break;

    default:
        if (IS_LED_ERROR_STATUS(status))
        {
          DBG_MSG("LED_STATUS_ANIMATION_ERROR -> %s\n", LED_Animation_PrintName(AnimationPtr));
        }
        break;
    }

}

// Function to print the name of the animation
const char *LED_Animation_PrintName(void *AnimationPtr)
{
    if (AnimationPtr == &LED_Solid_BatteryChargeFinished)
    {
        return "LED_Solid_BatteryChargeFinished\n";
    }
    else if (AnimationPtr == &LED_Solid_BatteryLow)
    {
        return "LED_Solid_BatteryLow\n";
    }
    else if (AnimationPtr == &LED_Solid_BatteryMid)
    {
        return "LED_Solid_BatteryMid\n";
    }
    else if (AnimationPtr == &LED_Solid_BatteryHigh)
    {
        return "LED_Solid_BatteryHigh\n";
    }
    else if (AnimationPtr == &LED_Solid_DefaultColor)
    {
        return "LED_Solid_DefaultColor\n";
    }
    else if (AnimationPtr == &LED_Flash_BatteryVeryLow)
    {
        return "LED_Flash_BatteryVeryLow\n";
    }
    else if (AnimationPtr == &LED_Flash_BatteryEmpty)
    {
        return "LED_Flash_BatteryEmpty\n";
    }
    else if (AnimationPtr == &LED_Flash_Error)
    {
        return "LED_Flash_Error\n";
    }
    else if (AnimationPtr == &LED_Pulse_BatteryCharge_LowAndVeryLow)
    {
        return "LED_Pulse_BatteryCharge_LowAndVeryLow\n";
    }
    else if (AnimationPtr == &LED_Pulse_BatteryCharge_Mid)
    {
        return "LED_Pulse_BatteryCharge_Mid\n";
    }
    else if (AnimationPtr == &LED_Pulse_BatteryCharge_High)
    {
        return "LED_Pulse_BatteryCharge_High\n";
    }
    else if (AnimationPtr == NULL)
    {
        return "LED_Off\n";
    }
    else
    {
        return "Unknown Animation\n";
    }
}

#endif



