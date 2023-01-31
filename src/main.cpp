#include <Arduino.h>

__attribute__((unused)) void setup() {

}

__attribute__((unused)) void loop() {
    Serial.begin(115200);
    Serial.println("hellsdasdo!");
    sleep(1);
}