#include <Arduino.h>
#include <LCDDebug.h>
#include <SoftwareSerial.h>

LCDDebug lcd(2, 3, 4, 5, 6, 7);
SoftwareSerial mySerial(10, 11);

const byte red[7] = {0x55, 0x71, 0xAA, 0x00, 0x01, 0x00, 0x96};
const byte redOff[7] = {0x55, 0x71, 0xAA, 0x00, 0x00, 0x00, 0x95};
const byte get[7] = {0x55, 0x10, 0xAA, 0x00, 0x00, 0x00, 0xF6};

void setup() {
    mySerial.begin(115200);
	Serial.begin(115200);
    delay(1000);
    mySerial.write(red, sizeof(red));
	delay(2000);
	mySerial.write(redOff, 7);
}

void loop() {
    if (mySerial.available()){
        Serial.write(mySerial.read());
    }
    if (Serial.available()){
        mySerial.write(Serial.read());
    }
}