void setup() {
 
  pinMode(9, OUTPUT);
  int A = 1000;
  
}

void loop() 

{
  
  if (A > 1200)
  {
  digitalWrite(9, HIGH);
  delay(300);
  digitalWrite(9, LOW);
  delay(150);
  }
  else 
  {
    digitalWrite(9, LOW);
    }
  
}
