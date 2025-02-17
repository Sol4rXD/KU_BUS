#include <Arduino.h>

#ifndef MILLIS_TIMER_H
#define MILLIS_TIMER_H

#include <Arduino.h>

class Millis {
public:
    Millis(unsigned long interval) : interval(interval), lastMillis(millis()) {}

    operator bool() {
        unsigned long currentMillis = millis();
        if (currentMillis - lastMillis >= interval) {
            lastMillis = currentMillis; // Reset the timer
            return true;
        }
        return false;
    }

private:
    unsigned long interval;
    unsigned long lastMillis;
};

#endif // MILLIS_TIMER_H