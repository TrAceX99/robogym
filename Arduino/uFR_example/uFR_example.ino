#include <LiquidCrystal.h>
#include <uFR.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // rs, en, d4, d5, d6, d7

// If this doesn't work, try switching rx and tx
uFR reader(10, 11, 12); // rx, tx, reset

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.begin(16, 2);
  lcd.clear();
  delay(100);
  reader.begin();
  delay(2000); // Required!
  
  /*  For testing purposes:
   *  reader.setRedLED(HIGH);
   *  delay(2000);
   *  reader.setRedLED(HIGH);
   */
  uint8_t type[4];
  lcd.print("Code: ");
  lcd.print(reader.getReaderType(type), HEX);
  lcd.setCursor(0, 1);
  lcd.print("; Type:");
  for (int i = 0; i < 4; i++) {
    lcd.print(type[i], HEX);
    lcd.print(" ");
  }
  delay(2000);
  lcd.clear();
  lcd.print("Waiting for card");
}

void loop() {
  uint8_t cardID[10];
  uint8_t length = 0;
  uint8_t code = reader.getCardID(cardID, &length);
  if (code == 0) {
    lcd.print("\nUID:");
    lcd.setCursor(0, 1);
    for (int i = 0; i < length; i++) {
        lcd.print(cardID[i], HEX);
        lcd.print(" ");
      }
    digitalWrite(LED_BUILTIN, HIGH);
    delay(3000);
    digitalWrite(LED_BUILTIN, LOW);
    lcd.clear();
    lcd.print("Waiting for card");
  } else if (code != NO_CARD) {
    lcd.print("Error code:");
    lcd.setCursor(0, 1);
    lcd.print("0x");
    lcd.print(code, HEX);
    delay(1000);
    lcd.clear();
    lcd.print("Waiting for card");
  }
  delay(200);
}
