#define INPUT_PIN 2
#define RESOLUTION 500

// Input pin je od 0 do 7!!!
// PIND -> 0-7
// PINB -> 8-13

/*unsigned long timer = 0;
unsigned long count = 0;
bool state;

void setup() {
  Serial.begin(9600);
  delay(10);
  state = digitalRead(INPUT_PIN);
}

void loop() {
  while((unsigned long)(millis() - timer) >= RESOLUTION) {
    if(state)
      if(!(PIND & (1 << INPUT_PIN))) {
        count++;
        state = false;
      } 
    else if(PIND & (1 << INPUT_PIN)) state = true;
  }
  Serial.print(count * RESOLUTION / 1000);
  Serial.println(" Hz");
  timer += RESOLUTION;
  count = 0;
}*/

// Obavezno pin 2 za input !!!

volatile unsigned long count = 0;
unsigned long timer = 0;

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  attachInterrupt(0, routine, FALLING); // INT0 -> pin 2
  delay(10);
}

void routine() {
  count++;
}

void loop() {
  if((unsigned long)(millis() - timer) >= RESOLUTION) {
    Serial.print(count * RESOLUTION / 1000);
    Serial.println(" Hz");
    timer += RESOLUTION;
    count = 0;
  }
}
