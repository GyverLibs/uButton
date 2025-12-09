#include <Arduino.h>
#include <uButtonMulti.h>

uButton b0(5);
uButton b1(6);
uButtonMulti b2;

void setup() {
    Serial.begin(115200);
}

void loop() {
    // опрос отдельных кнопок
    b0.tick();
    b1.tick();

    // опрос одновременного нажатия двух кнопок
    b2.tick(b0, b1);

    // обработка событий
    if (b0.click()) Serial.println("b0 click");
    if (b0.step()) Serial.println("b0 step");

    if (b1.click()) Serial.println("b1 click");
    if (b1.step()) Serial.println("b1 step");

    if (b2.click()) Serial.println("b0+b1 click");
    if (b2.step()) Serial.println("b0+b1 step");
}