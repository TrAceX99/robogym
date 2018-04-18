

void setup() {
  pinMode(9, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  tone(9, analogRead(0) * 2);
  delay(1);
}
