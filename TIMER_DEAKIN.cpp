#include "TIMER_DEAKIN.h"

// Constructor
TIMER_DEAKIN::TIMER_DEAKIN() : startTime(0), interval(0), running(false) {}

// Start the timer with a specified interval (in milliseconds)
void TIMER_DEAKIN::start(unsigned long intervalMs) {
    interval = intervalMs;
    startTime = millis();
    running = true;
}

// Check if the timer has elapsed
bool TIMER_DEAKIN::hasElapsed() {
    if (!running) return false;
    if (millis() - startTime >= interval) {
        startTime = millis(); // Reset start time for next interval
        return true;
    }
    return false;
}

// Stop the timer
void TIMER_DEAKIN::stop() {
    running = false;
}

// Reset the timer
void TIMER_DEAKIN::reset() {
    startTime = millis();
}
