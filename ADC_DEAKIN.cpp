#include "ADC_DEAKIN.h"

// Constructor
ADC_DEAKIN::ADC_DEAKIN(int pin) : pinNumber(pin) {
    pinMode(pinNumber, INPUT);
}

// Function to read the analog value
int ADC_DEAKIN::analogRead() {
    return ::analogRead(pinNumber);
}

// Function to read voltage (assuming a 5V reference)
float ADC_DEAKIN::readVoltage() {
    int value = analogRead();
    return (value / 1023.0) * 5.0; // Convert to voltage
}
