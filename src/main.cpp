#include <Arduino.h>
#include <AccelStepper.h>
#include "userInput.h"
#include "belt.h"
#include "pump.h"
#include "motors_test.h"

extern int selectedDrink;   // from userInput.cpp
extern bool startPressed;   // from userInput.cpp

static bool machineRunning = false;

void machineReset() {

  beltReset();
  pumpReset();
  userInputReset();

  machineRunning = false;
  startPressed   = false;
  selectedDrink  = -1;
}


void setup() {
  /*motorsInit();*/
  userInputInit();
  beltInit();
  pumpInit();
  Serial.begin(9600);

}

void loop() {
  /*motorsUpdate();*/
  
  userInputUpdate();

  // Start the machine when green button pressed and drink selected
  if (startPressed && !machineRunning && beltIdle()) {
    Serial.println("A drink has been selected");
    machineRunning = true;
    beltStart(selectedDrink);
  }

  beltUpdate();
  pumpUpdate();

  // Reset when belt returns home
  if (machineRunning && beltIdle()) {
    machineReset();
  }
}