#ifndef MCI_MOCK_H
#define MCI_MOCK_H

#include <stdint.h>
#include <stdbool.h>

// Mock definitions for MCI functions

#define MOTOR_RESPONSE_TIMEOUT (100U)
#define MCI_SYNC_INTERVAL (100)

/** @name Fault source error codes */
#define MC_NO_FAULTS ((uint16_t)0x0000) /**< 0 | @brief No error.*/
#define MC_FAULT_COMM ((uint8_t)0x01)
#define MC_FAULT_MOTOR ((uint8_t)0x02)
#define MC_FAULT_BRAKE ((uint8_t)0x04)
#define MC_FAULT_OC ((uint8_t)0x08)
#define MC_FAULT_CTRL ((uint8_t)0x10)
#define MC_FAULT_BATT ((uint8_t)0x20)


typedef struct
{
  uint32_t LastSyncTime;
  bool MotorOn;
  uint8_t SpeedSetting;
  uint8_t PrevMode;
  uint8_t BatteryPercentage;
  bool FirstSOCDataReceived;
  uint8_t NackedFaults;
  uint8_t AllFaults;

  bool SendFaultAck;

  bool CablePlugged;

  bool BatteryDetected;

  // Transaction IDs for messages
  int8_t MotorOnTransactionId;
  int8_t MotorModeTransactionId;
  int8_t BatterySocTransactionId;
  int8_t FaultAckTransactionId;
  int8_t GetFaultTransactionId;
  int8_t GetBatteryStatusTransactionId;
  int8_t GetBatteryDetectedTransactionId;
#ifdef TEST_MODAC_COMMANDS
  int8_t TestingTransactionID;
  int32_t TestingReceivedData;
#endif
} MCI_Handle_t;


extern MCI_Handle_t Mci;


bool MCI_IsBatteryPercentageInitialised(void);
uint8_t MCI_GetBatteryPercentage(void);
bool MCI_IsBatteryDetected(void);
bool MCI_IsCablePlugged(void);
void MCI_StopMotor(void);
void MCI_AcknowledgeFaults(void);
bool MCI_StartMotor(void);
void MCI_SetSpeedSetting(uint8_t SpeedSetting);
uint8_t MCI_GetNackedFaults(void);
void MCI_ResetFaultState(void);

void Buzzer_Disable(void);
void Buzzer_Enable(uint16_t Frequency);

#endif // MCI_MOCK_H
