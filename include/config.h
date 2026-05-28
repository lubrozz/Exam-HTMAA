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
#define BELT_STEP 9
#define BELT_DIR 10
#define BELT_ACCELERATION 1800
#define BELT_MAXSPEED 800

/* ------ BELT SENSOR PINS ------ */
#define BELT_SENSOR_0     3
#define BELT_SENSOR_1     4   // LQ1_HALL (shared with pump 1)
#define BELT_SENSOR_2     7  // LQ2_HALL (shared with pump 2) TODO: replace XX with actual pin
#define BELT_SENSOR_3     13  // LQ3_HALL (shared with pump 3) TODO: replace XX with actual pin

/* ------ ICE DISPENSER ------ */
#define ICE_SENSOR_PIN    3
#define ICE_SENSOR_LED    22
#define ICE_MOTOR_PIN     2
#define ICE_DISPENSE_MS   750 // how long to run ice motor in ms
#define ICE_WAIT_MS 500  // how long to wait at ice dispenser in ms

/* ------ STEPPER LIQUID VARIABLES ------ */
#define STEPS_PER_2CL   10000  // precisly 2Cl
#define STEPPER_MAX_SPEED 1000
#define STEPPER_ACCELERATION 1200
#define PUMP_WAIT_MS 500

/* ------ PUMP TYPES ------ */
#define PUMP_STEPPER    0
#define PUMP_DC         1
#define PUMP_TYPES      {PUMP_STEPPER, PUMP_DC}

/* ------ LIQUID #1 (Stepper) ------ */
#define LQ1_DIR         5
#define LQ1_STEP        6
#define LQ1_HALL        4
#define LQ1_HALL_LED    24

/* ------ LIQUID #2 (DC Motor) ------ */
#define LQ2_HALL        7 
#define LQ2_HALL_LED    26
#define LQ2_DC_PIN      8 
#define DC_MS_PER_CL    40000


/* ------ DRINK RECIPES ------ */
// values are in cl per pump (pump1, pump2)
// 0 = skip this pump
#define DRINK_BLUE      {2, 0}
#define DRINK_RED       {2, 8}
#define DRINK_YELLOW    {0, 4}