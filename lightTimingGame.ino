#include <LedControl.h>

// MAX7219 LED Matrix Pins
#define DIN 11  
#define CS  10  
#define CLK 12  

LedControl lc = LedControl(DIN, CLK, CS, 1);

// LED Pins
const int LED1 = 2;
const int LED2 = 3;
const int LED3 = 4; // Green LED
const int LED4 = 5;
const int LED5 = 6;

// Button and Buzzer Pins
const int BUTTON = 7;
const int BUZZER = 9;

// Potentiometer Pin
const int POT_PIN = A0; 

// Control Variables
int buttonState = 0;
unsigned long previousMillis = 0;
unsigned long interval = 500;
int currentLED = -1;
int greenWins = 0; // score

// Heart Pattern (8x8)
byte heart[8] = {
  0b01100110,
  0b11111111,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000,
  0b00000000
};

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(POT_PIN, INPUT);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  Serial.begin(9600);

  
  Serial.println("Testing LED Matrix...");
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, true);
      delay(100);
    }
  }
  delay(1000);
  lc.clearDisplay(0);
}

void loop() {
  int potValue = analogRead(POT_PIN); 
  interval = map(potValue, 0, 1023, 30, 600); 

  buttonState = digitalRead(BUTTON);

    // win when press at green led
  if (buttonState == HIGH && currentLED == 2) {
    winSequence();
  }
  // loss when press at red led
  else if (buttonState == HIGH && currentLED != 2) {
    failSequence();
  }
  
  updateLEDSequence();

}

void winSequence() {
  greenWins++; // counting score
  Serial.print("Green Wins: ");
  Serial.println(greenWins);
  
  lc.clearDisplay(0);

  if (greenWins == 3) {
    displayHeart();  // display heart after 3 winning
    delay(3000);
  } else {
    displayScore(greenWins);
    delay(1000);
  }

  lc.clearDisplay(0);
  resetSequence();
}

void failSequence() {
  tone(BUZZER, 600);
  delay(500);
  noTone(BUZZER);

  resetSequence();
}

void resetSequence() {
  currentLED = -1;
  previousMillis = millis();
}

void updateLEDSequence() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);

    currentLED = (currentLED + 1) % 5;

    switch (currentLED) {
      case 0: digitalWrite(LED1, HIGH); break;
      case 1: digitalWrite(LED2, HIGH); break;
      case 2: digitalWrite(LED3, HIGH); break;
      case 3: digitalWrite(LED4, HIGH); break;
      case 4: digitalWrite(LED5, HIGH); break;
    }
  }
}

// Display Heart
void displayHeart() {
  lc.clearDisplay(0);
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      bool pixel = (heart[row] >> (7 - col)) & 1;
      lc.setLed(0, row, col, pixel);
    }
  }
}

// Display Score
void displayScore(int num) {
  lc.clearDisplay(0); 

  if (num > 9) num = 9; 

  byte numbers[10][8] = {
    {0b00111100, 0b01100110, 0b01101110, 0b01110110, 0b01100110, 0b01100110, 0b00111100, 0b00000000}, // 0
    {0b00011000, 0b00111000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b01111110, 0b00000000}, // 1
    {0b00111100, 0b01100110, 0b00000110, 0b00001100, 0b00011000, 0b00110000, 0b01111110, 0b00000000}, // 2
    {0b00111100, 0b01100110, 0b00000110, 0b00011100, 0b00000110, 0b01100110, 0b00111100, 0b00000000}, // 3
    {0b00001100, 0b00011100, 0b00101100, 0b01001100, 0b01111110, 0b00001100, 0b00001100, 0b00000000}, // 4
    {0b01111110, 0b01100000, 0b01111100, 0b00000110, 0b00000110, 0b01100110, 0b00111100, 0b00000000}, // 5
    {0b00111100, 0b01100110, 0b01100000, 0b01111100, 0b01100110, 0b01100110, 0b00111100, 0b00000000}, // 6
    {0b01111110, 0b01100110, 0b00001100, 0b00011000, 0b00110000, 0b00110000, 0b00110000, 0b00000000}, // 7
    {0b00111100, 0b01100110, 0b01100110, 0b00111100, 0b01100110, 0b01100110, 0b00111100, 0b00000000}, // 8
    {0b00111100, 0b01100110, 0b01100110, 0b00111110, 0b00000110, 0b01100110, 0b00111100, 0b00000000}  // 9
  };

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      bool pixel = (numbers[num][row] >> (7 - col)) & 1;
      lc.setLed(0, row, col, pixel);
    }
  }
}
