#include "led_rgb.h"

/// TODO: re-do the scheduling of commands, it's prone to state bugs.
/// Also command next doesn't make sense for some states and could cause
/// stagnant commands to stored in next only to be executed much later.

static void RGBLED_DisablePWM(RGBLED_Handle_t *this);
static void RGBLED_EnablePWM(RGBLED_Handle_t *this);

static void RGBLED_BreathValueUpdate(RGBLED_Handle_t *this, RGBColor_Handle_t *Color);
static void RGBLED_SetBreathColor(RGBLED_Handle_t *this, RGBColor_Handle_t *Color);
static void RGBLED_SetColor(RGBLED_Handle_t *this, RGBColor_Handle_t *Color);
static void RGBLED_SetPWM(RGBLED_Handle_t *this);

static void RGBLED_Command_Generic(RGBLED_Handle_t *this, uint8_t Command, RGBLED_Cmd_Handle_t *CommandHandle, bool ForceNow);
static bool RGBLED_CheckBufferedCommand(RGBLED_Handle_t *this);

static RGBLED_Cmd_Handle_t NullCmdHandle;
static RGBColor_Handle_t LEDOffColorHandle = {
    .R = 0,
    .G = 0,
    .B = 0,
};

__WEAK void RGBLED_Init(RGBLED_Handle_t *this)
{
  this->CommandNow = RGBLED_NO_CMD;
  this->CommandNow_Handle = &NullCmdHandle;
  this->CommandNext_Handle = &NullCmdHandle;
  this->State = (RGBLED_State_Handle_t){RGBLED_OFF, RGBLED_OFF, 0};
}

__WEAK void RGBLED_Update(RGBLED_Handle_t *this)
{
  if (this->State.Counter > 0)
  {
    this->State.Counter--;
  }

  switch (this->State.Now)
  {

  case RGBLED_OFF:
  {
    /* Can move to any state from here. */

    if (this->State.Configured == false)
    {
      RGBLED_DisablePWM(this);

      this->CommandNow &= ~RGBLED_OFF_CMD;

      this->State.Configured = true;
    }

    switch (this->CommandNow)
    {
    case RGBLED_NO_CMD:
    {
      (void)RGBLED_CheckBufferedCommand(this);
      break;
    }

    case RGBLED_OFF_CMD:
    {
      (void)RGBLED_CheckBufferedCommand(this);
      break;
    }

    case RGBLED_SOLID_CMD:
    {
      this->CommandNow &= ~RGBLED_SOLID_CMD;
      this->State = (RGBLED_State_Handle_t){RGBLED_SOLID, this->State.Now, false, 0};
      break;
    }

    case RGBLED_FLASH_CMD:
    {
      this->CommandNow &= ~RGBLED_FLASH_CMD;
      this->State = (RGBLED_State_Handle_t){RGBLED_FLASH, this->State.Now, false, 0};
      break;
    }

    case RGBLED_BREATH_CMD:
    {
      this->CommandNow &= ~RGBLED_BREATH_CMD;
      this->State = (RGBLED_State_Handle_t){RGBLED_BREATH, this->State.Now, false, 0};
      break;
    }

    default:
    {
      break;
    }
    }

    break;
  }

  case RGBLED_SOLID:
  {
    /* Can move to RGBLED_OFF state from here. */
    static RGBLED_Solid_Handle_t *Config;

    if (this->CommandNow != RGBLED_NO_CMD)
    {
      /* New command received, move back to OFF state so it can be executed. */
      this->State = (RGBLED_State_Handle_t){RGBLED_OFF, this->State.Now, false, 0};
      break; /* Break immediately. */
    }

    if (this->State.Configured != true)
    {
      Config = (RGBLED_Solid_Handle_t *)this->CommandNow_Handle;

      RGBLED_EnablePWM(this);

      RGBLED_SetColor(this, Config->Color);

      this->State.Configured = true;
    }

    (void)RGBLED_CheckBufferedCommand(this);

    break;
  }

  case RGBLED_FLASH:
  {
    /// TODO: this needs to be change to a LocalConfig struct variable like PWMLED does
    static RGBLED_Flash_Handle_t *Config;
    static uint8_t FlashCounter = 0;
    static bool LEDisOn = false;

    if (this->CommandNow != RGBLED_NO_CMD)
    {
      /* Change of state requested immediately. Go back to OFF so command can be executed. */
      this->State = (RGBLED_State_Handle_t){RGBLED_OFF, this->State.Now, false, 0};
      break; /* Break immediately. */
    }

    /* Do initial state configuration. */
    if (this->State.Configured != true)
    {
      Config = (RGBLED_Flash_Handle_t *)this->CommandNow_Handle;

      /* Set local flash counter to track how many times LED has flashed. */
      FlashCounter = Config->Times;

      /* Enable PWM. */
      RGBLED_EnablePWM(this);

      /* Set initial LED state based on configured flash type. */
      if (Config->Type == RGBLEDFlashType_ON)
      {
        /* Set counter for how long LED should be on. */
        this->State.Counter = Config->OnTicks;

        /* Set initial LED color. */
        RGBLED_SetColor(this, Config->Color);

        LEDisOn = true;
      }
      else if (Config->Type == RGBLEDFlashType_OFF)
      {
        /* Set counter for how long LED should be off. */
        this->State.Counter = Config->OffTicks;

        RGBLED_SetColor(this, &LEDOffColorHandle);

        LEDisOn = false;
      }

      this->State.Configured = true;
    }

    /* If negative number was passed, assume this means to flash permanently. */
    if (Config->Times < 0)
    {
      FlashCounter = UINT8_MAX;
    }

    if (this->State.Counter == 0)
    {
      if (Config->Times < 0)
      {
        /* Flash permanently is configured, but this must be over-ridden in case 
           a new command arrives. */
        if (RGBLED_CheckBufferedCommand(this) == true)
        {
          /* Move to RGBLED_OFF state. */
          this->State = (RGBLED_State_Handle_t){RGBLED_OFF, this->State.Now, false, 0};
          break; /* Break immediately. */
        }
        else
        {
          /* no new command, do nothing and continue flashing. */
        }
      }
      else
      {
        /* Flash was configured for specific number of times, only check for new 
           commands after the sequence has finished completely. */
        if (FlashCounter == 0)
        {
          (void)RGBLED_CheckBufferedCommand(this);
          
          /* Move to RGBLED_OFF state as sequence is finished. */
          this->State = (RGBLED_State_Handle_t){RGBLED_OFF, this->State.Now, false, 0};
          break; /* Break immediately. */
        }
        else
        {
          /* Sequence hasn't finished, don't check for new commands. */
        }
      }

      /* Sequence still in progress, check which state to move to. */
      if (LEDisOn == true)
      {
        /* Turn LED off. */
        RGBLED_SetColor(this, &LEDOffColorHandle);

        /* Capture local LED state. */
        LEDisOn = false;

        /* Set counter to count how long we should remain with LED off. */
        this->State.Counter = Config->OffTicks;

        if (Config->Type == RGBLEDFlashType_ON)
        {
          /* Decrement flash counter as LED has completed one flash cycle. */
          FlashCounter--;
        }
      }
      else
      {
        /* Turn LED on. */
        RGBLED_SetColor(this, Config->Color);

        /* Capture local LED state. */
        LEDisOn = true;

        /* Set counter to count how long we should remain with LED on. */
        this->State.Counter = Config->OnTicks;

        if (Config->Type == RGBLEDFlashType_OFF)
        {
          /* Decrement flash counter as LED has completed one flash cycle. */
          FlashCounter--;
        }
      }
    }

    break;
  }

  case RGBLED_BREATH:
  {
    /// TODO: this needs to be change to a LocalConfig struct variable like PWMLED does
    static RGBLED_Breath_Handle_t *Config;
    static RGBColor_Handle_t LocalColorHandle;

    if (this->CommandNow != RGBLED_NO_CMD)
    {
      /* Change of state requested immediately. Go back to OFF so command can be executed. */
      this->State = (RGBLED_State_Handle_t){RGBLED_OFF, this->State.Now, false, 0};
      break; /* Break immediately. */
    }

    if (this->State.Configured != true)
    {
      int16_t Error;
      uint16_t TmpInhaleExhaleTicks;

      Config = (RGBLED_Breath_Handle_t *)this->CommandNow_Handle;
      LocalColorHandle = *(Config->Color);

      /* Calculate temporary ticks based on exact timings requested by config. */
      TmpInhaleExhaleTicks = (Config->PeriodTicks - Config->HoldInTicks - Config->HoldOutTicks) / 2U;

      /* Calculate time between each breath value update. With fair rounding applied. */
      this->Breath.InhaleExhaleIncrements = (TmpInhaleExhaleTicks + (1 << (RGBLED_BREATH_TICKS_MAX_BITS - 1))) >> RGBLED_BREATH_TICKS_MAX_BITS; 

      /* Make inhale exhale ticks an exact multiple of increments. */
      this->Breath.InhaleExhaleTicks = this->Breath.InhaleExhaleIncrements * 256U;

      /* Calculate error between exact ticks and ticks after truncation. */
      Error = TmpInhaleExhaleTicks - this->Breath.InhaleExhaleTicks;

      /* Adjust on and off ticks so total configured period is still correct. */
      this->Breath.HoldInTicks = Config->HoldInTicks + Error;
      this->Breath.HoldOutTicks = Config->HoldOutTicks + Error;

      /* Reset breath variables. */
      this->Breath.Counter = 0;
      this->Breath.ValueInc = 1;
      this->Breath.Value = 0;

      /* Enable PWM. */
      RGBLED_EnablePWM(this);

      /* Set state. */
      this->State.Counter = this->Breath.InhaleExhaleTicks;
      this->Breath.State = RGBLEDBreathState_IN;
      this->State.Configured = true;

      /* Break so first and subsequent breath sequences are identical. */
      break; 
    }

    switch (this->Breath.State)
    {
      case RGBLEDBreathState_IN:
      {
        if (this->State.Counter == 0)
        {
          /* Set LED to max value. */
          RGBLED_SetColor(this, &LocalColorHandle);

          /* Set number of ticks LED should remain on for. */
          this->State.Counter = this->Breath.HoldInTicks;
          
          /* Reset Breath counter. */
          this->Breath.Counter = 0;

          /* Reduce increment value by 2 since 2 was added and not used.
             This ensures exhale is symmetric with inhale. */
          this->Breath.ValueInc -= 2;

          /* Change state. */
          this->Breath.State = RGBLEDBreathState_HOLDIN;

          break; /* Break immediately. */
        }

        RGBLED_BreathValueUpdate(this, &LocalColorHandle);

        break;
      }

      case RGBLEDBreathState_HOLDIN:
      {
        if (this->State.Counter == 0)
        {
          /* Set number of ticks LED should exhale for. */
          this->State.Counter = this->Breath.InhaleExhaleTicks;

          /* Change state. */
          this->Breath.State = RGBLEDBreathState_OUT;
        }
        break;
      }

      case RGBLEDBreathState_OUT:
      {
        if (this->State.Counter == 0)
        {
          /* Set LED to off. */
          RGBLED_SetColor(this, &LEDOffColorHandle);

          /* Set number of ticks LED should be off for. */
          this->State.Counter = this->Breath.HoldOutTicks;
          
          /* Reset Breath State. */
          this->Breath.Counter = 0;
          this->Breath.ValueInc = 1;
          this->Breath.Value = 0;

          /* Change state. */
          this->Breath.State = RGBLEDBreathState_HOLDOUT;

          break; /* Break immediately. */
        }
        
        RGBLED_BreathValueUpdate(this, &LocalColorHandle);

        break;
      }

      case RGBLEDBreathState_HOLDOUT:
      {
        if (this->State.Counter == 0)
        {
          /* Set number of ticks LED should inhale for. */
          this->State.Counter = this->Breath.InhaleExhaleTicks;

          /* Change state. */
          this->Breath.State = RGBLEDBreathState_IN;

          /* One breath cycle finished so check if there is a buffered command. */
          RGBLED_CheckBufferedCommand(this);
        }
        break;
      }

    }

    break;
  }

  default:
  {
    break;
  }
  }
}

static void RGBLED_BreathValueUpdate(RGBLED_Handle_t *this, RGBColor_Handle_t *Color)
{
  if (++this->Breath.Counter >= this->Breath.InhaleExhaleIncrements)
  {

    switch (this->Breath.State)
    {
      case RGBLEDBreathState_IN:
      {
        this->Breath.Value += this->Breath.ValueInc;

        if (this->Breath.Value > RGBLED_BREATH_RESOLUTION)
        {
          this->Breath.Value = RGBLED_BREATH_RESOLUTION;
        }

        this->Breath.ValueInc += 2U;

        break;
      }

      case RGBLEDBreathState_OUT:
      {
        if (this->Breath.Value >= this->Breath.ValueInc)
        {
          this->Breath.Value -= this->Breath.ValueInc;

          /* Prevent underflow. */
          if (this->Breath.ValueInc > 1U)
          {
            this->Breath.ValueInc -= 2U;
          }
        } 

        break;
      }

      default:
      {
        break;
      }
    }

    RGBLED_SetBreathColor(this, Color);

    this->Breath.Counter = 0;
  }
}

static bool RGBLED_CheckBufferedCommand(RGBLED_Handle_t *this)
{
  bool NewCommandReceived = false;

  if (this->CommandNext != RGBLED_NO_CMD)
  {
    /* Set next to command to current command. */
    this->CommandNow = this->CommandNext;
    this->CommandNow_Handle = this->CommandNext_Handle;

    /* Remove pending next command. */
    this->CommandNext = RGBLED_NO_CMD;
    this->CommandNext_Handle = &NullCmdHandle;

    NewCommandReceived = true;
  }
  return NewCommandReceived;
}

static void RGBLED_DisablePWM(RGBLED_Handle_t *this)
{
  /* Turn PWM off and disable timer. */

  /* Set all channels to 0 */
  *(this->TimerInterface.CCR_Red) = 0;
  *(this->TimerInterface.CCR_Green) = 0;
  *(this->TimerInterface.CCR_Blue) = 0;

  // /* Disable PWM outputs. */
  this->TimerInterface.DisablePWM();
}

static void RGBLED_EnablePWM(RGBLED_Handle_t *this)
{
  /* Set all channels to 0 */
  *(this->TimerInterface.CCR_Red) = 0;
  *(this->TimerInterface.CCR_Green) = 0;
  *(this->TimerInterface.CCR_Blue) = 0;

  /* Enable PWM outputs. */
  this->TimerInterface.EnablePWM();
}

static void RGBLED_SetBreathColor(RGBLED_Handle_t *this, RGBColor_Handle_t *Color)
{
  this->PWMValue.R = ((((uint32_t)this->MaxPulse * this->Breath.Value) >> RGBLED_BREATH_RESOLUTION_BITS) * Color->R) >> RGBLED_COLOR_RESOLUTION_BITS;
  this->PWMValue.G = ((((uint32_t)this->MaxPulse * this->Breath.Value) >> RGBLED_BREATH_RESOLUTION_BITS) * Color->G) >> RGBLED_COLOR_RESOLUTION_BITS;
  this->PWMValue.B = ((((uint32_t)this->MaxPulse * this->Breath.Value) >> RGBLED_BREATH_RESOLUTION_BITS) * Color->B) >> RGBLED_COLOR_RESOLUTION_BITS;

  RGBLED_SetPWM(this);
}

static void RGBLED_SetColor(RGBLED_Handle_t *this, RGBColor_Handle_t *Color)
{
  this->PWMValue.R = ((uint32_t)this->MaxPulse * Color->R) >> RGBLED_COLOR_RESOLUTION_BITS;
  this->PWMValue.G = ((uint32_t)this->MaxPulse * Color->G) >> RGBLED_COLOR_RESOLUTION_BITS;
  this->PWMValue.B = ((uint32_t)this->MaxPulse * Color->B) >> RGBLED_COLOR_RESOLUTION_BITS;

  RGBLED_SetPWM(this);
}

static void RGBLED_SetPWM(RGBLED_Handle_t *this)
{
  *(this->TimerInterface.CCR_Red) = this->PWMValue.R;
  *(this->TimerInterface.CCR_Green) = this->PWMValue.G;
  *(this->TimerInterface.CCR_Blue) = this->PWMValue.B;
}

__WEAK void RGBLED_Command_Off(RGBLED_Handle_t *this, bool ForceNow)
{
  RGBLED_Command_Generic(this, RGBLED_OFF_CMD, &NullCmdHandle, ForceNow);
}

__WEAK void RGBLED_Command_Solid(RGBLED_Handle_t *this, RGBLED_Solid_Handle_t *CmdHandle, bool ForceNow)
{
  RGBLED_Command_Generic(this, RGBLED_SOLID_CMD, &CmdHandle->_Super, ForceNow);
}

__WEAK void RGBLED_Command_Flash(RGBLED_Handle_t *this, RGBLED_Flash_Handle_t *CmdHandle, bool ForceNow)
{
  RGBLED_Command_Generic(this, RGBLED_FLASH_CMD, &CmdHandle->_Super, ForceNow);
}

__WEAK void RGBLED_Command_Breath(RGBLED_Handle_t *this, RGBLED_Breath_Handle_t *CmdHandle, bool ForceNow)
{
  RGBLED_Command_Generic(this, RGBLED_BREATH_CMD, &CmdHandle->_Super, ForceNow);
}

static void RGBLED_Command_Generic(RGBLED_Handle_t *this, uint8_t Command, RGBLED_Cmd_Handle_t *CommandHandle, bool ForceNow)
{
  if (ForceNow == true)
  {
    this->CommandNow = Command;
    this->CommandNow_Handle = CommandHandle;
  }
  else
  {
    this->CommandNext = Command;
    this->CommandNext_Handle = CommandHandle;
  }
}

void RGBLED_ClearBufferedCommands(RGBLED_Handle_t *this)
{
  /* Remove pending next command. */
  this->CommandNext = RGBLED_NO_CMD;
  this->CommandNext_Handle = &NullCmdHandle;
}
