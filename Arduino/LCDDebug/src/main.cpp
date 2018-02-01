#include <Arduino.h>
#include <LCDDebug.h>
#include <SoftwareSerial.h>

LCDDebug lcd(2, 3, 4, 5, 6, 7);
SoftwareSerial mySerial(10, 11);

void setup() {
    mySerial.begin(9600);
    Serial.begin(115);
    delay (500);
    Serial.write(0x55);
    Serial.write(0x71);
    Serial.write(0xAA);
    Serial.write(0x00);
    Serial.write(0x01);
    Serial.write(0x00);
    Serial.write(0x96);
}

void loop() {
    if (Serial.available()){
        lcd.raw.print(Serial.read());
    }
}