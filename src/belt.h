#pragma once

/* Initialize the pins used and their modes.
Set the max speed and acceleration for the belt 
*/

int beltCurrentPump();
int beltGetRecipeCl(int drink, int pump);
void beltInit();

/* The main loop 
Updates the sensor states to begin with
Switch case on the belt state 
BELT_IDLE => break switch case
BELT_MOVING_TO_ICE => run belt, stop if ICE_SENSOR triggered, change belt state and start dispensing ice
BELT_AT_ICE => dispense ice for given time. When done, start moving to pump
BELT_MOVING_TO_PUMP => move belt. Sensor triggered, check if current drink should stop there.
    if yes, change belt state to pump, signal pump to start
    else, look for next pump target
    if pump target is done move to finish position and belt state BELT_RETURNING_HOME
BELT_AT_PUMP => calls pump.cpp, will call beltContinue() when done.
BELT_RETURNING_HOME => change belt state to BELT_IDLE when at finished position
*/
void beltUpdate();

/* set variables for the belt
what drink is chosen gives ingredient list
pumptarget always starts at 0
sensor is not triggered
belt state is BELT_MOVING_TO_ICE
.move(10000) is big number to just keep moving. Will be stopped by function .stop()
*/
void beltStart(int drinkIndex);

/* see if cup is at destination */
bool beltAtDestination();

/* set the belt state to BELT_IDLE */
bool beltIdle();

/* called by pump.cpp 
Resets the sensorTriggered to false
Up to next pump target
Find out what the next pump target is
If no more targets, then start the move to end position
    else, move the belt and set belt state to BELT_MOVING_TO_PUMP
*/
void beltContinue();