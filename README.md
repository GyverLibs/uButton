[![latest](https://img.shields.io/github/v/release/GyverLibs/uButton.svg?color=brightgreen)](https://github.com/GyverLibs/uButton/releases/latest/download/uButton.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/uButton.svg)](https://registry.platformio.org/libraries/gyverlibs/uButton)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/uButton?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# uButton
Очередной класс кнопки для Arduino.

По API это на 99% аналог [EncButton](https://github.com/GyverLibs/EncButton), но чуточку легче (6 байт RAM на кнопку и ~650 байт флешки) и написан в более читаемом конечно-автоматном стиле, а не на флагах.

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

### Зависимости
- GyverIO

## Содержание
- [Использование](#usage)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="usage"></a>

## Использование
### Дефайны настроек
Объявлять до подключения библиотеки

```cpp
#define UB_DEB_TIME 50      // дебаунс
#define UB_HOLD_TIME 600    // время до перехода в состояние "удержание"
#define UB_STEP_TIME 400    // время до перехода в состояние "импульсное удержание"
#define UB_STEP_PRD 200     // период импульсов
#define UB_CLICK_TIME 500   // ожидание кликов
#define UB_TOUT_TIME 1000   // таймаут события "таймаут"
```

### Классы
#### uButtonVirt
Класс виртуальной кнопки.

```cpp
// =============== СОСТОЯНИЕ ===============

enum class State {
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
State getState();   // получить текущее состояние
void reset();       // сбросить состояние (принудительно закончить обработку)

// =============== СОБЫТИЯ ===============

// У всех событий кроме timeout есть перегрузка с аргументом uint8_t clicks - функция вернёт true, если событие "истино" и количество кликов совподает

// кнопка нажата [событие]
bool press();
bool press(uint8_t clicks);

// клик по кнопке (отпущена без удержания) [событие]
bool click();
bool click(uint8_t clicks);

// кнопка была удержана (больше таймаута) [событие]
bool hold();
bool hold(uint8_t clicks);

// кнопка отпущена после удержания [событие]
bool releaseHold();
bool releaseHold(uint8_t clicks);

// импульсное удержание [событие]
bool step();
bool step(uint8_t clicks);

// кнопка отпущена после импульсного удержания [событие]
bool releaseStep();
bool releaseStep(uint8_t clicks);

// кнопка отпущена после удержания или импульсного удержания [событие]
bool releaseHoldStep();
bool releaseHoldStep(uint8_t clicks);

// кнопка отпущена (в любом случае) [событие]
bool release();
bool release(uint8_t clicks);

// зафиксировано несколько кликов [событие]
bool hasClicks();
bool hasClicks(uint8_t clicks);

// после взаимодействия с кнопкой, мс [событие]
bool timeout();

// =============== СОСТОЯНИЯ ===============

// вышел таймаут после взаимодействия с кнопкой, но меньше чем системный UB_TOUT_TIME
bool timeout(uint16_t ms);

// кнопка зажата (между press() и release()) [состояние]
bool pressing();
bool pressing(uint8_t clicks);

// кнопка удерживается (после hold()) [состояние]
bool holding();
bool holding(uint8_t clicks);

// кнопка удерживается (после step()) [состояние]
bool stepping();
bool stepping(uint8_t clicks);

// кнопка ожидает повторных кликов (между click() и hasClicks()) [состояние]
bool waiting();

// идёт обработка (между первым нажатием и после ожидания кликов) [состояние]
bool busy();

// =============== ВРЕМЯ ===============

// время, которое кнопка удерживается (с начала нажатия), мс
uint16_t pressFor();

// кнопка удерживается дольше чем (с начала нажатия), мс [состояние]
bool pressFor(uint16_t ms);

// время, которое кнопка удерживается (с начала удержания), мс
uint16_t holdFor();

// кнопка удерживается дольше чем (с начала удержания), мс [состояние]
bool holdFor(uint16_t ms);

// время, которое кнопка удерживается (с начала степа), мс
uint16_t stepFor();

// кнопка удерживается дольше чем (с начала степа), мс [состояние]
bool stepFor(uint16_t ms);

// =============== ЗНАЧЕНИЯ ===============

// получить количество кликов
uint8_t getClicks();

// получить количество степов
uint8_t getSteps();

// =============== ОБРАБОТКА ===============

// вызвать, когда кнопка нажата в прерывании
void pressISR();

// обработка с антидребезгом. Вернёт true при смене состояния
bool pollDebounce(bool pressed);

// обработка. Вернёт true при смене состояния
bool poll(bool pressed);
```

#### uButton
Ккласс `uButton` наследует `uButtonVirt`, автоматически инициализируя пин и отправляя данные с него в `pollDebounce` внутри `tick`.

```cpp
// кнопка подключается на GND (open drain)
uButton(uint8_t pin, uint8_t mode = INPUT_PULLUP);

// вызывать в loop. Вернёт true при смене состояния
bool tick();

// прочитать состояние кнопки
bool readButton();
```

## Примеры
```cpp
#include <uButton.h>

uButton b(3);

void setup() {
    Serial.begin(115200);
    Serial.println("start");
}

void loop() {
    if (b.tick()) {
        if (b.press()) Serial.println("Press");
        if (b.click()) Serial.println("Click");
        if (b.hold()) Serial.println("Hold");
        if (b.releaseHold()) Serial.println("ReleaseHold");
        if (b.step()) Serial.println("Step");
        if (b.releaseStep()) Serial.println("releaseStep");
        if (b.release()) Serial.println("Release");
        if (b.hasClicks()) Serial.print("Clicks: "), Serial.println(b.getClicks());
        if (b.timeout()) Serial.println("Timeout");

        switch (b.getState()) {
            // ...
        }
    }
}
```

<a id="versions"></a>

## Версии
- v1.0

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **uButton** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/uButton/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="feedback"></a>

## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код