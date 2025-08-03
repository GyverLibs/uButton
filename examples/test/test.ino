#include <Arduino.h>
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
    }
}