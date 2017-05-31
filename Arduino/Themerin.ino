#include <toneAC.h>

// Zvucnik na pinove 9 i 10!

#define PITCH_PHOTO 0
#define VOLUME_PHOTO 1
#define PITCH_RANGE 600
#define PITCH_OFFSET 200

int volumeMax;
int volumeMin;
int pitchMax;
int pitchMin;

void setup() {
  // Kalibracija:
  digitalWrite(13, HIGH);
  delay(2000);
  pitchMax = analogRead(PITCH_PHOTO);
  volumeMax = analogRead(VOLUME_PHOTO);
  digitalWrite(13, LOW);
  while(analogRead(PITCH_PHOTO) > pitchMax - 100 && analogRead(VOLUME_PHOTO) > volumeMax - 100);
  digitalWrite(13, HIGH);
  delay(4000);
  pitchMin = analogRead(PITCH_PHOTO);
  volumeMin = analogRead(VOLUME_PHOTO);
  digitalWrite(13, LOW);
}

void loop() {
  int pitch = analogRead(PITCH_PHOTO) - pitchMin;
  int volume = analogRead(VOLUME_PHOTO) - volumeMin;

  toneAC(pitch / pitchMax * PITCH_RANGE + PITCH_OFFSET, (1 - volume * volumeMax) * 10);
}
