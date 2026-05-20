/*
This file includies defintions for
- PINS
- Constant variables

To be used in other files in /src
*/
#pragma once

/* ------ USER INPUT AREA ------ */
#define BUTTON_PINS     {43, 42, 41, 40} // order = blue, red, yellow, green (start)
#define BUTTON_LEDS     {49, 48, 47, 46} // order = blue, red, yellow, green (start) - placed by button on breadboard
#define COUNT           3       // number of drink buttons (excludes start)
#define BLINK_INTERVAL  1000

/* ------ BELT STEPPER ------ */
#define BELT_STEP 2
#define BELT_DIR 3
#define BELT_ACCELERATION 500
#define BELT_MAXSPEED 1000

/* ------ BELT SENSOR PINS ------ */
#define BELT_SENSOR_0   19
#define BELT_SENSOR_1     7   // LQ1_HALL (shared with pump 1)
#define BELT_SENSOR_2     1  // LQ2_HALL (shared with pump 2) TODO: replace XX with actual pin
#define BELT_SENSOR_3     4  // LQ3_HALL (shared with pump 3) TODO: replace XX with actual pin
#define BELT_HOME_STEPS   5000 // steps from sensor 3 to home position

/* ------ ICE DISPENSER ------ */
#define ICE_SENSOR_PIN    3
#define ICE_SENSOR_LED    22
#define ICE_MOTOR_PIN     2
#define ICE_DISPENSE_MS   2000 // how long to run ice motor in ms

/* ------ STEPPER LIQUID VARIABLES ------ */
#define STEPS_PER_2CL   11000  // PRECISELY 2CL DO NOT CHANGE!!

/* ------ PUMP TYPES ------ */
#define PUMP_STEPPER    0
#define PUMP_DC         1
#define PUMP_TYPES      {PUMP_STEPPER, PUMP_DC, PUMP_STEPPER}

/* ------ LIQUID #1 (Stepper) ------ */
#define LQ1_DIR         6
#define LQ1_STEP        5
#define LQ1_HALL        4
#define LQ1_HALL_LED    24

/* ------ LIQUID #2 (DC Motor) ------ */
#define LQ2_HALL        7  // TODO: replace with actual pin
#define LQ2_HALL_LED    26  // TODO: replace with actual pin
#define LQ2_DC_PIN      8  // TODO: replace with actual pin
#define DC_MS_PER_CL    500 // TODO: calibrate - ms per cl for DC motor

/* ------ LIQUID #3 (Stepper) ------ */
#define LQ3_DIR         7  // TODO: replace with actual pin
#define LQ3_STEP        6  // TODO: replace with actual pin
#define LQ3_HALL        4  // TODO: replace with actual pin
#define LQ3_HALL_LED    5  // TODO: replace with actual pin


/* ------ DRINK RECIPES ------ */
// values are in cl per pump (pump1, pump2, pump3)
// 0 = skip this pump
#define DRINK_BLUE      {2, 0, 0}
#define DRINK_RED       {0, 4, 0}
#define DRINK_YELLOW    {2, 0, 2}