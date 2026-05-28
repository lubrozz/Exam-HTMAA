#pragma once

/* initiate pump(s) maxspeed and acceleration (stepper only)
initiate sensorpins and their LEDs
initiate DC pump
*/
void pumpInit();

/* main loop
Update LEDs to LOW
When belt arrives and sensor confirms position, start dispensing
    gets cl amount from current drink recipe - taken from userInput.cpp
    starts pumpDispenseCl(pumpIndex, cl)
Check if pump is stepper and then run until pump is not dispensing
    Then call beltContinue() from belt.cpp
Check if pump is DC and run in duration.
    When done, call beltContinue() from belt.cpp
*/
void pumpUpdate();

/* Calculating how long to pump for stepper and DC
Stepper => give number of steps in .move() calculated through clToSteps(cl)
DC => find duration to pump using clToMs(cl) to find number of milliseconds per cl

finally give the go that pump can run.
*/
void pumpDispenseCl(int pumpIndex, int cl);

bool pumpIdle(int pumpIndex);