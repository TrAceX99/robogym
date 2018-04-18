void setup() {
  // put your setup code here, to run once:
pinMode(2, OUTPUT);
pinMode(3, OUTPUT);
pinMode(4, OUTPUT);
pinMode(9, OUTPUT);
pinMode(10, OUTPUT);
pinMode(11, OUTPUT);
pinMode(12, OUTPUT);
pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(2) > 0){
    digitalWrite(9, HIGH);
    digitalWrite(13, HIGH);
    digitalWrite(12, HIGH);
  }
  if(digitalRead(3) > 0){
    digitalWrite(9, HIGH);
    digitalWrite(11, HIGH);
    digitalWrite(10, HIGH);
  }
  if(digitalRead(4) > 0){
    digitalWrite(
  }
}
