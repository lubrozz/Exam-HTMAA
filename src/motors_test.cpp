#include <Arduino.h>
#include <AccelStepper.h>
#include "../include/config.h"

AccelStepper beltStepper_test(AccelStepper::DRIVER, BELT_STEP, BELT_DIR);
AccelStepper pumpStepper_test(AccelStepper::DRIVER, LQ1_STEP, LQ1_DIR);

void motorsInit() {
    Serial.begin(9600);
    beltStepper_test.setMaxSpeed(BELT_MAXSPEED);
    beltStepper_test.setAcceleration(BELT_ACCELERATION);

    pumpStepper_test.setMaxSpeed(BELT_MAXSPEED);
    pumpStepper_test.setAcceleration(BELT_ACCELERATION);

    // Sensor pins
  pinMode(ICE_SENSOR_PIN, INPUT);
  pinMode(BELT_SENSOR_1, INPUT);
  pinMode(BELT_SENSOR_2, INPUT);
  pinMode(BELT_SENSOR_3, INPUT);

  // LED pins
  pinMode(ICE_SENSOR_LED, OUTPUT);
  pinMode(LQ1_HALL_LED, OUTPUT);

  // Motor pins
  pinMode(ICE_MOTOR_PIN, OUTPUT);
  pinMode(LQ2_DC_PIN, OUTPUT);

  digitalWrite(ICE_MOTOR_PIN, LOW);
  digitalWrite(LQ2_DC_PIN, LOW);
  digitalWrite(ICE_SENSOR_LED, LOW);
  digitalWrite(LQ1_HALL_LED, LOW);

  
  Serial.println("Motor test ready. Commands:");
  Serial.println("  b = belt 200 steps");
  Serial.println("  B = belt 1000 steps");
  Serial.println("  p = pump1 200 steps");
  Serial.println("  P = pump1 2cl (11000 steps)");
  Serial.println("  d = DC motor toggle");
  Serial.println("  i = ice motor toggle");
  Serial.println("  s = print all sensor states");
}

void motorsUpdate() {
    bool dcOn = false;
    bool iceOn = false;
    beltStepper_test.run();
  pumpStepper_test.run();

  // Continuously mirror sensor states to LEDs
  digitalWrite(ICE_SENSOR_LED, digitalRead(ICE_SENSOR_PIN));
  digitalWrite(LQ1_HALL_LED,   digitalRead(LQ1_HALL) == LOW ? HIGH : LOW);

  // Print sensor states every 500ms
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    Serial.print("Sensors | ICE: ");  Serial.print(digitalRead(ICE_SENSOR_PIN));
    Serial.print(" | S1: ");          Serial.print(digitalRead(BELT_SENSOR_1));
    Serial.print(" | S2: ");          Serial.print(digitalRead(BELT_SENSOR_2));
    Serial.print(" | S3: ");          Serial.print(digitalRead(BELT_SENSOR_3));
    Serial.print(" | LQ1_HALL: ");    Serial.println(digitalRead(LQ1_HALL));
  }

  if (Serial.available()) {
    char cmd = Serial.read();
    switch (cmd) {
      case 'b': beltStepper_test.move(200);    Serial.println("Belt: 200 steps");  break;
      case 'B': beltStepper_test.move(1000);   Serial.println("Belt: 1000 steps"); break;
      case 'p': pumpStepper_test.move(200);   Serial.println("Pump1: 200 steps"); break;
      case 'P': pumpStepper_test.move(11000); Serial.println("Pump1: 2cl");       break;
      case 'd':
        dcOn = !dcOn;
        digitalWrite(LQ2_DC_PIN, dcOn ? HIGH : LOW);
        Serial.println(dcOn ? "DC motor ON" : "DC motor OFF");
        break;
      case 'i':
        iceOn = !iceOn;
        digitalWrite(ICE_MOTOR_PIN, iceOn ? HIGH : LOW);
        Serial.println(iceOn ? "Ice motor ON" : "Ice motor OFF");
        break;
    }
  }
}