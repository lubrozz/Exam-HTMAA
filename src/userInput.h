#pragma once

extern int selectedDrink; // sets variable to public, so it can be used throughout
extern bool startPressed; // sets variable to public

/* initiate the buttons and LED pins
All LEDs are off by default*/
void userInputInit();

/* main loop 
Checks if button has been pressed
    set the selectedDrink variable to the button index
    Turn off all LEDs except for the selected drink's
When drink button is pressed start blinking green start LED, when start is not pressed
If start button is pressed keep the green LED on and update startPressed to true 
*/
void userInputUpdate();