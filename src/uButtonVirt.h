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

// #ifndef UB_TOUT_TIME
// #define UB_TOUT_TIME 3000  // таймаут события "таймаут"
// #endif

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
        SkipEvents,    // пропускает события [событие]
    };

    uButtonVirt() : _press(0), _steps(0), _clicks(0), _state(static_cast<uint8_t>(State::Idle)) {}

    // сбросить состояние (принудительно закончить обработку)
    void reset() {
        setStateCast(State::Idle);
        _clicks = 0;
        _steps = 0;
    }

    // игнорировать все события до отпускания кнопки
    void skipEvents() {
        if (pressing()) setStateCast(State::SkipEvents);
    }

    // кнопка нажата [событие]
    bool press() {
        return getStateCast() == State::Press;
    }
    bool press(uint8_t clicks) {
        return _clicks == clicks && press();
    }

    // клик по кнопке (отпущена без удержания) [событие]
    bool click() {
        return getStateCast() == State::Click;
    }
    bool click(uint8_t clicks) {
        return _clicks == clicks && click();
    }

    // кнопка была удержана (больше таймаута) [событие]
    bool hold() {
        return getStateCast() == State::Hold;
    }
    bool hold(uint8_t clicks) {
        return _clicks == clicks && hold();
    }

    // кнопка отпущена после удержания [событие]
    bool releaseHold() {
        return getStateCast() == State::ReleaseHold;
    }
    bool releaseHold(uint8_t clicks) {
        return _clicks == clicks && releaseHold();
    }

    // импульсное удержание [событие]
    bool step() {
        return getStateCast() == State::Step;
    }
    bool step(uint8_t clicks) {
        return _clicks == clicks && step();
    }

    // кнопка отпущена после импульсного удержания [событие]
    bool releaseStep() {
        return getStateCast() == State::ReleaseStep;
    }
    bool releaseStep(uint8_t clicks) {
        return _clicks == clicks && releaseStep();
    }

    // кнопка отпущена после удержания или импульсного удержания [событие]
    bool releaseHoldStep() {
        return getStateCast() == State::ReleaseStep || getStateCast() == State::ReleaseHold;
    }
    bool releaseHoldStep(uint8_t clicks) {
        return _clicks == clicks && releaseHoldStep();
    }

    // кнопка отпущена (в любом случае) [событие]
    bool release() {
        return getStateCast() == State::Release;
    }
    bool release(uint8_t clicks) {
        return _clicks == clicks && release();
    }

    // зафиксировано несколько кликов [событие]
    bool hasClicks() {
        return getStateCast() == State::Clicks;
    }
    bool hasClicks(uint8_t clicks) {
        return _clicks == clicks && hasClicks();
    }

    // вышел таймаут [событие]
    bool timeout() {
        return getStateCast() == State::Timeout;
    }

    // вышел таймаут после взаимодействия с кнопкой
    bool timeout(uint16_t ms) {
        if (getStateCast() == State::WaitTimeout && _getTime() >= ms) {
            setStateCast(State::Timeout);
            return true;
        }
        return false;
    }

    // кнопка зажата (между press() и release()) [состояние]
    bool pressing() {
        switch (getStateCast()) {
            case State::Press:
            case State::WaitHold:
            case State::Hold:
            case State::WaitStep:
            case State::Step:
            case State::WaitNextStep:
            case State::SkipEvents:
                return true;

            default:
                return false;
        }
    }
    bool pressing(uint8_t clicks) {
        return _clicks == clicks && pressing();
    }

    // кнопка удерживается (после hold()) [состояние]
    bool holding() {
        switch (getStateCast()) {
            case State::Hold:
            case State::WaitStep:
            case State::Step:
            case State::WaitNextStep:
                return true;

            default: return false;
        }
    }
    bool holding(uint8_t clicks) {
        return _clicks == clicks && holding();
    }

    // кнопка удерживается (после step()) [состояние]
    bool stepping() {
        switch (getStateCast()) {
            case State::Step:
            case State::WaitNextStep:
                return true;

            default: return false;
        }
    }
    bool stepping(uint8_t clicks) {
        return _clicks == clicks && stepping();
    }

    // кнопка ожидает повторных кликов (между click() и hasClicks()) [состояние]
    bool waiting() {
        return getStateCast() == State::WaitClicks;
    }

    // идёт обработка (между первым нажатием и после ожидания кликов) [состояние]
    bool busy() {
        return getStateCast() != State::Idle;
    }

    // время, которое кнопка удерживается (с начала нажатия), мс
    uint16_t pressFor() {
        switch (getStateCast()) {
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
        switch (getStateCast()) {
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
        switch (getStateCast()) {
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
        return getStateCast();
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
    bool poll(bool pressed) {
        if (_press == pressed) {
            _deb = 0;
        } else {
            if (!_deb) _deb = millis();
            else if (uint8_t(uint8_t(millis()) - _deb) >= UB_DEB_TIME) _press = pressed;
        }
        return pollRaw(_press);
    }

    // обработка. Вернёт true при смене состояния
    bool pollRaw(bool pressed) {
        State pstate = getStateCast();

        switch (getStateCast()) {
            case State::Idle:
                if (pressed) setStateCast(State::Press);
                break;

            case State::Press:
                setStateCast(State::WaitHold);
                _resetTime();
                break;

            case State::WaitHold:
                if (!pressed) {
                    setStateCast(State::Click);
                    ++_clicks;
                } else if (_getTime() >= UB_HOLD_TIME) {
                    setStateCast(State::Hold);
                    _resetTime();
                }
                break;

            case State::Hold:
                setStateCast(State::WaitStep);
                break;

            case State::WaitStep:
                if (!pressed) setStateCast(State::ReleaseHold);
                else if (_getTime() >= UB_STEP_TIME) {
                    setStateCast(State::Step);
                    _resetTime();
                }
                break;

            case State::Step:
                setStateCast(State::WaitNextStep);
                break;

            case State::WaitNextStep:
                if (!pressed) setStateCast(State::ReleaseStep);
                else if (_getTime() >= UB_STEP_PRD) {
                    setStateCast(State::Step);
                    ++_steps;
                    _resetTime();
                }
                break;

            case State::SkipEvents:
                if (!pressed) setStateCast(State::Release);
                break;

            case State::ReleaseHold:
            case State::ReleaseStep:
                setStateCast(State::Release);
                _clicks = 0;
                break;

            case State::Click:
                setStateCast(State::Release);
                break;

            case State::Release:
                _steps = 0;
                setStateCast(_clicks ? State::WaitClicks : State::WaitTimeout);
                _resetTime();
                break;

            case State::WaitClicks:
                if (pressed) setStateCast(State::Press);
                else if (_getTime() >= UB_CLICK_TIME) {
                    setStateCast(State::Clicks);
                    _resetTime();
                }
                break;

            case State::Clicks:
                _clicks = 0;
                setStateCast(State::WaitTimeout);
                break;

            case State::WaitTimeout:
                if (pressed) setStateCast(State::Press);
                // else if (_getTime() >= UB_TOUT_TIME) setStateCast(State::Timeout);
                break;

            case State::Timeout:
                setStateCast(State::Idle);
                break;
        }

        return pstate != getStateCast();
    }

   protected:
    /* Методы-посредники для доступа к защищённой переменной _state.
     * Обеспечивают двустороннее преобразование между типами State (enum class) и uint8_t.
     * Введены из-за того, что объявление переменной _state как относящейся к типу State при использовании узких битовых
     * полей (как `State _state : 4') приводило к предупреждению при компиляции, поскольку GCC требует, чтобы переменная
     * могла вместить полную переменную типа, к которому "привязан" enum class (в данном случае это uint8_t),
     * а не только фактически представленные в enum'е значения.
     */
    State getStateCast() const {
        return static_cast<State>(_state);
    }
    void setStateCast(State new_state) {
        _state = static_cast<uint8_t>(new_state);
    }

    void skipToTimeout() {
        _resetTime();
        setStateCast(State::WaitTimeout);
    }
    void skipToRelease() {
        setStateCast(State::SkipEvents);
    }

   protected:
    uint16_t _tmr = 0;
    uint8_t _deb = 0;
    uint8_t _press : 1;
    uint8_t _steps : 7;
    uint8_t _clicks : 4;
    uint8_t _state : 4;

    uint16_t _getTime() {
        return uint16_t(millis()) - _tmr;
    }
    void _resetTime() {
        _tmr = millis();
    }
};
