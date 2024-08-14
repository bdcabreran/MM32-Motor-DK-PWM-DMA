#include "buzzer.h"
#include <string.h>

void BZR_Init(BZR_Handle_t *this)
{
  this->State = BZR_OFF;
}

void BZR_Run(BZR_Handle_t *this, BZR_ReturnData_t *ReturnData)
{
  switch (this->State)
  {
  case BZR_OFF:
  {
    if (this->Counter > 0)
    {
      this->Counter--;
    }
    else
    {
      if (this->CommandNow.Active == true)
      {
        this->Counter = this->CommandNow.OnTicks;
        this->State = BZR_ON;
      }
      else
      {
        /* Check for new commands. */
        if (this->CommandNext.Active == true)
        {
          memcpy(&this->CommandNow, &this->CommandNext, sizeof(BZR_Command_t));
          this->CommandNext.Active = false;
        }
      }
    }
  }
  break;

  case BZR_ON:
  {
    if (this->Counter > 0)
    {
      this->Counter--;
    }
    else
    {
      if (this->CommandNow.TimesCounter > 1)
      {
        this->CommandNow.TimesCounter--;
        this->Counter = this->CommandNow.OffTicks;
      }
      else
      {
        /* Sequence over. */
        this->CommandNow.Active = false;
      }

      this->State = BZR_OFF;
    }
  }
  break;

  default:
    break;
  }

  ReturnData->Frequency = this->CommandNow.Frequency;
  ReturnData->State = this->State;
}

bool BZR_SetOnCommand(
  BZR_Handle_t *this, 
  uint16_t Frequency, 
  int8_t Times, 
  uint16_t TicksDurationOn, 
  uint16_t TicksDurationOff)
{
  this->CommandNext.Active = true;
  this->CommandNext.OnTicks = TicksDurationOn;
  this->CommandNext.OffTicks = TicksDurationOff;
  this->CommandNext.Frequency = Frequency;
  this->CommandNext.TimesCounter = Times;
  return true;
}
