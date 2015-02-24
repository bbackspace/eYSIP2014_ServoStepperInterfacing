/*
 * ServoPrecisionControl.h
 *
 * By Joel M. Pinto
 *
 * The code is derived from NEX Robotics' documentation on precise servo control
 * using a single timer to control upto 24 servos at a time.
 *
 */

/********************************************************************************

   Copyright (c) 2010, NEX Robotics Pvt. Ltd.                       -*- c -*-
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   * Source code can be used for academic purpose. 
	 For commercial use permission form the author needs to be taken.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. 

  Software released under Creative Commons cc by-nc-sa license.
  For legal information refer to: 
  http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode

*********************************************************************************/

#ifndef SERVOPRECISIONCONTROL_H_
#define SERVOPRECISIONCONTROL_H_

#ifndef F_CPU
#define F_CPU 14745600
#endif

struct servo_t
{
	volatile uint8_t *port;
	uint8_t pin;
	
	volatile uint16_t position; //Current position of the servo; volatile to prevent compiler optimisation
	short enabled; //State variable denoting enabled status of servo
#ifdef SERVOSPEEDCONTROL_H_
	short speed_set; //1 if speed control is being used
#endif
} Servo;

//State variable for the current set number being processed in the interrupts
uint8_t _set_number = 0;

volatile struct servo_t servos[24];

void _timer1_init();
uint16_t servoAngleValue(unsigned int degree);

#ifdef SERVOSPEEDCONTROL_H_
void _update_position(uint8_t channel);
#endif

/*
	Initialises the servo control system including timer.
	
	Parameters:
	None
*/
void servoInit()
{
	for(int i = 0; i < 24; i++)
	{
		servos[i].enabled = 0;
		servos[i].position = 0;
#ifdef SERVOSPEEDCONTROL_H_
		servos[i].speed_set = 0;
#endif
	}
	_timer1_init(); // initialise timer1
	TIMSK1 = 0x0F; // timer1 interrupt sources
}

/*
	To start using a servo connected at the specified port and pin and
	initialise it to a specified angle. The channel is a number between 0 and
	23 (inclusive) to assign to this servo for further identification and
	reference.
	
	Parameters:
	port - The address of the port to which the servo is connected
		   Use &PORTx to specify the port. x = A to L except I
	pin - The pin number on the port where the servo is connected
	channel - The channel number to be assigned to this servo
	degrees - The angle in degrees to which the servo is to be initially set.
	
	Example: Call,
	servo_enable(&PORTL, PL5, 4, 90);
	to start using a servo at PORTL at PL5 at channel 4 and to initialise it to
	90 degrees.
*/
void servoEnable(volatile uint8_t *port, uint8_t pin, uint8_t channel, uint8_t degrees)
{
	if(channel > 24)
		return;
	servos[channel].port = port;
	servos[channel].pin = pin;
	
	*(port - 1) |= 1<<pin; //Set DDR pin to output
	*(port) |= 1<<pin; //Set PORT pin to 1
	
	servos[channel].position = servoAngleValue(degrees);
	servos[channel].enabled = 1;
}


/*
	Makes the servo at the specified channel free to move.
	
	Parameters:
	channel - The channel of the servo to be freed
*/
void servoFree(uint8_t channel)
{
	servos[channel].enabled = 0;
}

/*
	Sends a high-to-low edge to the servo at the specified channel
	
	NOTE: This function is for private use by the API.
	
	Parameters:
	channel - The servo channel to which a high-to-low edge is to be sent
*/
void _reset_servo(uint8_t channel)
{
	*(servos[channel].port) &= ~(1 << servos[channel].pin);
}

/*
	Sends a low-to-high edge to the servo at the specified channel
	
	NOTE: This function is for private use by the API.
	
	Parameters:
	channel - The servo channel to which a low-to-high edge is to be sent
*/
void _set_servo(uint8_t channel)
{
	*(servos[channel].port) |= (1 << servos[channel].pin);
}

/*
	Initialises Timer1 for using in generating the PWM signal to the 24 servos.
	
	Timer1 configuration:
	Prescalar: 1
	Waveform Generation Mode(WGM): 0) Normal, TOP=0xFFFF
	Frequency: 400Hz
	Preload value(BOTTOM): 0x6FFF

	NOTE: This function is for private use by the API.
	
	Parameters:
	None
*/
void _timer1_init(void)
{
	TCCR1B = 0x00; //stop
	TCNT1 = 0x6FFF;
	OCR1A = 0x8FFF;
	OCR1B = 0x8FFF;
	OCR1C = 0x0000;
	ICR1  = 0x8FFF;
	TCCR1A = 0x00;
	TCCR1C = 0x00;
	TCCR1B = 0x01; //start Timer
}

/*
	Interrupt Service Routine for compare match on servos 0, 3, 6, 9, 12, 15,
	18, 21
*/
ISR(TIMER1_COMPA_vect)
{
	_reset_servo(3 * _set_number);
}

/*
	Interrupt Service Routine for compare match on servos 1, 4, 7, 10, 13, 16,
	19, 22
*/
ISR(TIMER1_COMPB_vect)
{
	_reset_servo(3 * _set_number + 1);
}

/*
	Interrupt Service Routine for compare match on servos 2, 5, 8, 11, 14, 17,
	20, 23
*/
ISR(TIMER1_COMPC_vect)
{
	_reset_servo(3 * _set_number + 2);
}

/*
	Interrupt Service Routine for overflow interrupt
*/
ISR(TIMER1_OVF_vect)
{
	//TIMER1 has overflowed
	TCNT1 = 0x6FFF; //reload counter value for 400Hz

	_set_number++;

	if(_set_number > 7) // 400Hz/8 = 50Hz
	{
		_set_number = 0;
	}
	int ch = 3 * _set_number;
	if(servos[ch].enabled)
		_set_servo(ch);
	if(servos[ch + 1].enabled)
		_set_servo(ch + 1);
	if(servos[ch + 2].enabled)
		_set_servo(ch + 2);

	if(servos[ch].enabled)
	{
#ifdef SERVOSPEEDCONTROL_H_
		if(servos[ch].speed_set)
			_update_position(ch);
#endif
		OCR1A = servos[ch].position;
	}
	else
	{
		OCR1A = 0xFFFF;
	}
	if(servos[ch + 1].enabled)
	{
#ifdef SERVOSPEEDCONTROL_H_
		if(servos[ch + 1].speed_set)
			_update_position(ch + 1);
#endif
		OCR1B = servos[ch + 1].position;
	}
	else
	{
		OCR1B = 0xFFFF;
	}
	if(servos[ch + 2].enabled)
	{
#ifdef SERVOSPEEDCONTROL_H_
		if(servos[ch + 2].speed_set)
			_update_position(ch + 2);
#endif
		OCR1C = servos[ch + 2].position;
	}
	else
	{
		OCR1C = 0xFFFF;
	}
}

/*
	Returns the OCR value to be set to achieve the specified angle in degrees
	
	Parameters:
	degree - The angle in degrees
*/
uint16_t servoAngleValue(unsigned int degree)
{
	uint16_t angle = 0;

	if (degree > 180)
	degree = 180;

	angle = (uint16_t)(37518.36 + 139.264 * (float)degree); //actual constant is 139.4
	return angle;
}

/*
	Moves the servo at the specified channel to the specified angle in degrees
	
	Parameters:
	channel - The channel of the servo to move
	degree - The angle in degrees to which the servo is to be moved
*/
void servoMove(uint8_t channel, uint8_t degree)
{
	if(channel > 24)
		return;
	servos[channel].position = servoAngleValue(degree);
	servos[channel].enabled = 1;
}

#endif /* SERVOPRECISIONCONTROL_H_ */