void setup() {
  // put your setup code here, to run once:
pinMode(7, OUTPUT);
pinMode(5, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(7, HIGH);
delay (500);
digitalWrite(7, LOW);
delay (500);
digitalWrite(5, HIGH);
delay (500);
digitalWrite(5, LOW);
delay (500);
}
