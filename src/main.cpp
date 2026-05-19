#include <Arduino.h>
#include "userInput.h"
#include "liquid1_stepper.h"

void setup() {
  Serial.begin(115200);
  userInputInit();
  liquid1_stepperInit();
}

void loop() {
  userInputUpdate();
  liquid1_stepperUpdate();
}