#include <Arduino.h>
#include <uFR.h>
#include <LCDDebug.h>

uFR reader(10, 11, 12);

#define RELAY_PIN 2

void setup() {
	pinMode(RELAY_PIN, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(RELAY_PIN, LOW);
	Serial.begin(9600);
	delay(100);
	reader.begin();
	delay(2000);
	reader.setRedLED(HIGH);
	uint8_t type[4];
	uint8_t serial[4];
	uint8_t data[16];
	uint8_t dataIn[16] = "**test464646**";
	Serial.print(reader.setUserData(dataIn));
	Serial.print("\nType:");
	Serial.print(reader.getReaderType(type), HEX);
	Serial.print(": ");
	for (int i = 0; i < 4; i++) {
		Serial.print(type[i], HEX);
		Serial.print(" ");
	}
	delay(1000);
	Serial.print("\nSerial:");
	Serial.print(reader.getReaderSerial(serial), HEX);
	Serial.print(": ");
	for (int i = 0; i < 4; i++) {
		Serial.print(serial[i], HEX);
		Serial.print(" ");
	}
	delay(1000);
	Serial.print("\nUser data:");
	Serial.print(reader.getUserData(data), HEX);
	Serial.print(": ");
	for (int i = 0; i < 16; i++) {
		Serial.print(static_cast<char>(data[i]));
	}
	Serial.print(", ili: 0x");
	for (int i = 0; i < 16; i++) {
		Serial.print(data[i], HEX);
	}
	delay(1000);
	Serial.println();
	delay(1000);
	reader.setRedLED(LOW);
}

void loop() {
	uint8_t cardID[4];
	uint8_t code = reader.getCardID(cardID);
	delay(10);
	if (code == 0) {
		Serial.print("\nID: 0x");
		for (int i = 0; i < 4; i++) {
				Serial.print(cardID[i], HEX);
				Serial.print(" ");
			}
		Serial.print("\nType: 0x");
		Serial.println();
		digitalWrite(RELAY_PIN, HIGH);
		delay(3000);
		digitalWrite(RELAY_PIN, LOW);
	} else {
		Serial.print(code, HEX);
	}
	delay(500);
}