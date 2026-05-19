#include <Arduino.h>
#include "userInput.h"
#include "belt.h"
#include "pump.h"

extern int selectedDrink;   // from userInput.cpp
extern bool startPressed;   // from userInput.cpp

static bool machineRunning = false;

void setup() {
  userInputInit();
  beltInit();
  pumpInit();
}

void loop() {
  userInputUpdate();

  // Start the machine when green button pressed and drink selected
  if (startPressed && !machineRunning && beltIdle()) {
    machineRunning = true;
    beltStart(selectedDrink);
  }

  beltUpdate();
  pumpUpdate();

  // Reset when belt returns home
  if (machineRunning && beltIdle()) {
    machineRunning = false;
    startPressed   = false;
    selectedDrink  = -1;
  }
}