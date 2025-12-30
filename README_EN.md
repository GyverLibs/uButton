This is an automatic translation, may be incorrect in some places. See sources and examples!

#uButton
Another button class for Arduino.

In terms of API, this is 99% analogous to [EncButton](https://github.com/GyverLibs/EncButton), but a little lighter (6 bytes of RAM per button and ~650 bytes of flash drive) and written in a more readable finite-automatic style, rather than using flags.

### Compatibility
Compatible with all Arduino platforms (uses Arduino functions)

### Dependencies
-GyverIO

## Contents
- [Usage](#usage)
- [Versions](#versions)
- [Installation](#install)
- [Bugs and feedback](#feedback)

<a id="usage"></a>

## Usage
### Settings Defines
Declare before connecting the library

```cpp
#define UB_DEB_TIME 50 // debounce
#define UB_HOLD_TIME 600 // time before entering the "hold" state
#define UB_STEP_TIME 400 // time before entering the "pulse hold" state
#define UB_STEP_PRD 200 // pulse period
#define UB_CLICK_TIME 500 // wait for clicks
```

### Classes
#### uButtonVirt
Virtual button class.

```cpp
// =============== STATUS ===============

enum class State {
Idle, // idle [state]
Press, // pressing [event]
Click, // click (released before holding) [event]
WaitHold, // waiting for hold [state]
Hold, // hold [event]
ReleaseHold, // released until pulses [event]
WaitStep, // waiting for pulses [state]
Step, // impulse [event]
WaitNextStep, // waiting for the next impulse [state]
ReleaseStep, // released after impulses [event]
Release, // released (in any case) [event]
WaitClicks, // waiting for clicks [state]
Clicks, // clicks [event]
WaitTimeout, // waiting for timeout [state]
Timeout, // timeout [event]
};
State getState();// get the current state
void reset();// reset the state (forcefully end processing)

// =============== EVENTS ===============

// All events except timeout have an overload with the argument uint8_t clicks - the function will return true if the event is "true" and the number of clicks matches

// button pressed [event]
bool press();
bool press(uint8_t clicks);

// click on the button (released without holding) [event]
bool click();
bool click(uint8_t clicks);

// button was held (more than timeout) [event]
bool hold();
bool hold(uint8_t clicks);

// button released after holding [event]
bool releaseHold();
bool releaseHold(uint8_t clicks);

// pulse hold [event]
bool step();
bool step(uint8_t clicks);

// button released after pulse holding [event]
bool releaseStep();
bool releaseStep(uint8_t clicks);

// button released after holding or pulse holding [event]
bool releaseHoldStep();
bool releaseHoldStep(uint8_t clicks);

// button released (in any case) [event]
bool release();
bool release(uint8_t clicks);

// several clicks recorded [event]
bool hasClicks();
bool hasClicks(uint8_t clicks);

// timed out after interaction [event]
bool timeout(uint16_t ms);

// =============== STATES ===============

// button is pressed (between press() and release()) [state]
bool pressing();
bool pressing(uint8_t clicks);

// button is held (after hold()) [state]
bool holding();
bool holding(uint8_t clicks);

// button is held (after step()) [state]
bool stepping();
bool stepping(uint8_t clicks);

// button waits for repeated clicks (between click() and hasClicks()) [state]
bool waiting();

// processing in progress (between the first click and after waiting for clicks) [state]
bool busy();

// =============== TIME ===============

// time the button is held (from the beginning of pressing), ms
uint16_t pressFor();

// button is held longer than (from the beginning of pressing), ms [state]
bool pressFor(uint16_t ms);

// time the button is held (from the beginning of holding), ms
uint16_t holdFor();

// button is held longer than (from the beginning of holding), ms [state]
bool holdFor(uint16_t ms);

// time the button is held (from the beginning of the step), ms
uint16_t stepFor();

// button is held longer than (from the beginning of the step), ms [state]
bool stepFor(uint16_t ms);

// =============== VALUES ===============

// get the number of clicks
uint8_t getClicks();

// get the number of steps
uint8_t getSteps();

// =============== PROCESSING ===============

// call when button is pressed in interrupt
void pressISR();

// processing with anti-bounce.Returns true when changing state
bool poll(bool pressed);

// processing.Returns true when changing state
bool pollRaw(bool pressed);
```

#### uButton
The `uButton` class extends `uButtonVirt` by automatically initializing a pin and sending its data to `poll` inside `tick`.

```cpp
// the button is connected to GND (open drain)
uButton(uint8_t pin, uint8_t mode = INPUT_PULLUP);

// call in loop.Returns true when changing state
bool tick();

// poll without debounce, call in loop.Returns true when changing state
bool tickRaw();

// read the button state
bool readButton();
```

#### uButtonMulti
Treat clicks of two `uButtons` as a third button.

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

### Two buttons
```cpp
#include <uButtonMulti.h>

uButton b0(5);
uButton b1(6);
uButtonMulti b2;

void setup() {
Serial.begin(115200);
}

void loop() {
// polling individual buttons
b0.tick();
b1.tick();

// polling for simultaneous pressing of two buttons
b2.tick(b0, b1);

// event processing
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
- [Download library](https://github.com/GyverLibs/uButton/archive/refs/heads/main.zip).zip archive for manual installation:
- Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
- Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
- RaspaForge the cranberries and put them in *Documents/Arduino/libraries/*
- (Arduino IDE) automatic installation from .zip: *Sketch/Connect library/Add .ZIP library…* and indicate the downloaded archive
- Read more detailed instructions for installing libraries[here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: in new versions errors and bugs are corrected, as well as optimization is carried out and new features are added
- Through the IDE library manager: find the library as during installation and click "Update"
- Manually: **delete the folder with the old version**, and then put the new one in its place.“Replacement” cannot be done: sometimes new versions delete files that will remain after replacement and can lead to errors!

<a id="feedback"></a>

## Bugs and feedback
When you find bugs, create an **Issue**, or better yet, immediately write to [alex@alexgyver.ru](mailto:alex@alexgyver.ru)
The library is open for improvement and your **Pull Requests**!

When reporting bugs or incorrect operation of the library, be sure to indicate:
- Library version
- Which MK is used?
- SDK version (for ESP)
- Arduino IDE version
- Do the built-in examples that use functions and constructs that lead to a bug in your code work correctly?
- What code was loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but minimal code