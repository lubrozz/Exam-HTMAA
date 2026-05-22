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
  BELT_WAIT_AT_ICE,
  BELT_AT_ICE,
  BELT_MOVING_TO_PUMP,
  BELT_WAITING_AT_PUMP,
  BELT_AT_PUMP
};

static BeltState beltState        = BELT_IDLE; // first state of the belt
static int selectedDrink          = -1; // no drink selected yet, start at -1
static int currentPumpTarget      = 0;  // which pump stop we're heading to next (1-3)
static unsigned long iceWaitStart = 0;
static unsigned long iceStartTime = 0;
bool iceRunning = false;

static unsigned long pumpWaitStart = 0;

void beltInit() {
  beltStepper.setMaxSpeed(BELT_MAXSPEED);
  beltStepper.setAcceleration(BELT_ACCELERATION);

  pinMode(ICE_SENSOR_PIN, INPUT);
  pinMode(ICE_MOTOR_PIN, OUTPUT);
  pinMode(ICE_SENSOR_LED, OUTPUT);
  digitalWrite(ICE_MOTOR_PIN, LOW);
  digitalWrite(ICE_SENSOR_LED, LOW);

  for (int i = 0; i < 4; i++) {
    pinMode(sensorPins[i], INPUT);
  }
}

void beltStart(int drinkIndex) {
  selectedDrink    = drinkIndex;
  currentPumpTarget = 0;
  beltState        = BELT_MOVING_TO_ICE;
  beltStepper.move(100000); // move until sensor stops it
}


bool beltIdle() {
  return beltState == BELT_IDLE;
}

void beltUpdate() {
  switch (beltState) {
    case BELT_IDLE:
      break;

    case BELT_MOVING_TO_ICE:
      Serial.println("Moving to ice");
      beltStepper.run();
      if (digitalRead(sensorPins[0]) == LOW) {
        beltStepper.stop();
        Serial.println("Belt stopped");
        beltState    = BELT_WAIT_AT_ICE;
        iceWaitStart = millis();
      }
      break;
      
    case BELT_WAIT_AT_ICE:
      Serial.println("Waiting at ice");
      if (millis() - iceWaitStart >= ICE_WAIT_MS)
      {
        digitalWrite(ICE_MOTOR_PIN, HIGH);
        digitalWrite(ICE_SENSOR_LED, HIGH);
        iceRunning   = true;
        iceStartTime = millis();
        beltState = BELT_AT_ICE;
        break;
      }
      
    case BELT_AT_ICE:
      // Wait for ice motor to finish dispensing
      if (iceRunning && millis() - iceStartTime >= ICE_DISPENSE_MS) {
        digitalWrite(ICE_SENSOR_LED, LOW);
        digitalWrite(ICE_MOTOR_PIN, LOW);
        iceRunning      = false;
        currentPumpTarget = 1; // move to first pump
        beltStepper.move(100000);
      }
      beltState = BELT_MOVING_TO_PUMP; 
      break;

    case BELT_MOVING_TO_PUMP:
      Serial.println("Moving to a pump");
      beltStepper.run();
      if (digitalRead(sensorPins[currentPumpTarget]) == LOW)
      {
        if (drinkRecipes[selectedDrink][currentPumpTarget - 1] == 1) {
          beltStepper.stop();
          beltState       = BELT_WAITING_AT_PUMP;
        } else {
          // Not needed for this drink, keep moving
          currentPumpTarget++;
      }
      if (currentPumpTarget > 3) {
        // NEED: check if the drink is done or move belt back to past pump
        // Past all pumps, go home
        beltStepper.move(BELT_HOME_STEPS);
      }
      pumpWaitStart = millis();
  }
  break;

  case BELT_WAITING_AT_PUMP:
    Serial.println("Belt waiting at pump");
    if (millis() - pumpWaitStart >= PUMP_WAIT_MS )
    {
      
    }
    

}