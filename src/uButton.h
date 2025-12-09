#pragma once
#include <GyverIO.h>

#include "uButtonVirt.h"

class uButton : public uButtonVirt {
   public:
    uButton(uint8_t pin, uint8_t mode = INPUT_PULLUP) : _pin(pin) {
        gio::init(pin, mode);
    }

    // вызывать в loop. Вернёт true при смене состояния
    bool tick() {
        return uButtonVirt::poll(readButton());
    }

    // опрос без дебаунса, вызывать в loop. Вернёт true при смене состояния
    bool tickRaw() {
        return uButtonVirt::pollRaw(readButton());
    }

    // прочитать состояние кнопки
    bool readButton() {
        return !gio::read(_pin);
    }

   private:
    using uButtonVirt::poll;
    using uButtonVirt::pollRaw;

    uint8_t _pin;
};