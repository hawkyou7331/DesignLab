#define POWER_LED A0
#define UART_LED A1
#define I2C_LED A2

int buttonPins[4] = {2, 3, 4, 5};
int buttonState[4];

void setup() {
  pinMode(POWER_LED, OUTPUT);
  pinMode(UART_LED, OUTPUT);
  pinMode(I2C_LED, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  digitalWrite(POWER_LED, HIGH);
  Serial.begin(2400);

}

void loop() {
  for (int i = 0; i < 4; i++) {
    int state = digitalRead(buttonPins[i]);
    
    if (state == LOW && buttonState[i] != LOW) {
      Serial.println(i + 1);
    }

    buttonState[i] = state;
  }

  delay(50);

}
