#define R_1 5
#define R_2 8
#define R_3 3
#define R_4 9
#define R_5 A0 
#define R_6 2
#define R_7 A2
#define R_8 10

#define C_1 A1
#define C_2 7
#define C_3 6
#define C_4 11
#define C_5 4
#define C_6 12
#define C_7 A3
#define C_8 A4

const byte rows[] = {R_1, R_2, R_3, R_4, R_5, R_6, R_7, R_8};

byte TODOS[] = {B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111};
byte EX[] = {B00000000,B00010000,B00010000,B00010000,B00010000,B00000000,B00010000,B00000000};
byte A[] = {B00000000,B00011000,B00100100,B00100100,B00111100,B00100100,B00100100,B00000000};
byte B[] = {B01111000,B01001000,B01001000,B01110000,B01001000,B01000100,B01000100,B01111100};
byte DRW[] = {B00000000,B00001000,B00111110,B00001000,B00001000,B00111110,B00100010,B00100010};

void setup() {
  for (byte i = 2; i <= 12; i++)
    pinMode(i, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  draw(A);
  delay(2);
}

void  draw(byte buffer2[]){
     
    for (byte i = 0; i < 8; i++) {
        setColumns(buffer2[i]); // Set columns for this specific row
        
        digitalWrite(rows[i], HIGH);
        delay(1); // Set this to 50 or 100 if you want to see the multiplexing effect!
        digitalWrite(rows[i], LOW);
        
    }
}


void setColumns(byte b) {
    digitalWrite(C_1, (~b >> 0) & 0x01); // Get the 1st bit: 10000000
    digitalWrite(C_2, (~b >> 1) & 0x01); // Get the 2nd bit: 01000000
    digitalWrite(C_3, (~b >> 2) & 0x01); // Get the 3rd bit: 00100000
    digitalWrite(C_4, (~b >> 3) & 0x01); // Get the 4th bit: 00010000
    digitalWrite(C_5, (~b >> 4) & 0x01); // Get the 5th bit: 00001000
    digitalWrite(C_6, (~b >> 5) & 0x01); // Get the 6th bit: 00000100
    digitalWrite(C_7, (~b >> 6) & 0x01); // Get the 7th bit: 00000010
    digitalWrite(C_8, (~b >> 7) & 0x01); // Get the 8th bit: 00000001
    
    // If the polarity of your matrix is the opposite of mine
    // remove all the '~' above.
}
