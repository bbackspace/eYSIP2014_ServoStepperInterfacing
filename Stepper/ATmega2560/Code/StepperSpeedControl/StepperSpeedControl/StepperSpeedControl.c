/*
 * StepperSpeedControl.c
 *
 * Created: 6/16/2014 9:16:39 AM
 *  Author: Joel
 *
 * This code acts as a sample code to using the stepperspeed.h header file.
 * Refer stepperspeed.h for the various functions' documentation
 *
 */ 

#define F_CPU 14745600

#include <avr/io.h>
#include "stepperspeed.h"
#include <util/delay.h>

int speed = 4;

/*
	The following code demonstrates the speed controlling capability be creating
	a speed ramp. The code starts with setting the speed close to the minimum
	speed supported by the timer, i.e., 4 steps per sec (Minimum is 3.52). It then
	ramps up the speed in a while loop until 400 steps are covered, after which it
	keeps a constant speed until 400 steps are remaining. Subsequently, the speed
	is ramped down in a similar fashion. The process is repeated in the opposite
	direction.
	Note that the speed variable is incremented once every 5 ms. This corresponds
	to an acceleration of 200 steps per sec^2 or 200 Hz/s. The final speed
	attained can be calculated by Newton's laws,
		v^2 - u^2 = 2as
		v^2 = u^2 + 2as
			= 4^2 + 2*200*400
			= 160016
		v = sqrt(160016) = 400.02 Hz
*/
int main(void)
{
	stepperEnableMulti(&PORTL, &PORTL, &PORTD,
					   &PORTD, 7, 6, 1, 0, 0, 200, FULL_STEP);
	
	stepperSetSpeed(0, speed);
	stepperMove(0, 2000, CW);
    while(_steppers[0].stepstomove > 1600)
    {
        speed++;
		stepperSetSpeed(0, speed);
		_delay_ms(5);
    }
	while(_steppers[0].stepstomove > 400)
	{
		
	}
	while(speed > 4)
	{
		speed--;
		stepperSetSpeed(0, speed);
		_delay_ms(5);
	}
	
	speed = 4;
	stepperSetSpeed(0, speed);
	stepperMove(0, 2000, CCW);
	while(_steppers[0].stepstomove > 1600)
	{
		speed++;
		stepperSetSpeed(0, speed);
		_delay_ms(5);
	}
	while(_steppers[0].stepstomove > 400)
	{
		
	}
	while(speed > 4)
	{
		speed--;
		stepperSetSpeed(0, speed);
		_delay_ms(5);
	}
	
	while(1)
	{
		
	}
}
