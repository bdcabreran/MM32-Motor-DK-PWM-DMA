#include "main.h"
#include "p_tasks.h"
#include "p_config.h"
#include "mci_mock.h"



#define DEBUG_BUFFER_SIZE 256
static char debugBuffer[DEBUG_BUFFER_SIZE];

#define PTASK_DBG_ENABLE 1
#if PTASK_DBG_ENABLE
#define PTASK_DBG_MSG(fmt, ...) do { \
  snprintf(debugBuffer, DEBUG_BUFFER_SIZE, (fmt), ##__VA_ARGS__); \
  Uart_Put_Buff(debugBuffer, strlen(debugBuffer)); \
} while(0)
#else
#define PTASK_DBG_MSG(fmt, ...) do { } while(0)
#endif

#define SLEEP_TICKS 5000
#define BATT_FULL_LIGHT_OFF_DURATION_TICKS 3000
#define ON_TICKS (60000*60) //60 minutes 
#define SPEED_LED_FLASH_DURATION_ON_TICKS 750
#define SPEED_LED_FLASH_DURATION_OFF_TICKS 250
#define LED_RGYW_SUBSYSTEM_RUN_TICKS (1)
#define BATT_EMPTY_SHUTDOWN_TICKS (10000)
#define TIMER_MODE_DURATION_TICKS (60000*10) 
#define BATT_DISPLAY_DURATION_TICKS (3000)
#define FAULT_ACK_TIMEOUT_TICKS (3000)
#define SINGLE_BUTTON_PRESS_TIMEOUT_TICKS (350)

static bool SleepTriggered = false;

#define SPEED1_LED_ON  do{}while(0) // do nothing
#define SPEED2_LED_ON  do{}while(0) // do nothing
#define SPEED3_LED_ON  do{}while(0) // do nothing
#define TURBOL_LED_ON  do{}while(0) // do nothing
#define SPEED1_LED_OFF do{}while(0) // do nothing
#define SPEED2_LED_OFF do{}while(0) // do nothing
#define SPEED3_LED_OFF do{}while(0) // do nothing
#define TURBOL_LED_OFF do{}while(0) // do nothing


/* Battery Percentage Levels. */
#define BATT_LEVEL1_THRESHOLD (10U) /* (%) */
#define BATT_LEVEL2_THRESHOLD (25U) /* (%) */
#define BATT_LEVEL3_THRESHOLD (50U) /* (%) */
#define BATT_LEVEL4_THRESHOLD (99U) /* (%) */


// #include "p_parameter_conversion.h"
// #include "debug_config.h"
// #include "mcu_interface.h"
// #include "integer_math_utils.h"
// #include "m_control_interface.h"

// Define if you want White LED to be used, otherwise Default Color (RGY Color) will be used.
#define USE_WHITE_LED (0)

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
// BZR_States_t BuzzerStatePrev = BZR_OFF;
static volatile uint16_t LEDFlashCounter = ((uint16_t)0);
static volatile bool LEDFlashState = false;

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

static void PD_ProcessDebuggerCommands(void);

static void PD_UserInput(void);
static void PD_RunStateMachine(void);


void PD_RunSpeedChangeIndication(uint8_t Index);
static void PD_RunBuzzer(void);
static void PD_RunSubsystem_RGBW_LED(void);
static void PD_SetBatteryLevel(uint8_t Percentage);
static void PD_SetBatteryLED_Discharging(BATTLevel_State_t Level);
static void PD_SetBatteryLED_Charging(BATTLevel_State_t Level);
static void PD_RunPowerModeStateMachine(void);

bool PD_CheckErrorCodes(void);
void PD_RunWaitForSinglePress(void);

static uint16_t PD_UICommandScheduler(void);

static void PD_ClearAllBufferedCommands(void);

static void LED_Complete_Callback(LED_Animation_Type_t animationType, LED_Status_t status);

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

//   DETSW_Init(&DCInDetectSwitch);

  // LED Complete Callback is optional and can be used to trigger other events when LED animations are complete.
  LED_Animation_Init(&MainLED, &LED_Controller, LED_Complete_Callback);
  LED_Transition_Init(&LEDTransition, &MainLED);

  // This Mapping is optional and can be used to map the LED transitions to the LED animations.
  LED_Transition_SetMapping(&LEDTransition, LEDTransitionMap, LED_TRANSITION_MAP_SIZE);

//   BZR_Init(&Buzzer);
//   Buzzer_Disable();

  /* Initialize to idle state. */
  Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};

  Product.InitComplete = true;
  PTASK_DBG_MSG("Product Control Initialised.\r\n");
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

    PD_RunSubsystem_RGBW_LED();

    /* Decrement any counters if necessary. */
    if (Product.State.Counter > 0)
    {
      Product.State.Counter--;
    }
  }
}

static void PD_RunBuzzer(void)
{
    #if 0
  BZR_ReturnData_t BuzzerCommand;
  BZR_Run(&Buzzer, &BuzzerCommand);
  
//   if (BuzzerStatePrev != BuzzerCommand.State)
  {
    if (BuzzerCommand.State == BZR_ON)
    {
      Buzzer_Enable(BuzzerCommand.Frequency);
    }
    else
    {
      Buzzer_Disable();
    }
    // BuzzerStatePrev = BuzzerCommand.State;
  }
  #endif 
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

static void PD_RunSubsystem_RGBW_LED(void)
{
  static uint8_t LEDUpdateCounter = 0;
  static uint32_t tick = 0;

  if (++LEDUpdateCounter >= LED_RGYW_SUBSYSTEM_RUN_TICKS)
  {
    
    // Expected to be called with a 1ms period.
    LED_Transition_Update(&LEDTransition, tick);
    
    LEDUpdateCounter = 0;
    tick++; // update tick every 1ms
  }
}

void PD_BtnPwr_OnShortButtonPress(void)
{
  UICommand |= UI_TOUCH_PWR_SHORT;
}

void PD_BtnPwr_OnLongButtonPress(void)
{
  UICommand |= UI_TOUCH_PWR_LONG;
}

void PD_BtnSpeed1_OnShortButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED1_SHORT;
}

void PD_BtnSpeed1_OnLongButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED1_LONG;
}

void PD_BtnSpeed2_OnShortButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED2_SHORT;
}

void PD_BtnSpeed2_OnLongButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED2_LONG;
}

void PD_BtnSpeed3_OnShortButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED3_SHORT;
}

void PD_BtnSpeed3_OnLongButtonPress(void)
{
  UICommand |= UI_TOUCH_SPEED3_LONG;
}

void PD_BtnTurbo_OnShortButtonPress(void)
{
  UICommand |= UI_TOUCH_TURBO_SHORT;
}

void PD_BtnTurbo_OnLongButtonPress(void)
{
  UICommand |= UI_TOUCH_TURBO_LONG;
}

void PD_ClearAllBufferedCommands(void)
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
    // PTASK_DBG_MSG("Battery Percentage Initialised.\r\n");
    BatteryDisplayPercent = MCI_GetBatteryPercentage();
    /// TODO: could change to UART commands in future.
    if (BatteryDisplayPercent == (uint8_t)100)
    {
      // PTASK_DBG_MSG("Battery Full.\r\n");
      ChargingFinished = true;
    }
    else
    {
      // PTASK_DBG_MSG("Battery Percent: %d\r\n", BatteryDisplayPercent);
      ChargingFinished = false;
    }

#ifndef DISABLE_BATTERY_EMPTY
    if (BatteryDisplayPercent == (uint8_t)0)
    {
      // PTASK_DBG_MSG("Battery Empty.\r\n");
      BatteryIsEmpty = true;
    }
    else
    {
      // PTASK_DBG_MSG("Battery Not Empty.\r\n");
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
    // PTASK_DBG_MSG("P_IDLE\r\n");
    if (Product.State.Configured == false)
    {
      MCI_StopMotor();
      Product.ErrorCodeIndex = -1;
      MCI_AcknowledgeFaults();
      Product.State.Configured = true;

      // MOCK! 
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
            /* Turn off LED as charging is done. */
            PTASK_DBG_MSG("P_IDLE: Charging Finished, Turning Off LEDs\r\n");
            LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
          }
          else
          {
            /* Do nothing, leave charging LED logic in other state machine. */

          }
        }
        else
        {
          /* If cable not plugged, turn off. */
          PTASK_DBG_MSG("P_IDLE: Cable Not Plugged, Turning off LEDs\r\n");
          LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
        }
      }
      else
      {
        /* If no battery detected, turn off. */
        PTASK_DBG_MSG("P_IDLE: No Battery Detected, Turning off LEDs\r\n");
        LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
      }
    }

#ifdef ENABLE_POWER_DISPLAY
    if (CMD == UI_BTN_SHORT_1 && SinglePressTimeExceeded != true)
    {
      /* Short button press detected. */
      Product.State = (ProductState_Handle_t){P_POWER_DISPLAY, Product.State.Now, POWER_DISPLAY_TICKS, false};
      break; /* Break immediately so as not to run other code below. */
    }
    else
#endif
    {

    }

    if (CMD != UI_NO_CMD)
    {
      /* Any UI command will reset sleep timer. */
      Product.State.Counter = SLEEP_TICKS;

      if (CMD == UI_TOUCH_PWR_LONG)
      {
        PTASK_DBG_MSG("P_IDLE: UI_TOUCH_PWR_LONG\r\n");
#ifdef BATT_EMPTY_AUTO_OFF_ENABLE
        if (BatteryIsEmpty == true)
        {
          Product.State = (ProductState_Handle_t){P_BATT_EMPTY, Product.State.Now, BATT_EMPTY_WAIT_TICKS, false};
        }
        else
#endif
        {
          /* Long button press recieved. Prepare to move to P_START if requirements are met. */
          Product.State = (ProductState_Handle_t){P_START, Product.State.Now, 0, false};
        }
        break; /* break immediately. */
      }
      else if (CMD == UI_TOUCH_PWR_SHORT)
      {
        PTASK_DBG_MSG("P_IDLE: UI_TOUCH_PWR_SHORT\r\n");
        if (MCI_IsBatteryDetected() == true)
        {
          /* Show battery life on LED. */
          ShowBatteryLife = true;
          ShowBatteryLifeConfigured = false;
          PTASK_DBG_MSG("P_IDLE: Show Battery Life\r\n");
        }
      }
    }
    else
    {
#ifndef DISABLE_SLEEP_MODE
      /* No UI command, check if product should enter sleep state. */
      /* If state counter reaches zero, product goes to sleep. */
      if (Product.State.Counter == 0 && MCI_IsCablePlugged() != true)
      {
        /* Go to sleep. */
        PD_ClearAllBufferedCommands();
        Product.State = (ProductState_Handle_t){P_SLEEP, Product.State.Now, 0, false};
        break; /* break immediately. */
      }
#endif
    }
  }
  break;

  case P_START:
  {
    PTASK_DBG_MSG("P_START\r\n");

    if (MCI_StartMotor() == true)
    {
      ShowBatteryLife = false;

      if (LED_Transition_IsLEDOff(&LEDTransition))
      {
        if (MCI_IsCablePlugged() == true)
        {
          if (ChargingFinished == true)
          {
            LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryChargeFinished, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
          }
          else
          {
            PD_SetBatteryLED_Charging(Product.BatteryLevel);
          }
        }
        else
        {
          #if USE_WHITE_LED
          LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
          TurnOnWhiteLEDOnCompletion = true; // handled on LED Callback
          #else
          LED_Transition_ToSolid(&LEDTransition, &LED_Solid_DefaultColor, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
          #endif 
        }
      }
      else
      {
        /* LED is still on, retain statee. */
      }

    //   BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_1, BUZZER_DEFAULT_TIMES, BUZZER_DURATION_TURN_ON_TICKS, 0);
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
    PTASK_DBG_MSG("P_ON\r\n");
    /* Can move to P_IDLE, P_CABLEIN or P_FAULT_NOW from here. */
    static bool BattLowStatePrev = false;
    bool BattLowState = false;
    uint8_t MotorFaults;

    if (Product.State.Configured != true)
    {
      BattEmptyTimerStarted = false;
      Product.State.Configured = true;
      Product.SpeedSetting = SPEED_SETTING_1;
      Product.PrevSpeedIndex = SPEED_SETTING_1;
    //   Mci.SpeedSetting = SPEED_SETTING_1;
    //   Mci.PrevMode = SPEED_SETTING_1;
    }

    if (MCI_IsBatteryDetected() == true)
    {
      if (BatteryIsEmpty == true)
      {
        (void)MCI_StopMotor();

  #ifdef BATT_EMPTY_AUTO_OFF_ENABLE
        Product.State = (ProductState_Handle_t){P_BATT_EMPTY, Product.State.Now, BATT_EMPTY_WAIT_TICKS, false};
        break; /* break immediately. */
  #else
        if (BattEmptyTimerStarted != true)
        {
          BattEmptyTimerStarted = true;
          BattEmptyShutdownCounter = BATT_EMPTY_SHUTDOWN_TICKS;
        }
        else
        {
          if (BattEmptyShutdownCounter > 0)
          {
            BattEmptyShutdownCounter--;
          }
          else
          {
            /* Battery has been empty for BATT_EMPTY_SHUTDOWN_TIME,
                go to IDLE so sleep can be triggered. */
            Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
          }
        }
  #endif
      }
    }

    if (PD_CheckErrorCodes() == true)
    {
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

    if (CMD == UI_TOUCH_PWR_LONG || Product.State.Counter == 0)
    {
      if (CMD == UI_TOUCH_PWR_LONG)
      {
        /* Only buzz if it was user interaction. */
        // BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_1, BUZZER_DEFAULT_TIMES, BUZZER_DURATION_TURN_OFF_TICKS, 0);
      }

      MCI_StopMotor();

      Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
    }
    else if (CMD == UI_TOUCH_PWR_SHORT)
    {
      if (MCI_IsBatteryDetected() == true)
      {
        /* Show battery life for 3 seconds. */
        ShowBatteryLife = true;
        ShowBatteryLifeConfigured = false;
      }
    }
    else if (CMD == UI_TOUCH_SPEED1_SHORT)
    {
      Product.SpeedSetting = SPEED_SETTING_1;
    //   BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_1, BUZZER_DEFAULT_TIMES, BUZZER_DURATION_SPEED_CHANGE_ON_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_SPEED1_LONG)
    {
      Product.SpeedSetting = SPEED_SETTING_1;
      Product.NewMode = !Product.Mode; /* Alternate between the two modes. */
    //   BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_1, MODE_CHANGE_BEEP_TIMES, MODE_CHANGE_BEEP_DURATION_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_SPEED2_SHORT)
    {
      Product.SpeedSetting = SPEED_SETTING_2;
    //   BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_2, BUZZER_DEFAULT_TIMES, BUZZER_DURATION_SPEED_CHANGE_ON_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_SPEED2_LONG)
    {
      Product.SpeedSetting = SPEED_SETTING_2;
      Product.NewMode = !Product.Mode; /* Alternate between the two modes. */
    //   BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_2, MODE_CHANGE_BEEP_TIMES, MODE_CHANGE_BEEP_DURATION_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_SPEED3_SHORT)
    {
      Product.SpeedSetting = SPEED_SETTING_3;
    //   BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_3, BUZZER_DEFAULT_TIMES, BUZZER_DURATION_SPEED_CHANGE_ON_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_SPEED3_LONG)
    {
      Product.SpeedSetting = SPEED_SETTING_3;
      Product.NewMode = !Product.Mode; /* Alternate between the two modes. */
    //   BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_3, MODE_CHANGE_BEEP_TIMES, MODE_CHANGE_BEEP_DURATION_TICKS, 0);
    }
    else if (CMD == UI_TOUCH_TURBO_SHORT)
    {
      Product.SpeedSetting = SPEED_SETTING_TURBO;
    //   BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_3, BUZZER_TURBO_TIMES, BUZZER_DURATION_SPEED_CHANGE_ON_TICKS, BUZZER_DURATION_SPEED_CHANGE_TURBO_OFF_TICKS);
    }
    else if (CMD == UI_TOUCH_TURBO_LONG)
    {
      Product.SpeedSetting = SPEED_SETTING_TURBO;
      Product.NewMode = !Product.Mode; /* Alternate between the two modes. */
    //   BZR_SetOnCommand(&Buzzer, BUZZER_PWM_FREQUENCY_3, BUZZER_TURBO_TIMES, MODE_CHANGE_BEEP_DURATION_TICKS, BUZZER_DURATION_SPEED_CHANGE_TURBO_OFF_TIME);
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
    PTASK_DBG_MSG("P_SLEEP\r\n");
    if (MCI_IsCablePlugged() == true)
    {
      Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
    }
    else
    {
      if (SleepTriggered == true)
      {
        /* Do nothing. */
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
  }
  break;

#ifdef BATT_EMPTY_AUTO_OFF_ENABLE
  case P_BATT_EMPTY:
  {

    if (Product.State.Configured == false)
    {
      int8_t FlashTimes;
      uint32_t FlashWaitTime;

      if (Product.State.Previous == P_ON)
      {
        /* Flash more times so that user can see it when they wonder why scooter not working. */
        FlashTimes = BATT_EMPTY_FLASH_TIMES_DISCHARGE;
        FlashWaitTime = BATT_EMPTY_WAIT_TIME_DISCHARGE;
      }
      else
      {
        FlashTimes = BATT_EMPTY_FLASH_TIMES;
        FlashWaitTime = BATT_EMPTY_WAIT_TIME;
      }


      Product.State.Counter = FlashWaitTime;
      Product.State.Configured = true;
    }

    if (Product.State.Counter == 0)
    {
      Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
    }
  }
  break;
#endif

  case P_FAULT_NOW:
  {
    PTASK_DBG_MSG("P_FAULT_NOW\r\n");
    if (Product.State.Configured == false)
    {
      if (Product.ErrorCodeIndex != -1)
      {
        /* Display fault code on display. */
        if (LED_Transition_IsBusy(&LEDTransition))
        {
          LED_Transition_Stop(&LEDTransition);
        }

        LED_Transition_ToFlash(&LEDTransition, &LED_Flash_Error, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
      }
      else
      {
        /* Fault not found, move to next state. */
        Product.State = (ProductState_Handle_t){P_FAULT_ACKNOWLEDGE, Product.State.Now, 0, false};
      }
      Product.State.Configured = true;
    }

    MCI_AcknowledgeFaults();
    Product.State = (ProductState_Handle_t){P_FAULT_ACKNOWLEDGE, Product.State.Now, FAULT_ACK_TIMEOUT_TICKS, false};

    break;
  }

  case P_FAULT_ACKNOWLEDGE:
  {
    PTASK_DBG_MSG("P_FAULT_ACKNOWLEDGE\r\n");
    if (Product.State.Counter == 0)
    {
      MCI_ResetFaultState();
      Product.State = (ProductState_Handle_t){P_IDLE, Product.State.Now, SLEEP_TICKS, false};
    }
    break;
  }

  default:
  {
    break;
  }
  }
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

bool SendDefaultLEDCommand = true;

static void PD_RunPowerModeStateMachine(void)
{
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
          LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryChargeFinished, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
        }
        else
        {
          if (BatteryIsCharging == true)
          {
            CableInSubState = P_CABLEIN_CHARGING;
            PD_SetBatteryLED_Charging(Product.BatteryLevel);
          }
        }
      }
      break;

      case P_CABLEIN_CHARGING:
      {
        PD_SetBatteryLED_Charging(Product.BatteryLevel);

        if (ChargingFinished == true)
        {
          CableInSubState = P_CABLEIN_FINISHED;
          BatteryFullLEDCounter = BATT_FULL_LIGHT_OFF_DURATION_TICKS;
          LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryChargeFinished, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
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
            LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
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
            /* Do nothing. */
          }
          else
          {
            if (ShowBatteryLife == true)
            {
              if (ShowBatteryLifeConfigured != true)
              {
                /* Force one command. */
                PD_SetBatteryLED_Discharging(Product.BatteryLevel);
                ShowBatteryLifeTimer = BATT_DISPLAY_DURATION_TICKS;
                ShowBatteryLifeConfigured = true;
              }

              if (ShowBatteryLifeTimer > 0)
              {
                ShowBatteryLifeTimer--;
              }
              else
              {
                /* Turn back to off. */
                LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
                ShowBatteryLife = false;
                BatteryFullLEDCounter = 0;
              }
            }
            else
            {
              /* When product turned off, should turn off LED after duration. */
              if (BatteryFullLEDCounter > 0)
              {
                BatteryFullLEDCounter--;
              }
              else
              {
                /* Turn it off. */
                if (!LED_Transition_IsLEDOff(&LEDTransition))
                {
                  LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
                }
              }
            }
          }
        }
        else
        {
          PD_SetBatteryLED_Charging(Product.BatteryLevel);
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
          /* Force one command. */
          PD_SetBatteryLED_Discharging(Product.BatteryLevel);
          ShowBatteryLifeTimer = BATT_DISPLAY_DURATION_TICKS;
          ShowBatteryLifeConfigured = true;
        }

        if (ShowBatteryLifeTimer > 0)
        {
          ShowBatteryLifeTimer--;
        }
        else
        {
          if (Product.State.Now == P_ON)
          {
            /* Turn back to white. */
            #if USE_WHITE_LED
            LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
            TurnOnWhiteLEDOnCompletion = true; // handled on LED Callback
            #else
            LED_Transition_ToSolid(&LEDTransition, &LED_Solid_DefaultColor, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
            #endif 
          }
          else
          {
            LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
          }
          ShowBatteryLife = false;
        }
      }
      else
      {
        if (Product.State.Now == P_ON)
        {
          if (SendDefaultLEDCommand == true)
          {
            /// TODO: Be Careful not to call this function repeatedly , it will cause the LED to flicker.
            #if USE_WHITE_LED
            LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
            TurnOnWhiteLEDOnCompletion = true; // handled on LED Callback
            #else
            // LED_Transition_ToSolid(&LEDTransition, &LED_Solid_DefaultColor, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
            #endif 
            SendDefaultLEDCommand = false;
          }

        }
        else
        { 
          // if LED not off then turn it off
          if (!LED_Transition_IsLEDOff(&LEDTransition))
          {
            LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
          }
        }
      }
    }
  }
  else /* No battery detected. */
  {
    if (Product.State.Now == P_ON)
    {
      // Note: This transition is commented out due to the MCU becoming un-flashable, 
      #if !USE_WHITE_LED
      //LED_Transition_ToSolid(&LEDTransition, &LED_Solid_DefaultColor, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
      #endif 
    }
    else
    {
      /* Do nothing. */
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
  // PTASK_DBG_MSG("Battery Level: %d\r\n", Level);  
}

static void PD_SetBatteryLED_Discharging(BATTLevel_State_t Level)
{
  /* If level has changed or force is set to true, update LED command. */
  if (PreviousBatteryLevel != Level)
  {
    switch (Level)
    {
      case BATTERY_LEVEL_1:
      {
        LED_Transition_ToFlash(&LEDTransition, &LED_Flash_BatteryVeryLow, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
      }
      break;
      
      case BATTERY_LEVEL_2:
      {
        LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryLow, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
      }
      break;
      
      case BATTERY_LEVEL_3:
      {
        LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryMid, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
      }
      break;
      
      case BATTERY_LEVEL_4:
      case BATTERY_LEVEL_FULL:
  {
        LED_Transition_ToSolid(&LEDTransition, &LED_Solid_BatteryHigh, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS);
      }
      break;

      default:
        LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS); // Turn off LED.
      break;
    }
  }
  else
  {
    /* No change in level, do nothing. */
  }

  PreviousBatteryLevel = Level;
}

static void PD_SetBatteryLED_Charging(BATTLevel_State_t Level)
{
  /* If level has changed Update Animation  */
  if (PreviousBatteryLevel != Level)
  {  
    switch (Level)
    {
      case BATTERY_LEVEL_1:
      case BATTERY_LEVEL_2:
    {
        LED_Transition_ExecuteWithMap(&LEDTransition, &LED_Pulse_BatteryCharge_LowAndVeryLow, LED_ANIMATION_TYPE_PULSE);
      }
      break;
      
      case BATTERY_LEVEL_3:
      {
        LED_Transition_ExecuteWithMap(&LEDTransition, &LED_Pulse_BatteryCharge_Mid, LED_ANIMATION_TYPE_PULSE);

      }
      break;
      
      case BATTERY_LEVEL_4:
      {
        LED_Transition_ExecuteWithMap(&LEDTransition, &LED_Pulse_BatteryCharge_High, LED_ANIMATION_TYPE_PULSE);

      }
      break;

      case BATTERY_LEVEL_FULL:
      {
        LED_Transition_ExecuteWithMap(&LEDTransition, &LED_Solid_BatteryHigh, LED_ANIMATION_TYPE_SOLID);
      }
      break;

      default:
        LED_Transition_ToOff(&LEDTransition, LED_TRANSITION_INTERPOLATE, LED_RGYW_INTERPOLATION_TIME_MS); 
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

/**
 * @brief  Callback function for LED animation Events.
 */
static void LED_Complete_Callback(LED_Animation_Type_t animationType, LED_Status_t status)
{
      switch (status)
    {
    case LED_STATUS_ANIMATION_STARTED:
      PTASK_DBG_MSG("LED_STATUS_ANIMATION_STARTED\r\n");
      #if USE_WHITE_LED
          // Turn off White LED
          LED_RGYW_WHITE_OFF;
      #endif 
        break;

    case LED_STATUS_ANIMATION_COMPLETED:
    PTASK_DBG_MSG("LED_STATUS_ANIMATION_COMPLETED\r\n");
        break;

    case LED_STATUS_ANIMATION_STOPPED:
    PTASK_DBG_MSG("LED_STATUS_ANIMATION_STOPPED\r\n");
        break;

    case LED_STATUS_ANIMATION_TRANSITION_STARTED:
    PTASK_DBG_MSG("LED_STATUS_ANIMATION_TRANSITION_STARTED\r\n");
      #if USE_WHITE_LED
          // Turn off White LED
          LED_RGYW_WHITE_OFF;
      #endif 
        break;

    case LED_STATUS_ANIMATION_TRANSITION_COMPLETED:
    {
      PTASK_DBG_MSG("LED_STATUS_ANIMATION_TRANSITION_COMPLETED\r\n");
      // Once the LED animation is completed, turn on the white LED
      #if USE_WHITE_LED
      if (TurnOnWhiteLEDOnCompletion && LED_Transition_IsLEDOff(&LEDTransition))
      {
        TurnOnWhiteLEDOnCompletion = false;
        LED_RGYW_WHITE_ON;
      }
      #endif 
    }
      break;

    default:
        if (IS_LED_ERROR_STATUS(status))
        {
          PTASK_DBG_MSG("LED_STATUS_ERROR\r\n");
        }
        break;
    }

}
