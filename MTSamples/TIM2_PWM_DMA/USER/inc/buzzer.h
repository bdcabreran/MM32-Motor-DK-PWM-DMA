#ifndef BUZZER_H__
#define BUZZER_H__

#include "p_type.h"

typedef enum
{
  BZR_OFF = 0,
  BZR_ON,
} BZR_States_t;

typedef struct
{
  BZR_States_t State;
  uint16_t Frequency;
} BZR_ReturnData_t;

typedef struct
{
  bool Active;
  uint16_t OnTicks;
  uint16_t OffTicks;
  int8_t TimesCounter;
  uint16_t Frequency;
} BZR_Command_t;

typedef struct
{
  BZR_States_t State;
  uint16_t Counter;
  BZR_Command_t CommandNow;
  BZR_Command_t CommandNext;
} BZR_Handle_t;

void BZR_Init(BZR_Handle_t *this);
void BZR_Run(BZR_Handle_t *this, BZR_ReturnData_t *ReturnData);
bool BZR_SetOnCommand(
  BZR_Handle_t *this, 
  uint16_t Frequency, 
  int8_t Times, 
  uint16_t TicksDurationOn, 
  uint16_t TicksDurationOff);

#endif
