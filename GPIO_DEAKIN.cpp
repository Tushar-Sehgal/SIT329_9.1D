#include "GPIO_DEAKIN.h"

// Constructor
GPIO_DEAKIN::GPIO_DEAKIN(int pin) : pinNumber(pin) {
    pinMode(pinNumber, OUTPUT); // Default to OUTPUT mode
}

// Set pin HIGH
void GPIO_DEAKIN::setHigh() {
    digitalWrite(pinNumber, HIGH);
}

// Set pin LOW
void GPIO_DEAKIN::setLow() {
    digitalWrite(pinNumber, LOW);
}

// Toggle pin state
void GPIO_DEAKIN::toggle() {
    digitalWrite(pinNumber, !digitalRead(pinNumber));
}

// Check if pin is HIGH
bool GPIO_DEAKIN::isHigh() {
    return digitalRead(pinNumber) == HIGH;
}

// Check if pin is LOW
bool GPIO_DEAKIN::isLow() {
    return digitalRead(pinNumber) == LOW;
}

// Configure pin mode
void GPIO_DEAKIN::setMode(uint8_t mode) {
    pinMode(pinNumber, mode);
}
