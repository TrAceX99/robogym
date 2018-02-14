void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  double input = analogRead(5);
  input = input / 1023 * 5;
  Serial.print(input);
  Serial.println("V");
  delay(10);
}
