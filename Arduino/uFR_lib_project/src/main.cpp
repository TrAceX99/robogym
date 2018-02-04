#include <SoftwareSerial.h>
#include <Arduino.h>
#include <uFR.h>

uFR reader(10, 11);

void setup() {
	Serial.begin(9600);
    reader.begin();
}

void loop() {
    Serial.println(reader.setRedLED(HIGH), HEX);
    delay(2000);
    Serial.println(reader.setRedLED(LOW), HEX);
    delay(2000);
}