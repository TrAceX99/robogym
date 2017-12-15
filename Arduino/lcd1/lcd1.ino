#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 2, 3, 4, 5);

void setup() {
  lcd.begin(16, 2);
  lcd.print("test 1");
  lcd.setCursor(0, 1);
  lcd.print("test _ 2");
}

void loop() {
  // put your main code here, to run repeatedly:

}
