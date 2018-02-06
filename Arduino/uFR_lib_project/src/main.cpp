#include <Arduino.h>
#include <uFR.h>
#include <LCDDebug.h>

uFR reader(10, 11, 12);
LCDDebug lcd(3, 4, 5, 6, 7, 8);

#define RELAY_PIN 2

void setup() {
	pinMode(RELAY_PIN, OUTPUT);
	digitalWrite(RELAY_PIN, LOW);
	reader.begin();
	delay(500);
	reader.setRedLED(HIGH);
	uint8_t type[4];
	lcd.raw.print(reader.getReaderType(type), HEX);
	lcd.raw.print(", Reader type:");
	lcd.raw.setCursor(0, 1);
	lcd.raw.print("0x");
	for (int i = 0; i < 4; i++) {
		lcd.raw.print(type[i], HEX);
	}
	delay(5000);
	reader.setRedLED(LOW);
}

void loop() {
	uint8_t cardID[4];
	uint8_t cardType;
	uint8_t code = reader.getCardID(cardID, &cardType);
	if (code == 0) {
		lcd.raw.clear();
		lcd.raw.print("ID: 0x");
		for (int i = 0; i < 4; i++)
			lcd.raw.print(cardID[i], HEX);
		lcd.raw.setCursor(0, 1);
		lcd.raw.print("Type: 0x");
		lcd.raw.print(cardType, HEX);
		digitalWrite(RELAY_PIN, HIGH);
		delay(3000);
		digitalWrite(RELAY_PIN, LOW);
	} else {
		lcd.println(String(code, HEX));
	}
	delay(200);
}