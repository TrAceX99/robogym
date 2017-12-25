void setup() {
  pinMode (4, OUTPUT);
  digitalWrite (4,HIGH); 
}

void loop() {
  delay(500);
  digitalWrite (4,LOW);
  delay(500);
  digitalWrite (4,HIGH);
}
