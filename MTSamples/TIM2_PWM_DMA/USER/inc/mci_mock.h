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

#endif // MCI_MOCK_H
