#ifndef GPIO_DEAKIN_H
#define GPIO_DEAKIN_H

#include <Arduino.h>

class GPIO_DEAKIN {
public:
    // Constructor
    GPIO_DEAKIN(int pin);

    // Set pin HIGH
    void setHigh();

    // Set pin LOW
    void setLow();

    // Toggle pin state
    void toggle();

    // Check if pin is HIGH
    bool isHigh();

    // Check if pin is LOW
    bool isLow();

    // Configure pin mode
    void setMode(uint8_t mode);

private:
    int pinNumber; // The pin number for GPIO
};

#endif
