#pragma once
#include <GyverIO.h>

#include "uButtonVirt.h"

class uButton : public uButtonVirt {
   public:
    uButton(uint8_t pin, uint8_t mode = INPUT_PULLUP) : _pin(pin) {
        pinMode(pin, mode);
    }

    bool tick() {
        return uButtonVirt::pollDebounce(!gio::read(_pin));
    }

   private:
    uint8_t _pin;
};