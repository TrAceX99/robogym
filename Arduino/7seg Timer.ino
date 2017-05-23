const int nums[11][7] = { {0, 0, 0, 0, 0, 0, 1},  // 0
                          {1, 0, 0, 1, 1, 1, 1},  // 1
                          {0, 0, 1, 0, 0, 1, 0},  // 2
                          {0, 0, 0, 0, 1, 1, 0},  // 3
                          {1, 0, 0, 1, 1, 0, 0},  // 4
                          {0, 1, 0, 0, 1, 0, 0},  // 5
                          {0, 1, 0, 0, 0, 0, 0},  // 6
                          {0, 0, 0, 1, 1, 1, 1},  // 7
                          {0, 0, 0, 0, 0, 0, 0},  // 8
                          {0, 0, 0, 0, 1, 0, 0},  // 9
                          {1, 1, 1, 1, 1, 1, 0} };// -
                    
const int digits[5] = {8, 7, 6, 5, 8};
const int segments[7] = {12, 1, 2, 3, 4, 9, 10};
const int dot = 11;
const int button = 0;

unsigned long timer = 0;
unsigned long setTime = 0;
byte timerState = 0;
unsigned int debounceTimer = 0;
bool buttonState;
bool debounce = false;

void setDigit(int n) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segments[i], nums[n][i]);
  }
}

void displayNumber(int n, int decimal) {
  int digit = 3;
  if (n > 9999 || n < 0) {
    digitalWrite(dot, HIGH);    
    while (digit >= 0) {
      digitalWrite(digits[digit + 1], LOW);
      setDigit(10);      
      digitalWrite(digits[digit], HIGH);
      digit--;      
    }    
  } else {
    while (digit >= 0) {
      digitalWrite(digits[digit + 1], LOW);
      if (n == 0 && digit < decimal) break;      
      setDigit(n % 10);
      if (digit == decimal) digitalWrite(dot, LOW);
      else digitalWrite(dot, HIGH);
      digitalWrite(digits[digit], HIGH);
      n /= 10;
      digit--;      
    }
  }
}

void setup() {
  for (int i = 1; i < 13; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);  
  }
  digitalWrite(dot, HIGH);
  pinMode(button, INPUT);
  buttonState = digitalRead(button);
}

void loop() {
  unsigned long currTime = millis();
  
  if (timerState == 0) {
    displayNumber(-1, 4);
  } else {
    if (timerState == 1) timer = currTime - setTime;
    displayNumber(timer / 100, 2);
  }

  if (digitalRead(button) != buttonState) {
    if (debounce) {
      if (currTime - debounceTimer > 20) {
        debounce = false;
        buttonState = !buttonState; 
        if (!buttonState) {
          timerState = ++timerState % 3;
          if (timerState == 1) setTime = currTime;
        }
      }
    } else {
      debounce = true;
      debounceTimer = currTime;
    }
  } else {
    debounce = false;
  }
  
}
