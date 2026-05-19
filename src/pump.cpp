#include <Arduino.h>
#include <AccelStepper.h>
#include "pump.h"
#include "belt.h"
#include "../include/config.h"

// ---- Stepper motors ----
static AccelStepper pump1(AccelStepper::DRIVER, LQ1_STEP, LQ1_DIR);
// static AccelStepper pump3(AccelStepper::DRIVER, LQ3_STEP, LQ3_DIR); // TODO: uncomment when wired

// ---- Config arrays ----
static const int pumpTypes[3]   = PUMP_TYPES;
static const int hallPins[3]    = {LQ1_HALL, LQ2_HALL, LQ3_HALL};
static const int hallLedPins[3] = {LQ1_HALL_LED, LQ2_HALL_LED, LQ3_HALL_LED};
static const int dcPins[3]      = {0, LQ2_DC_PIN, 0}; // only DC pumps need this

// ---- State ----
static bool     pumpRunning[3]    = {false, false, false};
static bool     hallTriggered[3]  = {false, false, false};
static bool     lastHallState[3]  = {LOW,   LOW,   LOW};

// DC motor timing
static unsigned long dcStartTime[3] = {0, 0, 0};
static unsigned long dcDuration[3]  = {0, 0, 0}; // ms to run, set on dispense

// Helper: convert cl to steps for stepper pumps
static long clToSteps(int cl) {
  return (long)cl * STEPS_PER_2CL / 2;
}

// Helper: convert cl to ms for DC pumps
static unsigned long clToMs(int cl) {
  return (unsigned long)cl * DC_MS_PER_CL;
}

void pumpInit() {
  // Init stepper pumps
  pump1.setMaxSpeed(1000);
  pump1.setAcceleration(500);
  // pump3.setMaxSpeed(1000); pump3.setAcceleration(500); // TODO: uncomment when wired

  // Init pins
  for (int i = 0; i < 3; i++) {
    pinMode(hallPins[i], INPUT);
    pinMode(hallLedPins[i], OUTPUT);
    if (pumpTypes[i] == PUMP_DC) {
      pinMode(dcPins[i], OUTPUT);
      digitalWrite(dcPins[i], LOW);
    }
  }
}

void pumpDispenseCl(int pumpIndex, int cl) {
  if (pumpIndex < 0 || pumpIndex > 2 || cl <= 0) return;

  if (pumpTypes[pumpIndex] == PUMP_STEPPER) {
    if (pumpIndex == 0) pump1.move(clToSteps(cl));
    // if (pumpIndex == 2) pump3.move(clToSteps(cl)); // TODO: uncomment when wired
  } else if (pumpTypes[pumpIndex] == PUMP_DC) {
    dcDuration[pumpIndex]  = clToMs(cl);
    dcStartTime[pumpIndex] = millis();
    digitalWrite(dcPins[pumpIndex], HIGH);
  }

  pumpRunning[pumpIndex] = true;
}

void pumpUpdate() {
  for (int i = 0; i < 3; i++) {
    // Update hall sensor LED
    bool hallState = digitalRead(hallPins[i]) == LOW;
    digitalWrite(hallLedPins[i], hallState ? HIGH : LOW);

    // Trigger dispense when belt arrives and hall confirms position
    if (beltAtDestination() && hallState && !hallTriggered[i]) {
      hallTriggered[i] = true;
      // Get cl amount from the current drink recipe
      const int recipes[3][3] = {DRINK_BLUE, DRINK_RED, DRINK_YELLOW};
      extern int selectedDrink; // from userInput.cpp
      int cl = recipes[selectedDrink][i];
      if (cl > 0) {
        pumpDispenseCl(i, cl);
      }
    }

    if (!hallState) {
      hallTriggered[i] = false;
    }

    // ---- Run stepper pumps ----
    if (pumpRunning[i] && pumpTypes[i] == PUMP_STEPPER) {
      if (i == 0) pump1.run();
      // if (i == 2) pump3.run(); // TODO: uncomment when wired

      AccelStepper* stepper = (i == 0) ? &pump1 : nullptr;
      // AccelStepper* stepper = (i == 0) ? &pump1 : &pump3; // TODO: when pump3 wired
      if (stepper && stepper->distanceToGo() == 0) {
        pumpRunning[i] = false;
        beltContinue();
      }
    }

    // ---- Run DC pumps ----
    if (pumpRunning[i] && pumpTypes[i] == PUMP_DC) {
      if (millis() - dcStartTime[i] >= dcDuration[i]) {
        digitalWrite(dcPins[i], LOW);
        pumpRunning[i] = false;
        beltContinue();
      }
    }
  }
}