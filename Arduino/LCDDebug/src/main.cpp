#include <Arduino.h>
#include <LCDDebug.h>

LCDDebug lcd(2, 3, 4, 5, 6, 7);

void setup() {
    lcd.println("B_LED:");
    lcd.println("tes2");
    delay(1000);
}

void loop() {
    lcd.println(String(millis()));
    delay(1258);
}