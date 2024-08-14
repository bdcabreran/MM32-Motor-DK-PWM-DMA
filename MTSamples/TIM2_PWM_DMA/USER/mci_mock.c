#include "mci_mock.h"

// Variables to control the mock behavior
static bool batteryPercentageInitialised = true;
static uint8_t batteryPercentage = 50;
static bool batteryDetected = true;
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

// Functions to set the mock behavior for testing purposes
void MCI_SetMockBatteryPercentageInitialised(bool initialised) {
    batteryPercentageInitialised = initialised;
}

void MCI_SetMockBatteryPercentage(uint8_t percentage) {
    batteryPercentage = percentage;
}

void MCI_SetMockBatteryDetected(bool detected) {
    batteryDetected = detected;
}

void MCI_SetMockCablePlugged(bool plugged) {
    cablePlugged = plugged;
}

void MCI_SetMockNackedFaults(uint8_t faults) {
    nackedFaults = faults;
}

void Buzzer_Disable(void) {
    // Mock implementation: Do nothing
}

void Buzzer_Enable(uint16_t Frequency) {
    // Mock implementation: Do nothing
}
