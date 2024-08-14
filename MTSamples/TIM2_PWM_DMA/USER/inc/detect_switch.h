#ifndef __DETECT_SWITCH_H
#define __DETECT_SWITCH_H

#include "p_type.h"

typedef enum
{
  DETSW_EVENT_INACTIVE = 0,
  DETSW_EVENT_ACTIVE = 1,
} DETSW_EventIDs_t;

/** @brief Callback function for event detection. */
typedef void (*DETSW_Event_Callback)(DETSW_EventIDs_t EventID);

typedef enum
{
  DETSW_INACTIVE = 0,       /*!< Switch is inactive. */
  DETSW_INACTIVE_TO_ACTIVE, /*!< Transition state acting as filter. */
  DETSW_ACTIVE_TO_INACTIVE, /*!< Transition state acting as filter. */
  DETSW_ACTIVE,             /*!< Switch is active. */
} DETSW_States_t;

typedef struct
{
  DETSW_States_t Now;
  DETSW_States_t Prev;
  uint32_t Counter;
} DETSW_State_Handle_t;

typedef struct
{
  /******************* Configuration Parameters *******************/
  uint32_t ToActiveDelayTicks;   /*!< Number of DETSW_Update call ticks
                                      of switch being pressed before state
                                      moves to DETSW_ACTIVE. */
  uint32_t ToInactiveDelayTicks; /*!< Number of DETSW_Update call ticks
                                      of switch not being pressed before state
                                      moves to DETSW_ACTIVE. */

  /***************************************************************/
  DETSW_Event_Callback Event_CB; /*!< Called whenever a switch event occurs. */

  DETSW_State_Handle_t State; /*!< Current state handle for the module. */

} DETSW_Handle_t;

void DETSW_Init(DETSW_Handle_t *this);
void DETSW_AttachEventCallback(DETSW_Handle_t *this, DETSW_Event_Callback Event_CB);
void DETSW_Reset(DETSW_Handle_t *this);
bool DETSW_Update(DETSW_Handle_t *this, bool IsActive);

#endif
