/*
 * stepper.h
 *
 * Created: 6/11/2014 5:54:23 PM
 *  Author: Joel
 */ 


#ifndef STEPPER_H_ //Header guards to avoid double inclusion
#define STEPPER_H_

//Modify only these constants to your needs - the pins at which the stepper is connected to
#define STEPPERPORTSET IO1SET
#define STEPPERPORTCLR IO1CLR
#define STEPPERPORTDIR IO1DIR
#define COILAPIN 26
#define COILBPIN 27
#define COILCPIN 28
#define COILDPIN 29

//Initialises the stepper ports
void stepper_port_init()
{
	PINSEL2 = (PINSEL2 & 0x8);
	STEPPERPORTDIR |= (1<<COILAPIN) | (1<<COILBPIN) | (1<<COILCPIN) | (1<<COILDPIN);
}

/*
	Instructs the motor to take a step in the wave step mode in the
	specified direction
	Takes care of the current coil excitation state. Make sure you
	use only one kind of stepping mode function as the 3 functions
	all have different state variables.

	direction = 1 for forward, -1 for backward.
*/
void wave_step(int direction)
{
	static int stepindex = -1;
	stepindex = (stepindex + direction + 4) % 4;
	switch(stepindex)
	{
		case 0:
			STEPPERPORTSET = (1<<COILAPIN);
			STEPPERPORTCLR = (1<<COILBPIN);
			STEPPERPORTCLR = (1<<COILCPIN);
			STEPPERPORTCLR = (1<<COILDPIN);
			break;
		case 1:
			STEPPERPORTCLR = (1<<COILAPIN);
			STEPPERPORTSET = (1<<COILBPIN);
			STEPPERPORTCLR = (1<<COILCPIN);
			STEPPERPORTCLR = (1<<COILDPIN);
			break;
		case 2:
			STEPPERPORTCLR = (1<<COILAPIN);
			STEPPERPORTCLR = (1<<COILBPIN);
			STEPPERPORTSET = (1<<COILCPIN);
			STEPPERPORTCLR = (1<<COILDPIN);
			break;
		case 3:
			STEPPERPORTCLR = (1<<COILAPIN);
			STEPPERPORTCLR = (1<<COILBPIN);
			STEPPERPORTCLR = (1<<COILCPIN);
			STEPPERPORTSET = (1<<COILDPIN);
			break;
	}
}

/*
	Instructs the motor to take a step in the full step mode in the
	specified direction
	Takes care of the current coil excitation state. Make sure you
	use only one kind of stepping mode function as the 3 functions
	all have different state variables.

	direction = 1 for forward, -1 for backward.
*/
void full_step(int direction)
{
	static int stepindex = -1;
	stepindex = (stepindex + direction + 4) % 4;
	switch(stepindex)
	{
		case 0:
			STEPPERPORTSET = (1<<COILAPIN);
			STEPPERPORTSET = (1<<COILBPIN);
			STEPPERPORTCLR = (1<<COILCPIN);
			STEPPERPORTCLR = (1<<COILDPIN);
			break;
		case 1:
			STEPPERPORTCLR = (1<<COILAPIN);
			STEPPERPORTSET = (1<<COILBPIN);
			STEPPERPORTSET = (1<<COILCPIN);
			STEPPERPORTCLR = (1<<COILDPIN);
			break;
		case 2:
			STEPPERPORTCLR = (1<<COILAPIN);
			STEPPERPORTCLR = (1<<COILBPIN);
			STEPPERPORTSET = (1<<COILCPIN);
			STEPPERPORTSET = (1<<COILDPIN);
			break;
		case 3:
			STEPPERPORTSET = (1<<COILAPIN);
			STEPPERPORTCLR = (1<<COILBPIN);
			STEPPERPORTCLR = (1<<COILCPIN);
			STEPPERPORTSET = (1<<COILDPIN);
			break;
	}
}

/*
	Instructs the motor to take a step in the half step mode in the
	specified direction
	Takes care of the current coil excitation state. Make sure you
	use only one kind of stepping mode function as the 3 functions
	all have different state variables.

	direction = 1 for forward, -1 for backward.
*/
void half_step(int direction)
{
	static int stepindex = -1;
	stepindex = (stepindex + direction + 8) % 8;
	switch(stepindex)
	{
		case 0:
			STEPPERPORTSET = (1<<COILAPIN);
			STEPPERPORTCLR = (1<<COILBPIN);
			STEPPERPORTCLR = (1<<COILCPIN);
			STEPPERPORTCLR = (1<<COILDPIN);
			break;
		case 1:
			STEPPERPORTSET = (1<<COILAPIN);
			STEPPERPORTSET = (1<<COILBPIN);
			STEPPERPORTCLR = (1<<COILCPIN);
			STEPPERPORTCLR = (1<<COILDPIN);
			break;
		case 2:
			STEPPERPORTCLR = (1<<COILAPIN);
			STEPPERPORTSET = (1<<COILBPIN);
			STEPPERPORTCLR = (1<<COILCPIN);
			STEPPERPORTCLR = (1<<COILDPIN);
			break;
		case 3:
			STEPPERPORTCLR = (1<<COILAPIN);
			STEPPERPORTSET = (1<<COILBPIN);
			STEPPERPORTSET = (1<<COILCPIN);
			STEPPERPORTCLR = (1<<COILDPIN);
			break;
		case 4:
			STEPPERPORTCLR = (1<<COILAPIN);
			STEPPERPORTCLR = (1<<COILBPIN);
			STEPPERPORTSET = (1<<COILCPIN);
			STEPPERPORTCLR = (1<<COILDPIN);
			break;
		case 5:
			STEPPERPORTCLR = (1<<COILAPIN);
			STEPPERPORTCLR = (1<<COILBPIN);
			STEPPERPORTSET = (1<<COILCPIN);
			STEPPERPORTSET = (1<<COILDPIN);
			break;
		case 6:
			STEPPERPORTCLR = (1<<COILAPIN);
			STEPPERPORTCLR = (1<<COILBPIN);
			STEPPERPORTCLR = (1<<COILCPIN);
			STEPPERPORTSET = (1<<COILDPIN);
			break;
		case 7:
			STEPPERPORTSET = (1<<COILAPIN);
			STEPPERPORTCLR = (1<<COILBPIN);
			STEPPERPORTCLR = (1<<COILCPIN);
			STEPPERPORTSET = (1<<COILDPIN);
			break;
	}
}

#endif /* STEPPER_H_ */
