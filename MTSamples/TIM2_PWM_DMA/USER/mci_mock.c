#include "mci_mock.h"
#include "drv_uart.h"

#define MOCK_DBG_ENABLE 1

#if MOCK_DBG_ENABLE
#include "drv_uart.h"
#include "stdio.h"
#define DEBUG_BUFFER_SIZE 256
static int8_t debugBuffer[DEBUG_BUFFER_SIZE]; // Changed to int8_t
static const char* TAG = "MOCK";

#define MOCK_DBG_MSG(fmt, ...) do { \
  snprintf((char*)debugBuffer, DEBUG_BUFFER_SIZE, "%s: " fmt, TAG, ##__VA_ARGS__); \
  Uart_Put_Buff(debugBuffer, strlen((char*)debugBuffer)); \
} while(0)
#else
#define MOCK_DBG_MSG(fmt, ...) do { } while(0)
#endif

// Variables to control the mock behavior
static bool batteryPercentageInitialised = true;
static uint8_t batteryPercentage = 50;
static bool batteryDetected = false;
static bool cablePlugged = false;
static uint8_t nackedFaults = 0;

MCI_Handle_t Mci;

// Mock implementations

bool MCI_IsBatteryPercentageInitialised(void) {
    return batteryPercentageInitialised;
}

uint8_t MCI_GetBatteryPercentage(void) {
    return batteryPercentage;
}

bool MCI_IsBatteryDetected(void) {
    return batteryDetected;
}

bool MCI_IsCablePlugged(void) {
    return cablePlugged;
}

void MCI_StopMotor(void) {
    // Mock implementation: Do nothing
}

void MCI_AcknowledgeFaults(void) {
    // Mock implementation: Do nothing
}

bool MCI_StartMotor(void) {
    // Mock implementation: Always return true
    return true;
}

void MCI_SetSpeedSetting(uint8_t SpeedSetting) {
    // Mock implementation: Do nothing
}

uint8_t MCI_GetNackedFaults(void) {
    return nackedFaults;
}

void MCI_ResetFaultState(void) {
    // Mock implementation: Reset nacked faults
    nackedFaults = 0;
}

void MCI_Reset() {
    // Mock implementation: Reset all variables
    //batteryPercentageInitialised = true;
    //batteryPercentage = 50;
    //batteryDetected = true;
    //cablePlugged = false;
    //nackedFaults = 0;
}

// Functions to set the mock behavior for testing purposes


void MCI_SetMockBatteryPercentage(uint8_t percentage) {
    batteryPercentage = percentage;
}

void MCI_SetMockBatteryDetected(bool detected) {
    batteryDetected = detected;
}

void Buzzer_Disable(void) {
    // Mock implementation: Do nothing
}

void Buzzer_Enable(uint16_t Frequency) {
    // Mock implementation: Do nothing
}

// Setter functions for mock behavior

void MCI_SetMockBatteryPercentageInitialised(bool initialised) {
    // MOCK_DBG_MSG("Setting battery percentage initialised to %d\n", initialised);
    batteryPercentageInitialised = initialised;
}

void MCI_SetMockCablePlugged(bool plugged) {
    // MOCK_DBG_MSG("Setting cable plugged to %d\n", plugged);
    cablePlugged = plugged;
}

void MCI_SetMockNackedFaults(uint8_t faults) {
    // MOCK_DBG_MSG("Setting nacked faults to %d\n", faults);
    nackedFaults = faults;
}