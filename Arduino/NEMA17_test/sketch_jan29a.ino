const int stepPin = 3; 
const int dirPin = 4; 
 
void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  digitalWrite(dirPin,LOW);
}
void loop() {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(300); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(300); 
}
