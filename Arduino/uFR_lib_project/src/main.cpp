#include <Arduino.h>
#include <uFR.h>
#include <LCDDebug.h>

uFR reader(10, 11);
LCDDebug lcd(3, 4, 5, 6, 7, 8);

void setup() {
	//Serial.begin(9600);
    reader.begin();
    delay(1000);
    uint8_t type[4];
    lcd.raw.print("Reader type:");
    lcd.raw.setCursor(0, 1);
    lcd.raw.print("0x");
    for (int i = 4; i >= 0; i--)
        lcd.raw.print(type[i], HEX);
    delay(5000);
}

void loop() {
    uint8_t cardID[4];
    uint8_t cardType;
    uint8_t code = reader.getCardID(cardID, &cardType);
    if (code == 0) {
        lcd.raw.clear();
        lcd.raw.print("ID: 0x");
        for (int i = 4; i >= 0; i--)
            lcd.raw.print(cardID[i], HEX);
        lcd.raw.setCursor(0, 1);
        lcd.raw.print("Type: 0x");
        lcd.raw.print(cardType, HEX);
        delay(1000);
    } else {
        lcd.println(".");
    }
    delay (1000);
}