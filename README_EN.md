This is an automatic translation and may be incorrect in some places. See the source README and examples for authoritative information.

[![latest](https://img.shields.io/github/v/release/GyverLibs/uButton.svg?color=brightgreen)](https://github.com/GyverLibs/uButton/releases/latest/download/uButton.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/uButton.svg)](https://registry.platformio.org/libraries/gyverlibs/uButton)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/uButton?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# uButton
Another button library for Arduino. Used for "another" encoder library[uEncoder](https://github.com/GyverLibs/uEncoder).

On the API, it's 99% equivalent.[EncButton](https://github.com/GyverLibs/EncButton), but slightly lighter (6 bytes of RAM per button and ~650 bytes of flash drive) and written in a more readable finite-automatic style, rather than on flags.

Weight in Bytes compared to other libraries (survey of five button events):

|              | [uButton](https://github.com/GyverLibs/uButton) | [EncButton](https://github.com/GyverLibs/EncButton) | [EncButton opti](https://github.com/GyverLibs/EncButton) | [GyverButton](https://github.com/GyverLibs/GyverButton) | [OneButton](https://github.com/mathertel/OneButton) | [OneButtonTiny](https://github.com/mathertel/OneButton) | [JC_Button](https://github.com/JChristensen/JC_Button) | [AceButton](https://github.com/bxparks/AceButton)|
----------------------------------------------------------------------------------------
| Flash | 776 | 1006 | 610 | 1054 | 1838 | 1152 | 650 | 1482 |
| SRAM | 6 | 15 | 6 | 23 | 83 | 36 | 24 | 45 |
| Feech | Very Many (20+) | Very Many (20+) | Very Many (20+) | Very Many (13) | Average (8) | Very Little (4) | Extremely Little (6) | Average (7|)

### Compatibility
Compatible with all Arduino platforms (Arduino features are used)

### Dependencies
- GyverIO

## Contents
- [Use of use](#usage)
- [Versions](#versions)
- [Installation](#install)
- [Bugs and feedback](#feedback)

<a id="usage"></a>

## Use of use
### Define settings
Announce before connecting the library

```cpp
#define UB_DEB_TIME 50      // banus
#define UB_HOLD_TIME 600    // time before the transition to a "retention" state
#define UB_STEP_TIME 400    // time before the transition to a state of “pulse retention”
#define UB_STEP_PRD 200     // pulse-time
#define UB_CLICK_TIME 500   // click-in
```

### Classes
#### uButtonVirt
Virtual button class.

```cpp
// ==================================================

enum class State {
    Idle,          // idle
    Press,         // pressing
    Click,         // Click (released before retention) [event]
    WaitHold,      // expectation of retention [state]
    Hold,          // retention
    ReleaseHold,   // released to impulses [event]
    WaitStep,      // expectation of impulses [state]
    Step,          // momentum
    WaitNextStep,  // Waiting for the next impulse [state]
    ReleaseStep,   // released after impulses [event]
    Release,       // released (in any case) [event]
    WaitClicks,    // expectation of clicks [state]
    Clicks,        // cliques
    WaitTimeout,   // waiting timeout [state]
    Timeout,       // timeout
};
State getState();   // state
void reset();       // Reset the condition (forced to complete processing)

// =============================================================

// All events except timeout have an overload with the uint8 t clicks argument - the function will return true if the event is "truth" and the number of clicks coincides.

// button pressed [event]
bool press();
bool press(uint8_t clicks);

// Click on the button (released without retention) [event]
bool click();
bool click(uint8_t clicks);

// The button was held (more timeout) [event]
bool hold();
bool hold(uint8_t clicks);

// Button released after holding [event]
bool releaseHold();
bool releaseHold(uint8_t clicks);

// impulse retention [event]
bool step();
bool step(uint8_t clicks);

// button released after impulse retention [event]
bool releaseStep();
bool releaseStep(uint8_t clicks);

// button released after holding or impulse holding [event]
bool releaseHoldStep();
bool releaseHoldStep(uint8_t clicks);

// button released (in any case) [event]
bool release();
bool release(uint8_t clicks);

// A few clicks were recorded [event]
bool hasClicks();
bool hasClicks(uint8_t clicks);

// A timeout came out after the interaction [event]
bool timeout(uint16_t ms);

// =======================================================

// button pressed (between press() and release()) [state]
bool pressing();
bool pressing(uint8_t clicks);

// The button is held (after holding()) [state]
bool holding();
bool holding(uint8_t clicks);

// The button is held (after step())
bool stepping();
bool stepping(uint8_t clicks);

// The button awaits repeated clicks (between click() and hasClick())
bool waiting();

// processing (between the first click and after waiting for clicks)
bool busy();

// =======================================================

// time that the button is held (from the beginning of pressing), MS
uint16_t pressFor();

// The button is held longer than (from the beginning of pressing), ms [state]
bool pressFor(uint16_t ms);

// time that the button is held (from the beginning of retention), MS
uint16_t holdFor();

// The button is held longer than (from the beginning of retention), ms [state]
bool holdFor(uint16_t ms);

// time that the button is held (from the beginning of the step), MS
uint16_t stepFor();

// The button is held longer than (from the beginning of the steppe), MS [state]
bool stepFor(uint16_t ms);

// =======================================================

// click
uint8_t getClicks();

// steppe
uint8_t getSteps();

// ==========================================

// call when the button is pressed in interruption
void pressISR();

// anti-drunk treatment. It will return true when the state changes.
bool poll(bool pressed);

// processing. It will return true when the state changes.
bool pollRaw(bool pressed);
```

#### uButton
Class class`uButton`succeed`uButtonVirt`automatically initializing the pin and sending data from it to`poll`inside`tick`.

```cpp
// The button is connected to GND (open drain)
uButton(uint8_t pin, uint8_t mode = INPUT_PULLUP);

// call in the loop. It will return true when the state changes.
bool tick();

// without debunking, call in the loop. It will return true when the state changes.
bool tickRaw();

// readout
bool readButton();
```

#### uButtonMulti
Handling two button presses`uButton`Like the third button.

```cpp
bool tick(uButton& b0, uButton& b1);
```

## Examples
### Demo
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
        if (b.timeout(2000)) Serial.println("Timeout");

        switch (b.getState()) {
            // ...
        }
    }
}
```

### Two buttons.
```cpp
#include <uButtonMulti.h>

uButton b0(5);
uButton b1(6);
uButtonMulti b2;

void setup() {
    Serial.begin(115200);
}

void loop() {
    // button survey
    b0.tick();
    b1.tick();

    // polling
    b2.tick(b0, b1);

    // eventwork
    if (b0.click()) Serial.println("b0 click");
    if (b0.step()) Serial.println("b0 step");

    if (b1.click()) Serial.println("b1 click");
    if (b1.step()) Serial.println("b1 step");

    if (b2.click()) Serial.println("b0+b1 click");
    if (b2.step()) Serial.println("b0+b1 step");
}
```

<a id="versions"></a>

## Versions
- v1.0

<a id="install"></a>
## Installation
- The library can be found by the name **uButton** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download the library](https://github.com/GyverLibs/uButton/archive/refs/heads/main.zip).zip archive for manual installation:
    - Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/ *
    - (Arduino IDE) Automatic installation from .zip: *Sketch/Connect library/Add .ZIP library...* and specify downloaded archive
- Read more detailed instructions for installing libraries[here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: new versions fix errors and bugs, as well as optimize and add new features.
- Through the library manager IDE: find the library as when installing and click "Update"
- Manually: **Delete the folder with the old version** and then put the new one in its place. “Replacement” can not be done: sometimes new versions delete files that will remain when replaced and can lead to errors!

<a id="feedback"></a>

## Bugs and feedback
If you find bugs, create **Issue**, or better write to the mail immediately.[alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
The library is open for revision and your **Pull Requests*!

When reporting bugs or incorrect work of the library, it is necessary to specify:
- Library version
- What is used by the IC
- SDK version (for ESP)
- Arduino IDE version
- Are embedded examples that use features and designs that cause bugs in your code working correctly?
- What code was downloaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed. Not a canvas of a thousand lines, but a minimum code.
