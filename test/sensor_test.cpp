#include <Arduino.h>

const int hallPin = 38;
const int ledPin  = 5;

void setup() {

  pinMode(hallPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);
}

void loop() {

  int state = digitalRead(hallPin);

  Serial.println(state);

  // LOW usually means magnet detected
  if(state == LOW) {
    digitalWrite(ledPin, HIGH);
  }
  else {
    digitalWrite(ledPin, LOW);
  }

  delay(50);
}