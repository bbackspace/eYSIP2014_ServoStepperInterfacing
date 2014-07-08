/*
 * stepper.h
 *
 * Created: 5/29/2014 3:57:31 PM
 *  Author: Joel
 */ 


#ifndef STEPPER_H_ //Header guards to avoid double inclusion
#define STEPPER_H_

//Modify only these constants to your needs - the pins at which the stepper is connected to
#define COILAPORT PORTL
#define COILBPORT PORTL
#define COILCPORT PORTD
#define COILDPORT PORTD
#define COILAPIN 7
#define COILBPIN 6
#define COILCPIN 1
#define COILDPIN 0

//Initialises the stepper ports for output
void stepper_port_init()
{
	//Change it to your requirements
	DDRL |= (3<<6);
	DDRD |= 3;
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
		COILAPORT |= (1<<COILAPIN);
		COILBPORT &= ~(1<<COILBPIN);
		COILCPORT &= ~(1<<COILCPIN);
		COILDPORT &= ~(1<<COILDPIN);
		break;
	case 1:
		COILAPORT &= ~(1<<COILAPIN);
		COILBPORT |= (1<<COILBPIN);
		COILCPORT &= ~(1<<COILCPIN);
		COILDPORT &= ~(1<<COILDPIN);
		break;
	case 2:
		COILAPORT &= ~(1<<COILAPIN);
		COILBPORT &= ~(1<<COILBPIN);
		COILCPORT |= (1<<COILCPIN);
		COILDPORT &= ~(1<<COILDPIN);
		break;
	case 3:
		COILAPORT &= ~(1<<COILAPIN);
		COILBPORT &= ~(1<<COILBPIN);
		COILCPORT &= ~(1<<COILCPIN);
		COILDPORT |= (1<<COILDPIN);
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
		COILAPORT |= (1<<COILAPIN);
		COILBPORT |= (1<<COILBPIN);
		COILCPORT &= ~(1<<COILCPIN);
		COILDPORT &= ~(1<<COILDPIN);
		break;
	case 1:
		COILAPORT &= ~(1<<COILAPIN);
		COILBPORT |= (1<<COILBPIN);
		COILCPORT |= (1<<COILCPIN);
		COILDPORT &= ~(1<<COILDPIN);
		break;
	case 2:
		COILAPORT &= ~(1<<COILAPIN);
		COILBPORT &= ~(1<<COILBPIN);
		COILCPORT |= (1<<COILCPIN);
		COILDPORT |= (1<<COILDPIN);
		break;
	case 3:
		COILAPORT |= (1<<COILAPIN);
		COILBPORT &= ~(1<<COILBPIN);
		COILCPORT &= ~(1<<COILCPIN);
		COILDPORT |= (1<<COILDPIN);
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
			COILAPORT |= (1<<COILAPIN);
			COILBPORT &= ~(1<<COILBPIN);
			COILCPORT &= ~(1<<COILCPIN);
			COILDPORT &= ~(1<<COILDPIN);
			break;
		case 1:
			COILAPORT |= (1<<COILAPIN);
			COILBPORT |= (1<<COILBPIN);
			COILCPORT &= ~(1<<COILCPIN);
			COILDPORT &= ~(1<<COILDPIN);
			break;
		case 2:
			COILAPORT &= ~(1<<COILAPIN);
			COILBPORT |= (1<<COILBPIN);
			COILCPORT &= ~(1<<COILCPIN);
			COILDPORT &= ~(1<<COILDPIN);
			break;
		case 3:
			COILAPORT &= ~(1<<COILAPIN);
			COILBPORT |= (1<<COILBPIN);
			COILCPORT |= (1<<COILCPIN);
			COILDPORT &= ~(1<<COILDPIN);
			break;
		case 4:
			COILAPORT &= ~(1<<COILAPIN);
			COILBPORT &= ~(1<<COILBPIN);
			COILCPORT |= (1<<COILCPIN);
			COILDPORT &= ~(1<<COILDPIN);
			break;
		case 5:
			COILAPORT &= ~(1<<COILAPIN);
			COILBPORT &= ~(1<<COILBPIN);
			COILCPORT |= (1<<COILCPIN);
			COILDPORT |= (1<<COILDPIN);
			break;
		case 6:
			COILAPORT &= ~(1<<COILAPIN);
			COILBPORT &= ~(1<<COILBPIN);
			COILCPORT &= ~(1<<COILCPIN);
			COILDPORT |= (1<<COILDPIN);
			break;
		case 7:
			COILAPORT |= (1<<COILAPIN);
			COILBPORT &= ~(1<<COILBPIN);
			COILCPORT &= ~(1<<COILCPIN);
			COILDPORT |= (1<<COILDPIN);
			break;
	}
}

#endif /* STEPPER_H_ */