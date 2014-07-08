/*
 * StepperControl.c
 *
 * Created: 5/29/2014 3:56:53 PM
 *  Author: Joel
 */ 

#define F_CPU 14745600
#define SPEED 300 //Speed (stepping frequency) of the stepper motor in Hz

#include <avr/io.h>
#include <avr/interrupt.h>
#include "stepper.h" //Contains functions related to control of stepper motors

volatile int stepcount = 0; //Holds the count of steps rotated
volatile short direction = 1; //Current direction of rotation +1 for clockwise and -1 for anticlockwise

//Compare match interrupt that steps after the required time delay
ISR (TIMER1_COMPA_vect)
{
	wave_step(direction);
	stepcount++;
	if(stepcount > 200) //Change direction every revolution
	{
		direction *= -1;
		stepcount = 0;
	}
}
int main(void)
{
	stepper_port_init();
	
	cli(); //Clear global interrupts
	TCCR1B |= (1 << WGM12); //CTC mode (WGM13:0 = 0100)
	TIMSK1 |= (1 << OCIE1A); //Enable CTC interrupt
	sei(); //Enable global interrupts

	OCR1A = (F_CPU / SPEED) - 1; //Set TOP
	
	//Prescalar = 1
    TCCR1B |= ((0 << CS12) | (0 << CS11) | (1 << CS10));
	
	while(1);
}
