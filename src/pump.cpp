#include <Arduino.h>
#include <AccelStepper.h>
#include "pump.h"
#include "belt.h"
#include "../include/config.h"

// ---- Stepper motors ----
static AccelStepper pump1(AccelStepper::DRIVER, LQ1_STEP, LQ1_DIR);

// ---- Config arrays ----
static const int pumpTypes[2]   = PUMP_TYPES;
static const int hallPins[2]    = {LQ1_HALL, LQ2_HALL};
static const int hallLedPins[2] = {LQ1_HALL_LED, LQ2_HALL_LED};
static const int dcPins[2]      = {0, LQ2_DC_PIN}; // only DC pumps need this

// ---- State ----
static bool     pumpRunning[2]    = {false, false};

// DC motor timing
static unsigned long dcStartTime[2] = {0, 0};
static unsigned long dcDuration[2]  = {0, 0}; // ms to run, set on dispense

// Helper: convert cl to steps for stepper pumps
static long clToSteps(int cl) {
  return (long)cl * STEPS_PER_2CL / 2;
}

// Helper: convert cl to ms for DC pumps
static unsigned long clToMs(int cl) {
  return (unsigned long)cl * DC_MS_PER_CL;
}

bool pumpIdle(int pumpIndex) {
  return !pumpRunning[pumpIndex];
}

void pumpInit() {
  // Init stepper pumps
  pump1.setMaxSpeed(STEPPER_MAX_SPEED);
  pump1.setAcceleration(500);

  // Init pins
  for (int i = 0; i < 2; i++) {
    pinMode(hallPins[i], INPUT);
    pinMode(hallLedPins[i], OUTPUT);
    if (pumpTypes[i] == PUMP_DC) {
      pinMode(dcPins[i], OUTPUT);
      digitalWrite(dcPins[i], LOW);
    }
  }
}

void pumpDispenseCl(int pumpIndex, int cl) {
  if (pumpIndex < 0 || pumpIndex > 1 || cl <= 0) return;

  if (pumpTypes[pumpIndex] == PUMP_STEPPER) {
    if (pumpIndex == 0) pump1.move(clToSteps(cl));
  } else if (pumpTypes[pumpIndex] == PUMP_DC) {
    dcDuration[pumpIndex]  = clToMs(cl);
    dcStartTime[pumpIndex] = millis();
    digitalWrite(dcPins[pumpIndex], HIGH);
  }

  pumpRunning[pumpIndex] = true;
}

void pumpUpdate() {
  for (int i = 0; i < 2; i++) {
    // Update hall sensor LED
    bool hallState = digitalRead(hallPins[i]) == LOW;
    digitalWrite(hallLedPins[i], hallState ? HIGH : LOW);

    // ---- Run stepper pumps ----
    if (pumpRunning[i] && pumpTypes[i] == PUMP_STEPPER) {
      if (i == 0) pump1.run();

      AccelStepper* stepper = (i == 0) ? &pump1 : nullptr;
      if (stepper && stepper->distanceToGo() == 0) {
        pumpRunning[i] = false;
      }
    }

    // ---- Run DC pumps ----
    if (pumpRunning[i] && pumpTypes[i] == PUMP_DC) {
      if (millis() - dcStartTime[i] >= dcDuration[i]) {
        digitalWrite(dcPins[i], LOW);
        pumpRunning[i] = false;
      }
    }
  }
}