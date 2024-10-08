#ifndef ADC_DEAKIN_H
#define ADC_DEAKIN_H

#include <Arduino.h>

class ADC_DEAKIN {
public:
    // Constructor
    ADC_DEAKIN(int pin);

    // Function to read the analog value
    int analogRead();

    // Function to read voltage (assuming a 5V reference)
    float readVoltage();

private:
    int pinNumber; // The pin number for the ADC
};

#endif
