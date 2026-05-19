#include <Arduino.h>
#include <AccelStepper.h>
#include "belt.h"
#include "../include/config.h"

AccelStepper beltStepper(AccelStepper::DRIVER, BELT_STEP, BELT_DIR);

// Sensor pins in order
static const int sensorPins[4] = {
  BELT_SENSOR_0,
  BELT_SENSOR_1,
  BELT_SENSOR_2,
  BELT_SENSOR_3
};

// Drink recipes: which pump stops are needed (pump 1, 2, 3)
static const int drinkRecipes[3][3] = {
  DRINK_BLUE,
  DRINK_RED,
  DRINK_YELLOW
};

// Belt states - enumeration with names instead of int
enum BeltState {
  BELT_IDLE,
  BELT_MOVING_TO_ICE,
  BELT_AT_ICE,
  BELT_MOVING_TO_PUMP,
  BELT_AT_PUMP,
  BELT_RETURNING_HOME
};

static BeltState beltState        = BELT_IDLE; // first state of the belt
static int selectedDrink          = -1; // no drink selected yet, start at -1
static int currentPumpTarget      = 0;  // which pump stop we're heading to next (1-3)
static bool sensorTriggered       = false; // start with sensor not triggered
static bool lastSensorState[4]    = {LOW, LOW, LOW, LOW}; // start all sensors on low - will change later

// Ice motor timing
static unsigned long iceStartTime = 0; // used for timing ice dispension
static bool iceRunning            = false; // is ice dispensing or not

void beltInit() {
  beltStepper.setMaxSpeed(BELT_MAXSPEED);
  beltStepper.setAcceleration(BELT_ACCELERATION);

  pinMode(ICE_SENSOR_PIN, INPUT);
  pinMode(ICE_MOTOR_PIN, OUTPUT);
  digitalWrite(ICE_MOTOR_PIN, LOW);

  for (int i = 0; i < 4; i++) {
    pinMode(sensorPins[i], INPUT);
  }
}

void beltStart(int drinkIndex) {
  selectedDrink    = drinkIndex;
  currentPumpTarget = 0;
  sensorTriggered  = false;
  beltState        = BELT_MOVING_TO_ICE;
  beltStepper.move(100000); // move until sensor stops it
}

bool beltAtDestination() {
  return sensorTriggered;
}

bool beltIdle() {
  return beltState == BELT_IDLE;
}

// Check if a sensor just fired (rising edge)
static bool sensorJustTriggered(int sensorIndex) {
  bool current = digitalRead(sensorPins[sensorIndex]) == HIGH;
  bool triggered = current && !lastSensorState[sensorIndex];
  lastSensorState[sensorIndex] = current;
  return triggered;
}

void beltUpdate() {
  // Update all sensor states
  for (int i = 0; i < 4; i++) {
    sensorJustTriggered(i); // keeps lastSensorState up to date
  }

  switch (beltState) {

    case BELT_IDLE:
      break;

    case BELT_MOVING_TO_ICE:
      beltStepper.run();
      if (sensorJustTriggered(0)) {
        beltStepper.stop();
        beltState    = BELT_AT_ICE;
        sensorTriggered = true;
        // Start ice motor
        digitalWrite(ICE_MOTOR_PIN, HIGH);
        iceStartTime = millis();
        iceRunning   = true;
      }
      break;

    case BELT_AT_ICE:
      // Wait for ice motor to finish dispensing
      if (iceRunning && millis() - iceStartTime >= ICE_DISPENSE_MS) {
        digitalWrite(ICE_MOTOR_PIN, LOW);
        iceRunning      = false;
        sensorTriggered = false;
        currentPumpTarget = 1; // move to first pump
        beltStepper.move(100000);
        beltState = BELT_MOVING_TO_PUMP;
      }
      break;

    case BELT_MOVING_TO_PUMP:
      beltStepper.run();
      // Check sensor for current pump target (sensor index = pump number)
      if (sensorJustTriggered(currentPumpTarget)) {
        // Should we stop here for this drink?
        if (drinkRecipes[selectedDrink][currentPumpTarget - 1] == 1) {
          beltStepper.stop();
          beltState       = BELT_AT_PUMP;
          sensorTriggered = true; // signals pump.cpp to dispense
        } else {
          // Not needed for this drink, keep moving
          currentPumpTarget++;
          if (currentPumpTarget > 3) {
            // NEED: check if the drink is done or move belt back to past pump
            // Past all pumps, go home
            beltStepper.move(BELT_HOME_STEPS);
            beltState = BELT_RETURNING_HOME;
          }
        }
      }
      break;

    case BELT_AT_PUMP:
      // Waiting — pump.cpp will call beltContinue() when done dispensing
      break;

    case BELT_RETURNING_HOME:
      beltStepper.run();
      if (beltStepper.distanceToGo() == 0) {
        beltStepper.setCurrentPosition(0);
        beltState = BELT_IDLE;
      }
      break;
  }
}

// Called by pump.cpp when dispensing is done
void beltContinue() {
  sensorTriggered = false;
  currentPumpTarget++;

  // Find next required pump
  while (currentPumpTarget <= 3 &&
         drinkRecipes[selectedDrink][currentPumpTarget - 1] == 0) {
    currentPumpTarget++;
  }

  if (currentPumpTarget > 3) {
    // All pumps done, go home
    beltStepper.move(BELT_HOME_STEPS);
    beltState = BELT_RETURNING_HOME;
  } else {
    beltStepper.move(100000);
    beltState = BELT_MOVING_TO_PUMP;
  }
}