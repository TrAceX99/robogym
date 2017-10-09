#include <toneAC.h>
#include <LiquidCrystal.h>

// Zvucnik na pinove 9 i 10!

#define PITCH_PHOTO 0
#define VOLUME_PHOTO 1
#define PITCH_RANGE 400
#define PITCH_OFFSET 300
#define POT 2

int volumeMax;
int volumeMin;
int pitchMax;
int pitchMin;

LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // RS - 2, E - 3, D4-D7 - 4-7

void setup() {
  lcd.begin(16, 2);
  lcd.print("Kalibracija...");
  
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(2000);
  
  pitchMax = analogRead(PITCH_PHOTO);
  volumeMax = analogRead(VOLUME_PHOTO);
  digitalWrite(13, LOW);
  lcd.clear();
  lcd.print("Priblizi ruke");
  lcd.setCursor(0, 1);
  
  while(analogRead(PITCH_PHOTO) > pitchMax - 100 && analogRead(VOLUME_PHOTO) > volumeMax - 100)
  if (analogRead(PITCH_PHOTO) > analogRead(VOLUME_PHOTO)) lcd.print(analogRead(PITCH_PHOTO));
  else lcd.print(analogRead(VOLUME_PHOTO));
  digitalWrite(13, HIGH);
  lcd.clear();
  lcd.home();
  lcd.print("Pls wait 4 sec.");
  delay(4000);
  
  pitchMin = analogRead(PITCH_PHOTO);
  volumeMin = analogRead(VOLUME_PHOTO);
  digitalWrite(13, LOW);
  lcd.clear();
  lcd.print("Kalibracija");
  lcd.setCursor(0, 1);
  lcd.print("Zavrsena!");
  delay(2000);
  lcd.clear();
  lcd.home();
  lcd.print("PITCH:");
  lcd.setCursor(0, 1);
  lcd.print("VOLUME:");
}

void loop() {
  float pitch = analogRead(PITCH_PHOTO);
  float volume = analogRead(VOLUME_PHOTO);
  
//  pitch -= pitchMin;
//  volume -= volumeMin;

  lcd.setCursor(7, 0);
  lcd.print(pitch);
  lcd.setCursor(8, 1);
  lcd.print(volume);
  
  float pitchAC = (pitch - pitchMin) / (pitchMax - pitchMin) * PITCH_RANGE + PITCH_OFFSET;
  float volumeAC = 10 - (volume - volumeMin) / (volumeMax - volumeMin) * 10;
  if (volumeAC > 0) toneAC(pitchAC, volumeAC);
  else toneAC();
  delay(analogRead(POT) / 10);
}
