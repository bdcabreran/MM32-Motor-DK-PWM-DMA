#include "detect_switch.h"

void DETSW_Init(DETSW_Handle_t *this)
{
  /* Nothing to do. */
}

void DETSW_AttachEventCallback(DETSW_Handle_t *this, DETSW_Event_Callback Event_CB)
{
  this->Event_CB = Event_CB;
}

void DETSW_Reset(DETSW_Handle_t *this)
{
  this->State = (DETSW_State_Handle_t){DETSW_INACTIVE, this->State.Now, 0};
}

bool DETSW_Update(DETSW_Handle_t *this, bool IsActive)
{
  bool IsActiveReturn = false;

  if (this->State.Counter > 0)
  {
    this->State.Counter--;
  }

  switch (this->State.Now)
  {
  case DETSW_INACTIVE:
  {
    if (IsActive == true)
    {
      this->State = (DETSW_State_Handle_t){DETSW_INACTIVE_TO_ACTIVE, this->State.Now, this->ToActiveDelayTicks};
    }
    else
    {
      /* Do nothing. */
    }
  }
  break;

  case DETSW_INACTIVE_TO_ACTIVE:
  {
    if (IsActive == true)
    {
      if (this->State.Counter == 0)
      {
        /* Switch activated. */

        /* Call callback function if it exists. */
        if (this->Event_CB)
        {
          this->Event_CB(DETSW_EVENT_ACTIVE);
        }

        /* Move state. */
        this->State = (DETSW_State_Handle_t){DETSW_ACTIVE, this->State.Now, 0};
      }
      else
      {
        /* Do nothing. */
      }
    }
    else
    {
      /* Move back to inactive state immediately. */
      this->State = (DETSW_State_Handle_t){DETSW_INACTIVE, this->State.Now, 0};
    }
  }
  break;

  case DETSW_ACTIVE_TO_INACTIVE:
  {
    IsActiveReturn = true;

    if (IsActive != true)
    {
      if (this->State.Counter == 0)
      {
        /* Switch now inactive. */

        /* Call callback function if it exists. */
        if (this->Event_CB)
        {
          this->Event_CB(DETSW_EVENT_INACTIVE);
        }

        /* Move state. */
        this->State = (DETSW_State_Handle_t){DETSW_INACTIVE, this->State.Now, 0};
      }
      else
      {
        /* Do nothing. */
      }
    }
    else
    {
      /* Move back to active state immediately. */
      this->State = (DETSW_State_Handle_t){DETSW_ACTIVE, this->State.Now, 0};
    }
  }
  break;

  case DETSW_ACTIVE:
  {
    IsActiveReturn = true;
    if (IsActive != true)
    {
      this->State = (DETSW_State_Handle_t){DETSW_ACTIVE_TO_INACTIVE, this->State.Now, this->ToInactiveDelayTicks};
    }
    else
    {
      /* Do nothing. */
    }
  }
  break;

  default:
    break;
  }

  return IsActiveReturn;
}
