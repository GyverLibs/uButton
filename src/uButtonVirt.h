#pragma once
#include <Arduino.h>

#ifndef UB_DEB_TIME
#define UB_DEB_TIME 50  // дебаунс
#endif

#ifndef UB_HOLD_TIME
#define UB_HOLD_TIME 600  // время до перехода в состояние "удержание"
#endif

#ifndef UB_STEP_TIME
#define UB_STEP_TIME 400  // время до перехода в состояние "импульсное удержание"
#endif

#ifndef UB_STEP_PRD
#define UB_STEP_PRD 200  // период импульсов
#endif

#ifndef UB_CLICK_TIME
#define UB_CLICK_TIME 500  // ожидание кликов
#endif

#ifndef UB_TOUT_TIME
#define UB_TOUT_TIME 1000  // таймаут события "таймаут"
#endif

class uButtonVirt {
   public:
    enum class State : uint8_t {
        Idle,          // простаивает [состояние]
        Press,         // нажатие [событие]
        Click,         // клик (отпущено до удержания) [событие]
        WaitHold,      // ожидание удержания [состояние]
        Hold,          // удержание [событие]
        ReleaseHold,   // отпущено до импульсов [событие]
        WaitStep,      // ожидание импульсов [состояние]
        Step,          // импульс [событие]
        WaitNextStep,  // ожидание следующего импульса [состояние]
        ReleaseStep,   // отпущено после импульсов [событие]
        Release,       // отпущено (в любом случае) [событие]
        WaitClicks,    // ожидание кликов [состояние]
        Clicks,        // клики [событие]
        WaitTimeout,   // ожидание таймаута [состояние]
        Timeout,       // таймаут [событие]
    };

    uButtonVirt() : _press(0), _steps(0), _state(State::Idle), _clicks(0) {}

    // сбросить состояние (принудительно закончить обработку)
    void reset() {
        _state = State::Idle;
        _clicks = 0;
        _steps = 0;
    }

    // кнопка нажата [событие]
    bool press() {
        return _state == State::Press;
    }
    bool press(uint8_t clicks) {
        return press() && _clicks == clicks;
    }

    // клик по кнопке (отпущена без удержания) [событие]
    bool click() {
        return _state == State::Click;
    }
    bool click(uint8_t clicks) {
        return click() && _clicks == clicks;
    }

    // кнопка была удержана (больше таймаута) [событие]
    bool hold() {
        return _state == State::Hold;
    }
    bool hold(uint8_t clicks) {
        return hold() && _clicks == clicks;
    }

    // кнопка отпущена после удержания [событие]
    bool releaseHold() {
        return _state == State::ReleaseHold;
    }
    bool releaseHold(uint8_t clicks) {
        return releaseHold() && _clicks == clicks;
    }

    // импульсное удержание [событие]
    bool step() {
        return _state == State::Step;
    }
    bool step(uint8_t clicks) {
        return step() && _clicks == clicks;
    }

    // кнопка отпущена после импульсного удержания [событие]
    bool releaseStep() {
        return _state == State::ReleaseStep;
    }
    bool releaseStep(uint8_t clicks) {
        return releaseStep() && _clicks == clicks;
    }

    // кнопка отпущена после удержания или импульсного удержания [событие]
    bool releaseHoldStep() {
        return _state == State::ReleaseStep || _state == State::ReleaseHold;
    }
    bool releaseHoldStep(uint8_t clicks) {
        return releaseHoldStep() && _clicks == clicks;
    }

    // кнопка отпущена (в любом случае) [событие]
    bool release() {
        return _state == State::Release;
    }
    bool release(uint8_t clicks) {
        return release() && _clicks == clicks;
    }

    // зафиксировано несколько кликов [событие]
    bool hasClicks() {
        return _state == State::Clicks;
    }
    bool hasClicks(uint8_t clicks) {
        return hasClicks() && _clicks == clicks;
    }

    // после взаимодействия с кнопкой, мс [событие]
    bool timeout() {
        return _state == State::Timeout;
    }

    // вышел таймаут после взаимодействия с кнопкой, но меньше чем системный UB_TOUT_TIME
    bool timeout(uint16_t ms) {
        if (_state == State::WaitTimeout && _getTime() >= ms) {
            _state = State::Idle;
            return true;
        }
        return false;
    }

    // кнопка зажата (между press() и release()) [состояние]
    bool pressing() {
        switch (_state) {
            case State::Press:
            case State::WaitHold:
            case State::Hold:
            case State::WaitStep:
            case State::Step:
            case State::WaitNextStep:
                return true;

            default:
                return false;
        }
    }
    bool pressing(uint8_t clicks) {
        return pressing() && _clicks == clicks;
    }

    // кнопка удерживается (после hold()) [состояние]
    bool holding() {
        switch (_state) {
            case State::Hold:
            case State::WaitStep:
            case State::Step:
            case State::WaitNextStep:
                return true;

            default: return false;
        }
    }
    bool holding(uint8_t clicks) {
        return holding() && _clicks == clicks;
    }

    // кнопка удерживается (после step()) [состояние]
    bool stepping() {
        switch (_state) {
            case State::Step:
            case State::WaitNextStep:
                return true;

            default: return false;
        }
    }
    bool stepping(uint8_t clicks) {
        return stepping() && _clicks == clicks;
    }

    // кнопка ожидает повторных кликов (между click() и hasClicks()) [состояние]
    bool waiting() {
        return _state == State::WaitClicks;
    }

    // идёт обработка (между первым нажатием и после ожидания кликов) [состояние]
    bool busy() {
        return _state != State::Idle;
    }

    // время, которое кнопка удерживается (с начала нажатия), мс
    uint16_t pressFor() {
        switch (_state) {
            case State::WaitHold:
                return _getTime();

            case State::Hold:
            case State::WaitStep:
            case State::Step:
            case State::WaitNextStep:
                return UB_HOLD_TIME + holdFor();

            default: return 0;
        }
    }

    // кнопка удерживается дольше чем (с начала нажатия), мс [состояние]
    bool pressFor(uint16_t ms) {
        return pressFor() >= ms;
    }

    // время, которое кнопка удерживается (с начала удержания), мс
    uint16_t holdFor() {
        switch (_state) {
            case State::WaitStep:
                return _getTime();

            case State::Step:
            case State::WaitNextStep:
                return UB_STEP_TIME + stepFor();

            default:
                return 0;
        }
    }

    // кнопка удерживается дольше чем (с начала удержания), мс [состояние]
    bool holdFor(uint16_t ms) {
        return holdFor() >= ms;
    }

    // время, которое кнопка удерживается (с начала степа), мс
    uint16_t stepFor() {
        switch (_state) {
            case State::Step:
            case State::WaitNextStep:
                return _steps * UB_STEP_PRD + _getTime();

            default:
                return 0;
        }
    }

    // кнопка удерживается дольше чем (с начала степа), мс [состояние]
    bool stepFor(uint16_t ms) {
        return stepFor() >= ms;
    }

    // получить текущее состояние
    State getState() {
        return _state;
    }

    // получить количество кликов
    uint8_t getClicks() {
        return _clicks;
    }

    // получить количество степов
    uint8_t getSteps() {
        return _steps;
    }

    // кнопка нажата в прерывании
    void pressISR() {
        _press = 1;
        _deb = millis();
    }

    // обработка с антидребезгом. Вернёт true при смене состояния
    bool pollDebounce(bool pressed) {
        if (_press == pressed) {
            _deb = 0;
        } else {
            if (!_deb) _deb = millis();
            else if (uint8_t(uint8_t(millis()) - _deb) >= UB_DEB_TIME) _press = pressed;
        }
        return poll(_press);
    }

    // обработка. Вернёт true при смене состояния
    bool poll(bool pressed) {
        State pstate = _state;

        switch (_state) {
            case State::Idle:
                if (pressed) _state = State::Press;
                break;

            case State::Press:
                _state = State::WaitHold;
                _resetTime();
                break;

            case State::WaitHold:
                if (!pressed) {
                    _state = State::Click;
                    ++_clicks;
                } else if (_getTime() >= UB_HOLD_TIME) {
                    _state = State::Hold;
                    _resetTime();
                }
                break;

            case State::Hold:
                _state = State::WaitStep;
                break;

            case State::WaitStep:
                if (!pressed) _state = State::ReleaseHold;
                else if (_getTime() >= UB_STEP_TIME) {
                    _state = State::Step;
                    _resetTime();
                }
                break;

            case State::Step:
                _state = State::WaitNextStep;
                break;

            case State::WaitNextStep:
                if (!pressed) _state = State::ReleaseStep;
                else if (_getTime() >= UB_STEP_PRD) {
                    _state = State::Step;
                    ++_steps;
                    _resetTime();
                }
                break;

            case State::ReleaseHold:
            case State::ReleaseStep:
                _clicks = 0;
                // fall

            case State::Click:
                _state = State::Release;
                break;

            case State::Release:
                _steps = 0;
                _state = _clicks ? State::WaitClicks : State::WaitTimeout;
                _resetTime();
                break;

            case State::WaitClicks:
                if (pressed) _state = State::Press;
                else if (_getTime() >= UB_CLICK_TIME) {
                    _state = State::Clicks;
                    _resetTime();
                }
                break;

            case State::Clicks:
                _clicks = 0;
                _state = State::WaitTimeout;
                break;

            case State::WaitTimeout:
                if (pressed) _state = State::Press;
                else if (_getTime() >= UB_TOUT_TIME) _state = State::Timeout;
                break;

            case State::Timeout:
                _state = State::Idle;
                break;
        }

        return pstate != _state;
    }

   private:
    uint16_t _tmr = 0;
    uint8_t _deb = 0;
    uint8_t _press : 1;
    uint8_t _steps : 7;
    State _state : 4;
    uint8_t _clicks : 4;

    uint16_t _getTime() {
        return uint16_t(millis()) - _tmr;
    }
    void _resetTime() {
        _tmr = millis();
    }
};