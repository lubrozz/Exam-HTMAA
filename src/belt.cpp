#include <Arduino.h>
#include <AccelStepper.h>
#include "belt.h"
#include "pump.h"
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
static const int drinkRecipes[3][2] = {
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
  BELT_WAITING_FOR_PUMP,
  BELT_RETURNING_HOME
};

static BeltState beltState        = BELT_IDLE; // first state of the belt
static int selectedDrink          = -1; // no drink selected yet, start at -1
static int currentPumpTarget      = 0;  // which pump stop we're heading to next (1-2)
static unsigned long iceWaitStart = 0;
static unsigned long iceStartTime = 0;
static bool iceRunning = false;
static bool waitingForPumpStart = false;

static long stepsToHome = 0;


int beltCurrentPump() {
  return currentPumpTarget;
}

int beltGetRecipeCl(int drink, int pump) {
  return drinkRecipes[drink][pump];
}

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
  beltStepper.setCurrentPosition(0);
  Serial.print("Home position: ");
  Serial.println(beltStepper.currentPosition());
  beltStepper.moveTo(100000); // move until sensor stops it
}


bool beltIdle() {
  return beltState == BELT_IDLE;
}

void beltUpdate() {
  // IMPORTANT: Serial.print() outside if-statements will stall the belt motor!
  switch (beltState) {
    case BELT_IDLE:
      break;

    case BELT_MOVING_TO_ICE:
      beltStepper.run();
      if (digitalRead(sensorPins[0]) == LOW) {
        beltStepper.stop();
        Serial.print("Ice position: ");
        Serial.println(beltStepper.currentPosition());
        beltState    = BELT_WAIT_AT_ICE;
        iceWaitStart = millis();
      }
      break;
      
    case BELT_WAIT_AT_ICE:
      beltStepper.run();
      if (!beltStepper.isRunning())
      // if (millis() - iceWaitStart >= ICE_WAIT_MS)
      {
        digitalWrite(ICE_MOTOR_PIN, HIGH);
        Serial.println("Ice dispensing");
        digitalWrite(ICE_SENSOR_LED, HIGH);
        iceRunning   = true;
        iceStartTime = millis();
        beltState = BELT_AT_ICE;
      }
      break;
      
    case BELT_AT_ICE:
      // Wait for ice motor to finish dispensing
      if (iceRunning && millis() - iceStartTime >= ICE_DISPENSE_MS) {
        digitalWrite(ICE_SENSOR_LED, LOW);
        Serial.println("ice finished");
        digitalWrite(ICE_MOTOR_PIN, LOW);
        iceRunning      = false;
        currentPumpTarget = 1; // move to first pump
        beltStepper.moveTo(100000);
        beltState = BELT_MOVING_TO_PUMP; 
      }
      break;

    case BELT_MOVING_TO_PUMP:
      beltStepper.run();
      if (digitalRead(sensorPins[currentPumpTarget]) == LOW) {
        Serial.print("Sensor ");
        Serial.print(currentPumpTarget);
        Serial.print(" at position: ");
        Serial.println(beltStepper.currentPosition());
        if (drinkRecipes[selectedDrink][currentPumpTarget - 1] > 0) {
          beltStepper.stop();
          int pump = beltCurrentPump();
          int cl = beltGetRecipeCl(selectedDrink, pump - 1);
          pumpDispenseCl(pump - 1, cl);
          Serial.print("Currently pumping for: ");
          Serial.println(currentPumpTarget);
          waitingForPumpStart = true;
          beltState = BELT_WAITING_FOR_PUMP;
        } else {
          // Not needed for this drink, keep moving
          currentPumpTarget++;
          if (currentPumpTarget > 2) {
            // NEED: check if the drink is done or move belt back to past pump
            // Past all pumps, go home
            
            stepsToHome = beltStepper.currentPosition();
            beltStepper.stop();
            Serial.print("Going home with steps (BELT_MOVING_TO_PUMP): -");
            Serial.println(stepsToHome);
            beltState = BELT_RETURNING_HOME;
          }
        }
      }
      break;

    case BELT_WAITING_FOR_PUMP:
      beltStepper.run();

      // Wait until the belt has fully stopped
      if (beltStepper.isRunning()) {
        break;
      }
      if (waitingForPumpStart) // small delay
      {
        waitingForPumpStart = false;
        break; 
      }
    
      if (pumpIdle(currentPumpTarget - 1))
      {
        Serial.print("Pump done, currentPumpTarget was: ");
        Serial.println(currentPumpTarget);
        currentPumpTarget++;
        Serial.print("Now: ");
        Serial.println(currentPumpTarget);
        if (currentPumpTarget > 2) {
          
          stepsToHome = beltStepper.currentPosition();
          beltStepper.stop();
          Serial.print("Going home with steps (BELT_WAITING_FOR_PUMP): -");
          Serial.println(stepsToHome);
          beltStepper.moveTo(0);
          beltState = BELT_RETURNING_HOME;
        } else {
          beltStepper.moveTo(100000);
          beltState = BELT_MOVING_TO_PUMP;
        }      
      }
      break;

    case BELT_RETURNING_HOME:
      beltStepper.run();
      if (beltStepper.isRunning()) {
        break;
      }

      beltStepper.run();

      if (beltStepper.distanceToGo() == 0) {
        Serial.println("I'm home");
        beltState = BELT_IDLE;
      }
      break;
  }
}

void beltReset() {
  
  // Smoothly stop belt
  beltStepper.stop();

  while (beltStepper.isRunning()) {
    beltStepper.run();
  }

  // Reset stepper coordinates
  beltStepper.setCurrentPosition(0);

  // Reset state machine
  beltState = BELT_IDLE;

  selectedDrink = -1;
  currentPumpTarget = 0;

  iceWaitStart = 0;
  iceStartTime = 0;

  iceRunning = false;
  waitingForPumpStart = false;

  stepsToHome = 0;

  // Turn off outputs
  digitalWrite(ICE_MOTOR_PIN, LOW);
  digitalWrite(ICE_SENSOR_LED, LOW);
}
