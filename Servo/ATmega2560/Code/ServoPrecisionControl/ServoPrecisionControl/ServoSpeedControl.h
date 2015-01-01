/*
 * ServoSpeedControl.h
 *
 * Created: 6/3/2014 12:29:37 PM
 *  Author: Joel
 */ 


#ifndef SERVOSPEEDCONTROL_H_
#define SERVOSPEEDCONTROL_H_

#include "ServoPrecisionControl.h"

//Change MAX_SPEED to the Degrees per second traveled by the servo
//Practically, it changes with the servo supply voltage
#define MAX_SPEED 400 //60 deg in 0.15 sec for NRS-995 at 6.0V

#ifdef DEBUG
volatile int errorg[24] = {0}; //DEBUG code
volatile int errorgset[24] = {0}; //DEBUG code
#endif

typedef struct ServoSpeedData_t
{
	volatile uint16_t target; //The angle to be reached after the end of the movement; volatile to prevent compiler optimisation
	float speed;
} ServoSpeedData;

ServoSpeedData _servosspeeddata[24];

/*
	Sets the servo's speed for all subsequent calls of setTarget to the degrees
	per second specified.
	
	Parameters:
	channel - The servo channel whose speed is to be set
	speed - The speed in degrees per second at which the servo is to rotate
	
*/
void servoSetSpeed(uint8_t channel, float speed)
{
	_servosspeeddata[channel].speed = speed;
	if (!servos[channel].speed_set)
		_servosspeeddata[channel].target = servos[channel].position;
	servos[channel].speed_set = 1;
}

/*
	Instructs the servo to move to the specified angle at the speed previously
	set by a call to the servoSetSpeed() function. This function returns
	immediately, i.e., it is a non-blocking call. To wait till the target angle
	has been attained, call servoWaitTillTarget() function.
	
	Parameters:
	channel - The servo channel whose target is to be set
*/
void servoSetTarget(uint8_t channel, uint8_t degree)
{
	_servosspeeddata[channel].target = servoAngleValue(degree);
}

/*
	Returns the current servo position in degrees. Remember that this is the
	current angle position being sent to the servo at the instant and the
	actual position may be different.
	
	Parameters:
	channel - The servo channel whose position is needed
*/
uint8_t servoGetPosition(uint8_t channel)
{
	return (servos[channel].position - 0x8FAE) / 139;
}

/*
	Function called by the overflow interrupt to set the servo to a new
	position.
	
	NOTE: This function is for private use by the API.
	
	Parameters:
	channel - The servo channel to update
*/
void _update_position(uint8_t channel)
{
	int16_t error = (int32_t)servos[channel].position -
		(int32_t)_servosspeeddata[channel].target;
	if(error > 0)
	{
		error -= (139 * _servosspeeddata[channel].speed / 50);
		if(error < 0)
		{
			error = 0;
		}		
	}
	else if (error < 0)
	{
		error += (139 * _servosspeeddata[channel].speed / 50);
		if (error > 0)
		{
			error = 0;
		}
	}	
	
#ifdef DEBUG
	//errorg[channel] = error;
	//errorgset[channel] = 1;
#endif

	servos[channel].position = _servosspeeddata[channel].target + error;
}

/*
	Blocking call for waiting till the servo has moved to the target position
	
	Parameters:
	channel - The servo channel to wait on.
*/
void servoWaitTillTarget(uint8_t channel)
{
	while(servos[channel].position != _servosspeeddata[channel].target);
}

/*
	Returns a non-zero value if the servo has moved to the target position
	
	Parameters:
	channel - The servo channel concerned.
*/
int servoTargetReached(uint8_t channel)
{
	return (servos[channel].position == _servosspeeddata[channel].target);
}

#endif /* SERVOSPEEDCONTROL_H_ */