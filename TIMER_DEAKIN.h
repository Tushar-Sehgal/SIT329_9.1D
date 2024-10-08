#ifndef TIMER_DEAKIN_H
#define TIMER_DEAKIN_H

#include <Arduino.h>

class TIMER_DEAKIN {
public:
    // Constructor
    TIMER_DEAKIN();

    // Start the timer with a specified interval (in milliseconds)
    void start(unsigned long intervalMs);

    // Check if the timer has elapsed
    bool hasElapsed();

    // Stop the timer
    void stop();

    // Reset the timer
    void reset();

private:
    unsigned long startTime; // The time when the timer was started
    unsigned long interval;   // The interval for the timer
    bool running;             // Status of the timer
};

#endif
