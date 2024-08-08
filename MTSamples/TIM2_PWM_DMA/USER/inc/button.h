#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "p_type.h"

/** @brief Button component handle type */
typedef struct Button_Handle Button_Handle_t;

/** @brief Callback function on button press event. */
typedef void (*Button_Event_Callback)(void);

typedef enum
{
  Button_UP = 0,
  Button_UPtoDOWN, /*!< Transition state. Apply filter to see if it's a real button press. */
  Button_DOWN,
  Button_DOWNtoUP, /*!< Transition state. Apply filter to see if it's a real button press. */
  Button_FINISHED,
} ButtonStates_t;

typedef struct
{
  ButtonStates_t Value;
  uint16_t Counter;
} ButtonState_Handle_t;

/// TODO: add ACTIVE_HIGH or ACTIVE_LOW config for different button arrangements.
struct Button_Handle
{
  /******************* Configuration Parameters *******************/
  uint16_t FilterTicks;    /*!< Number of update call ticks to use as a debounce filter. */
  uint16_t LongPressTicks; /*!< Number of update call ticks to use as long button press. */

  bool DoublePressEnabled; /*!< Set to true if double button press functionality desired. */
  uint16_t DoublePressWaitTicks;

  /***************************************************************/

  ButtonState_Handle_t State;

  bool WaitingForDoublePress;

  Button_Event_Callback OnShortPress;
  Button_Event_Callback OnDoublePress;
  Button_Event_Callback OnLongPress;
  Button_Event_Callback OnDown;
  Button_Event_Callback OnUp;
};

void BTN_Init(Button_Handle_t *this);
void BTN_Update(Button_Handle_t *this, bool IsButtonDown);
void BTN_Reset(Button_Handle_t *this);

void BTN_RegisterCallback_ShortPress(Button_Handle_t *this, Button_Event_Callback CB);
void BTN_RegisterCallback_LongPress(Button_Handle_t *this, Button_Event_Callback CB);
void BTN_RegisterCallback_DoublePress(Button_Handle_t *this, Button_Event_Callback CB);
void BTN_RegisterCallback_Down(Button_Handle_t *this, Button_Event_Callback CB);
void BTN_RegisterCallback_Up(Button_Handle_t *this, Button_Event_Callback CB);

void BTN_ForceState(Button_Handle_t *this, ButtonStates_t NewState);
void BTN_ForceSetCounter(Button_Handle_t *this, uint32_t Ticks);

#endif
