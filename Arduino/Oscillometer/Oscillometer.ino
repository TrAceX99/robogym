#define INPUT_PIN 3
#define RESOLUTION 500

// Input pin je od 0 do 7!!!
// PIND -> 0-7
// PINB -> 8-13

unsigned long timer = 0;
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
}
