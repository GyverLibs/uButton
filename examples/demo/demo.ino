#include <Arduino.h>

// настройки таймаутов, мс
// #define UB_DEB_TIME 50     // дебаунс
// #define UB_HOLD_TIME 600   // время до перехода в состояние "удержание"
// #define UB_STEP_TIME 400   // время до перехода в состояние "импульсное удержание"
// #define UB_STEP_PRD 200    // период импульсов
// #define UB_CLICK_TIME 500  // ожидание кликов

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
    }
}