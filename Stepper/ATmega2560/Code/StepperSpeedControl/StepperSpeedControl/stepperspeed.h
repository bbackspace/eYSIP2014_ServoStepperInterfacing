/*
 * stepperspeed.h
 *
 * Created: 6/16/2014 9:17:13 AM
 *  Author: Joel
 */ 


#ifndef STEPPERSPEED_H_
#define STEPPERSPEED_H_

//Include dependencies in case they aren't included already
#include <avr/io.h>
#include <avr/interrupt.h>

//Constants for the stepping modes
#define WAVE_STEP 0
#define FULL_STEP 1
#define HALF_STEP 2

//Constants for the directions
#define CW 1
#define CCW 0

//Stepper motor's stepping sequence
uint8_t seq[8] = {
	0b1000,
	0b1100,
	0b0100,
	0b0110,
	0b0010,
	0b0011,
	0b0001,
	0b1001
};

struct stepper_t
{
	//Each pin can be taken from a different port for maximum flexibility
	volatile uint8_t *port[4];
	uint8_t pin[4];
	
	uint16_t stepsperrev;
	uint8_t stepmode;
	
	//Data members declared volatile to prevent compiler optimization as the
	//variables might get modified in an interrupt or the main line of execution
	volatile float freq;
	volatile uint8_t direction;
	volatile uint8_t stepindex;
	volatile uint16_t stepstomove;
};

struct stepper_t _steppers[3];
short _timer3enabled = 0; //State variable denoting status of timer3

/*
	To start using a stepper with wires connected at the specified ports and
	pins. The channel is a number either 0, 1 or 2 to assign to this stepper
	for further identification and reference. This function allows all 4 wires
	to be connected at different ports. If only a single port is used, consider
	using the simpler stepperEnable() function.
	
	Parameters:
	port1 - The address of the port to which the stepper's wire1 is connected
	port2 - The address of the port to which the stepper's wire2 is connected
	port3 - The address of the port to which the stepper's wire3 is connected
	port4 - The address of the port to which the stepper's wire4 is connected
			Use &PORTx to specify the port. x = A to L except I
	pin1 - The pin number on port1 where wire1 is connected
	pin2 - The pin number on port2 where wire2 is connected
	pin3 - The pin number on port3 where wire3 is connected
	pin4 - The pin number on port4 where wire4 is connected
	
	channel - The channel number to be assigned to this servo
	stepsperrev - The number of steps the stepper takes to complete a revolution
	stepmode - The stepping mode with which to run the stepper.
			   Either of WAVE_STEP, FULL_STEP and HALF_STEP
			   
	Example of usage:
	To start using a stepper connected at,
		Wire1 - PB1,
		Wire2 - PL5,
		Wire3 - PJ6,
		Wire4 - PD0
	at channel 2 and which has a steps per revolution of 200 (1.8 degrees step
	angle) in half stepping mode, call
	stepperEnableMulti(&PORTB, &PORTL, &PORTJ, &PORTD, 1, 5, 6, 0, 2, 200, HALF_STEP);
*/
void stepperEnableMulti(volatile uint8_t *port1, volatile uint8_t *port2,
						volatile uint8_t *port3, volatile uint8_t *port4,
						uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4,
						uint8_t channel, uint16_t stepsperrev, uint8_t stepmode)
{
	_steppers[channel].port[0] = port1;
	_steppers[channel].port[1] = port2;
	_steppers[channel].port[2] = port3;
	_steppers[channel].port[3] = port4;
	_steppers[channel].pin[0] = pin1;
	_steppers[channel].pin[1] = pin2;
	_steppers[channel].pin[2] = pin3;
	_steppers[channel].pin[3] = pin4;
	uint8_t i;
	for(i = 0; i < 4; i++)
	{
		*(_steppers[channel].port[i] - 1) |= (1<<_steppers[channel].pin[i]); //Set DDR pins to output
		*(_steppers[channel].port[i]) &= ~(1<<_steppers[channel].pin[i]); //Set PORT pins to 0
	}
	_steppers[channel].stepsperrev = stepsperrev;
	_steppers[channel].stepmode = stepmode;
	if(stepmode == FULL_STEP)
		_steppers[channel].stepindex = 1;
	else
		_steppers[channel].stepindex = 0;
		
	//Enable timer3 on first call of stepperEnable()
	if(!_timer3enabled)
	{
		//Initialise the data members used by the interrupts before starting timer
		int i;
		for(i = 0; i < 3; i++)
		{
			_steppers[i].stepstomove = 0;
			_steppers[i].direction = CW;
			_steppers[i].freq = 10;
		}
		_stepper_timer3_init();
		_timer3enabled = 1;
	}	
}

/*
	To start using a stepper with wires connected at the specified port and
	pins. The channel is a number either 0, 1 or 2 to assign to this stepper
	for further identification and reference. This function allows all 4 wires
	to be connected at a single port. If multiple ports are used, consider
	using the stepperEnableMulti() function.
	
	This is just a convenience function for the times when only a single port
	is used for all 4 wires of the stepper. This function in turn calls
	stepperEnableMulti().
	
	Parameters:
	port - The address of the port to which the stepper's wires are connected
			Use &PORTx to specify the port. x = A to L except I
	pin1 - The pin number on port1 where wire1 is connected
	pin2 - The pin number on port2 where wire2 is connected
	pin3 - The pin number on port3 where wire3 is connected
	pin4 - The pin number on port4 where wire4 is connected
	
	channel - The channel number to be assigned to this servo
	stepsperrev - The number of steps the stepper takes to complete a revolution
	stepmode - The stepping mode with which to run the stepper.
			   Either of WAVE_STEP, FULL_STEP and HALF_STEP
			   
	Example of usage:
	To start using a stepper connected at,
		Wire1 - PB1,
		Wire2 - PB5,
		Wire3 - PB6,
		Wire4 - PB0
	at channel 2 and which has a steps per revolution of 200 (1.8 degrees step
	angle) in half stepping mode, call
	stepperEnableMulti(&PORTB, 1, 5, 6, 0, 2, 200, HALF_STEP);
*/
void stepperEnable(volatile uint8_t *port, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4,
					uint8_t channel, uint16_t stepsperrev, uint8_t stepmode)
{
	stepperEnableMulti(port, port, port, port, pin1, pin2, pin3, pin4, channel, stepsperrev, stepmode);
}

/*
	Initialises Timer3 for using in generating the stepping sequences at the
	precise intervals of time.
	
	Timer3 configuration:
	Prescalar: 64
	Waveform Generation Mode(WGM): 0) Normal, TOP=0xFFFF
	Frequency: 230.4kHz

	NOTE: This function is for private use by the API.
	
	Parameters:
	None
*/
void _stepper_timer3_init()
{
	cli(); //Stop global interrupts
	TCCR3B = 0x00; //Stop timer
	TCCR3A = 0x00;
	TCCR3C = 0x00;
	TCCR3B = 0b00000011; //Start Timer at F_CPU/64
	TIMSK3 = 0b00001110; //Enable Compare Match Interrupts on timer channels A, B and C
	sei(); //Start global interrupts
}

/*
	Interrupt Service Routine for compare match on the stepper channel 0
*/
ISR(TIMER3_COMPA_vect)
{
	if(_steppers[0].stepstomove > 0)
	{
		step(0, _steppers[0].direction);
		_steppers[0].stepstomove--;
		
		//Schedule the next step after (1/freq) seconds
		OCR3A = OCR3A + (uint16_t)(230400.0 / _steppers[0].freq);
	}
}

/*
	Interrupt Service Routine for compare match on the stepper channel 1
*/
ISR(TIMER3_COMPB_vect)
{
	if(_steppers[1].stepstomove > 0)
	{
		step(1, _steppers[1].direction);
		_steppers[1].stepstomove--;
		
		//Schedule the next step after (1/freq) seconds
		OCR3B = OCR3B + (uint16_t)(230400.0 / _steppers[1].freq);
	}
}

/*
	Interrupt Service Routine for compare match on the stepper channel 2
*/
ISR(TIMER3_COMPC_vect)
{
	if(_steppers[2].stepstomove > 0)
	{
		step(2, _steppers[2].direction);
		_steppers[2].stepstomove--;
		
		//Schedule the next step after (1/freq) seconds
		OCR3C = OCR3C + (uint16_t)(230400.0 / _steppers[2].freq);
	}
}

/*
	Sets the stepper's speed for all subsequent steps to the steps per second
	specified.
	
	Parameters:
	channel - The stepper channel whose speed is to be set
	stepspersec - The speed in steps per second at which the stepper should rotate
				  Minimum speed = 3.52Hz
				  Maximum speed = 230400Hz
*/
void stepperSetSpeed(uint8_t channel, float stepspersec)
{
	_steppers[channel].freq = stepspersec;
}

/*
	Instructs the stepper to move the specified number of steps at the speed
	previously set by a call to the stepperSetSpeed() function. This function
	returns immediately, i.e., it is a non-blocking call. To wait till the
	steps are completed, call stepperWaitTillComplete() function.
	
	To move the stepper by specifying degrees, consider stepperMoveDegrees().
	Parameters:
	channel - The stepper channel to be moved
	steps - The number of steps to move
	direction - The direction in which to move the stepper
				Either CW or CCW for clockwise and anticlockwise respectively
*/
void stepperMove(uint8_t channel, uint16_t steps, uint8_t direction)
{
	_steppers[channel].direction = direction;
	_steppers[channel].stepstomove = steps;
	
	//Schedule the first step at (1/freq) seconds from current timer count
	switch(channel)
	{
		case 0:
			OCR3A = TCNT3 + (uint16_t)(230400.0 / _steppers[0].freq);
			break;
		case 1:
			OCR3B = TCNT3 + (uint16_t)(230400.0 / _steppers[1].freq);
			break;
		case 2:
			OCR3B = TCNT3 + (uint16_t)(230400.0 / _steppers[2].freq);
	}
}

/*
	Instructs the stepper to move the specified degrees at the speed
	previously set by a call to the stepperSetSpeed() function. This function
	returns immediately, i.e., it is a non-blocking call. To wait till the
	steps are completed, call stepperWaitTillComplete() function.
	
	To move the stepper by specifying number of steps, consider stepperMove().
	Keep in mind that the degrees specified will be converted to the equivalent
	number of steps and thus angles only in multiples of the step angle can be
	attained. This function is provided just for convenience and in turn calls
	stepperMove().
	
	Parameters:
	channel - The stepper channel to be moved
	degrees - The degrees to move
	direction - The direction in which to move the stepper
				Either CW or CCW for clockwise and anticlockwise respectively
*/
void stepperMoveDegrees(uint8_t channel, float degrees, uint8_t direction)
{
	if(_steppers[channel].stepmode == HALF_STEP) //Half stepping divides the step angle by half
		stepperMove(channel, (uint16_t)(degrees * (float)_steppers[channel].stepsperrev / 180.0), direction);
	else
		stepperMove(channel, (uint16_t)(degrees * (float)_steppers[channel].stepsperrev / 360.0), direction);
}

/*
	Instructs the stepper to move a single step immediately in the specified
	direction.
	
	Parameters:
	channel - The channel of the stepper to be stepped
	direction - The direction in which to move the stepper
				Either CW or CCW for clockwise and anticlockwise respectively
*/
void step(uint8_t channel, uint8_t direction)
{
	//Calculate the next step sequence based on the stepping mode and direction
	int delta = 1;
	if(_steppers[channel].stepmode != HALF_STEP)
		delta = 2;
	if(direction == CCW)
		delta = -delta;
	_steppers[channel].stepindex = (_steppers[channel].stepindex + 8 + delta) & 0x07;
	
	//Send the sequence to each of the stepper port pins
	uint8_t temp = seq[_steppers[channel].stepindex];
	int8_t i;
	for(i = 0; i < 4; i++)
	{
		if(temp & (1 << (3 - i)))
			*(_steppers[channel].port[i]) |= (1 << _steppers[channel].pin[i]);
		else
			*(_steppers[channel].port[i]) &= ~(1 << _steppers[channel].pin[i]);
	}
}

/*
	Blocking call for waiting till the stepper has moved the said number of
	steps
	
	Parameters:
	channel - The stepper channel to wait on.
*/
void stepperWaitTillComplete(uint8_t channel)
{
	//This while loop exits when the Compare Match interrupts decrease the
	//stepstomove to zero.
	while(_steppers[channel].stepstomove > 0);
}

/*
	Returns a non-zero value if the stepper has moved the said number of steps
	
	Parameters:
	channel - The stepper channel concerned.
*/
int stepperMoveCompleted(uint8_t channel)
{
	return (_steppers[channel].stepstomove > 0);
}

#endif /* STEPPERSPEED_H_ */