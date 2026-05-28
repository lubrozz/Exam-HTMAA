#include <Arduino.h>
#include "userInput.h"
#include "../include/config.h"

const int buttons[] = BUTTON_PINS;
const int leds[]    = BUTTON_LEDS;

int selectedDrink = -1;
bool startPressed = false;

static unsigned long previousMillis = 0;
static bool blinkState = false;

static bool lastButtonState[4] = {LOW, LOW, LOW, LOW};

void userInputInit() {
  for (int i = 0; i < 4; i++) {
    pinMode(buttons[i], INPUT);
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW); // all LEDs off by default
  }
}

void userInputUpdate() {
  // --- Drink buttons (blue=0, red=1, yellow=2) ---
  for (int i = 0; i < 3; i++) {
    bool currentState = digitalRead(buttons[i]);

    // detect falling edge (LOW -> HIGH = button just pressed)
    if (currentState == HIGH && lastButtonState[i] == LOW) {
      delay(20); // debounce
      if (digitalRead(buttons[i]) == HIGH) {
        selectedDrink = i;
        startPressed = false;
        blinkState = false;
        previousMillis = 0;

        // turn off all LEDs except selected drink
        for (int j = 0; j < 4; j++) {
          digitalWrite(leds[j], LOW);
        }
        digitalWrite(leds[i], HIGH);
      }
    }
    lastButtonState[i] = currentState;
  }

  // --- Blink start LED if a drink is selected and start not yet pressed ---
  if (selectedDrink != -1 && !startPressed) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= BLINK_INTERVAL) {
      previousMillis = currentMillis;
      blinkState = !blinkState;
      digitalWrite(leds[3], blinkState ? HIGH : LOW);
    }
  }

  // --- Start button (green=3) ---
  bool startState = digitalRead(buttons[3]);
  if (startState == HIGH && lastButtonState[3] == LOW) {
    delay(20); // debounce
    if (digitalRead(buttons[3]) == HIGH && selectedDrink != -1 && !startPressed) {
      startPressed = true;

      // turn off drink LEDs, start LED solid on
      for (int j = 0; j < 3; j++) {
        digitalWrite(leds[j], LOW);
      }
      digitalWrite(leds[3], HIGH);
    }
  }
  lastButtonState[3] = startState;
}

void userInputReset() {

  selectedDrink = -1;
  startPressed = false;

  previousMillis = 0;
  blinkState = false;

  // Reset button history
  for (int i = 0; i < 4; i++) {
    lastButtonState[i] = LOW;
  }

  // Turn off all LEDs
  for (int i = 0; i < 4; i++) {
    digitalWrite(leds[i], LOW);
  }
}