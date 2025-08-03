#pragma once
#include <GyverIO.h>

#include "uButtonVirt.h"

class uButton : public uButtonVirt {
   public:
    uButton(uint8_t pin, uint8_t mode = INPUT_PULLUP) : _pin(pin) {
        pinMode(pin, mode);
    }

    // вызывать в loop. Вернёт true при смене состояния
    bool tick() {
        return uButtonVirt::pollDebounce(readButton());
    }

    // прочитать состояние кнопки
    bool readButton() {
        return !gio::read(_pin);
    }

   private:
    uint8_t _pin;
};