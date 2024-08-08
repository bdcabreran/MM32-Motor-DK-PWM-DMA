#include "button.h"

__WEAK void BTN_Init(Button_Handle_t *this)
{
  /* Nothing to do. */
}

__WEAK void BTN_Reset(Button_Handle_t *this)
{
  this->WaitingForDoublePress = false;
  this->State = (ButtonState_Handle_t){Button_UP, 0};
}

__WEAK void BTN_RegisterCallback_ShortPress(Button_Handle_t *this, Button_Event_Callback CB)
{
  this->OnShortPress = CB;
}

__WEAK void BTN_RegisterCallback_LongPress(Button_Handle_t *this, Button_Event_Callback CB)
{
  this->OnLongPress = CB;
}

__WEAK void BTN_RegisterCallback_DoublePress(Button_Handle_t *this, Button_Event_Callback CB)
{
  this->OnDoublePress = CB;
}

__WEAK void BTN_RegisterCallback_Down(Button_Handle_t *this, Button_Event_Callback CB)
{
  this->OnDown = CB;
}

__WEAK void BTN_RegisterCallback_Up(Button_Handle_t *this, Button_Event_Callback CB)
{
  this->OnUp = CB;
}

__WEAK void BTN_Update(Button_Handle_t *this, bool IsButtonDown)
{
  if (this->State.Counter > 0)
  {
    this->State.Counter--;
  }

  switch (this->State.Value)
  {
  case Button_UP:
  {
    if (IsButtonDown == true)
    {
      this->State = (ButtonState_Handle_t){Button_UPtoDOWN, this->FilterTicks};
    }
    else
    {
      /* Button was up and still is.*/
      
      /* Check if double press functionality is enabled. */
      if (this->DoublePressEnabled == true)
      {
        /* Check if we're waiting for double press. */
        if (this->WaitingForDoublePress == true)
        {
          /* Check if double press wait timer lapses. */
          if (this->State.Counter == 0)
          {
            /* Last press wasn't a double press, assume it is a short press
              and call callback if it exists. */
            this->WaitingForDoublePress = false;
            if (this->OnShortPress)
            {
              this->OnShortPress();
            }
            else
            {
              /* Do nothing. */
            }
            break; /* Break immediately. */
          }
          else
          {
            /* Do nothing. */
          }
        }
        else
        {
          /* Do nothing. */
        }
      }
    }
  }
  break;

  case Button_UPtoDOWN:
  {
    if (IsButtonDown == true)
    {
      /* Button is still down. Check if it passes filter. */
      if (this->State.Counter == 0)
      {
        /* Button press has passed the filter stage, move to DOWN state. */
        if (this->OnDown)
        {
          this->OnDown();
        }
        this->State = (ButtonState_Handle_t){Button_DOWN, this->LongPressTicks};
      }
      else
      {
        /* Do nothing, waiting for filter. */
      }
    }
    else
    {
      /* False positive. Go back to UP state. 
         Note: this means no OnUp callbacks called here. */

      /* First need to check if double press enabled to ensure timer is set properly
         i.e. if we're in middle of double press action need to have continuity.  */
      if (this->DoublePressEnabled == true)
      {
        /* Check if we're waiting for double press. */
        if (this->WaitingForDoublePress == true)
        {
          /* Keep the same counter value. */
          this->State = (ButtonState_Handle_t){Button_UP, this->State.Counter};
        }
        else
        {
          this->State = (ButtonState_Handle_t){Button_UP, 0};
        }
      }
      else
      {
        this->State = (ButtonState_Handle_t){Button_UP, 0};
      }
    }
  }
  break;

  case Button_DOWN:
  {
    if (IsButtonDown == true)
    {
      /* Button was down and still is.
          Check if it has exceeded Long Press Ticks. */
      if (this->State.Counter == 0)
      {
        /* Long press has been made. Call callback function if it exists. */
        if (this->OnLongPress)
        {
          this->OnLongPress();
        }

        /* Move to FINISHED state. This ensures we don't also register short press upon release. */
        this->State = (ButtonState_Handle_t){Button_FINISHED, 0};
      }
      else
      {
        /* Do nothing. */
      }
    }
    else
    {
      /* Button seems to have been released, move to transition state. */
      this->State = (ButtonState_Handle_t){Button_DOWNtoUP, this->FilterTicks};
    }
  }
  break;

  case Button_DOWNtoUP:
  {
    if (IsButtonDown == false)
    {
      if (this->State.Counter == 0)
      {
        /* Button has been released. */

        if (this->DoublePressEnabled == true)
        {
          /* Check if already waiting for double press event. */
          if (this->WaitingForDoublePress == true)
          {
            /* Button has been double pressed. */

            /* Reset wait boolean. */
            this->WaitingForDoublePress = false;

            /* Call callback if it exists. */
            if (this->OnUp)
            {
              this->OnUp();
            }

            if (this->OnDoublePress)
            {
              this->OnDoublePress();
            }

            this->State = (ButtonState_Handle_t){Button_UP, 0};
          }
          else
          {
            /* Not already waiting for double press, move to up state and wait to see if double press. */
            this->WaitingForDoublePress = true;

            if (this->OnUp)
            {
              this->OnUp();
            }

            this->State = (ButtonState_Handle_t){Button_UP, this->DoublePressWaitTicks};
          }
        }
        else
        {
          /* Double press not enabled, so it's always a short press. */

          /* Call callback function if it exists. */
          if (this->OnUp)
          {
            this->OnUp();
          }

          if (this->OnShortPress)
          {
            this->OnShortPress();
          }

          this->State = (ButtonState_Handle_t){Button_UP, 0};
        }
      }
      else
      {
        /* Do nothing. */
      }
    }
    else
    {
      /* False positive. Button is actually still down, move back to down state. 
        Note: no OnDown callback called here as was false positive. */
      this->State = (ButtonState_Handle_t){Button_DOWN, this->LongPressTicks};
    }
  }
  break;

  case Button_FINISHED:
  {
    /* Waiting for button release to go back to UP state. */
    if (IsButtonDown == false)
    {
      /* No need for transition state or filtering since no functionality
        remains anyway. Move straight back to UP state. */
      if (this->OnUp)
      {
        this->OnUp();
      }
      
      this->State = (ButtonState_Handle_t){Button_UP, 0};
    }
    else
    {
      /* Do nothing. */
    }
  }
  break;

  default:
  {
    break;
  }
  }
}

void BTN_ForceState(Button_Handle_t *this, ButtonStates_t NewState)
{
  this->WaitingForDoublePress = false;

  switch (NewState)
  {
    case Button_UP:
    {
      this->State = (ButtonState_Handle_t){Button_UP, 0};
    }
    break;
    
    case Button_UPtoDOWN:
    {
      this->State = (ButtonState_Handle_t){Button_UPtoDOWN, this->FilterTicks};
    }
    break;

    case Button_DOWN:
    {
      this->State = (ButtonState_Handle_t){Button_DOWN, this->LongPressTicks};
    }
    break;

    case Button_DOWNtoUP:
    {
      this->State = (ButtonState_Handle_t){Button_DOWNtoUP, this->FilterTicks};
    }
    break;

    case Button_FINISHED:
    {
      this->State = (ButtonState_Handle_t){Button_FINISHED, 0};
    }
    break;

    default:
    break;
  }
}

void BTN_ForceSetCounter(Button_Handle_t *this, uint32_t Ticks)
{
  this->State.Counter = Ticks;
}
