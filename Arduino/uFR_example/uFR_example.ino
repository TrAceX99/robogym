#include <LiquidCrystal.h>
#include <uFR.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // RS, E, d4, d5, d6, d7

// If this doesn't work, try switching rx and tx
uFR reader(10, 11, 12); // rx, tx, reset

//#define DEBUG_CODE

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
  lcd.print("  Type:");
  lcd.setCursor(0, 1);
  for (int i = 0; i < 4; i++) {
    lcd.print(type[i], HEX);
    lcd.print(" ");
  }
  delay(3000);
  lcd.clear();
  lcd.print("Waiting for card");
}

void loop() {
  uint8_t cardID[10];
  uint8_t length = 0;
  uint8_t code = reader.getCardID(cardID, &length);
  if (code == 0) {
    lcd.clear();
    lcd.print("Card found! UID:");
    lcd.setCursor(0, 1);
    for (int i = 0; i < length; i++) {
		//show leading zero for 0x00 to 0x0F 
		if(cardID[i] < 16)
        lcd.print("0"); 
      lcd.print(cardID[i], HEX);
    }
    digitalWrite(LED_BUILTIN, HIGH);
    delay(3000);
    digitalWrite(LED_BUILTIN, LOW);
    lcd.clear();
    lcd.print("Waiting for card");
  } else if (code != NO_CARD) {
#ifdef 	DEBUG_CODE
	lcd.print("Error code:");
    lcd.setCursor(0, 1);
    lcd.print("0x");
    lcd.print(code, HEX);
    delay(2000);
    lcd.clear();
    lcd.print("Waiting for card");
#endif //DEBUG_CODE	
  }
  // Reduce delay if response is slow
  delay(200);
}
