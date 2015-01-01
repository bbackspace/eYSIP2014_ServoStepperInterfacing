/*
 * ServoPrecisionControl.c
 *
 * By Joel M. Pinto
 *
 * This code acts as a sample code to using the ServoSpeedControl.h and
 * ServoPrecisionControl.h headers. There are 2 main() functions defined,
 * one for each of the two. Uncomment and test the respective test code.
 *
 * Refer the header files for the various functions' documentation.
 *
 */

#define F_CPU 14745600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"
#include "ServoSpeedControl.h"

//--------------------------------------------------------------------------------
//port initialisation for Servos i/p's
//--------------------------------------------------------------------------------
void servo_pin_config (void)
{
	DDRB = DDRB | 0x20; //making PORTB 5 pin output
	PORTB = PORTB | 0x20; //setting PORTB 5 pin to logic 1
	DDRB = DDRB | 0x40; //making PORTB 6 pin output
	PORTB = PORTB | 0x40; //setting PORTB 6 pin to logic 1
	DDRB = DDRB | 0x80; //making PORTB 7 pin output
	PORTB = PORTB | 0x80; //setting PORTB 7 pin to logic 1
}


void lcd_port_config (void)
{
	DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
	PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

//--------------------------------------------------------------------------------
//call this routine to initialize all peripherals
//--------------------------------------------------------------------------------
void init_devices(void)
{
	//stop errant interrupts until set up
	cli();                                // disable all interrupts

	lcd_port_config();
	servo_pin_config();                   // servo configuration 
	servoInit();
 
	sei(); //re-enable interrupts
	//all peripherals are now initialized
}

//Test program for Servo Precision Control
/*
int main(void) 
{
	init_devices();
 
	servo_enable(_SFR_IO_ADDR(PORTB), 6, 0, 0);
	_delay_ms(450);
 
	while(1)
	{
		// demo calls for testing
		servo_move(0, 0);
		_delay_ms(450);
		servo_move(0, 45);
		_delay_ms(120);
		servo_move(0, 90);
		_delay_ms(120);
		servo_move(0, 135);
		_delay_ms(120);
		servo_move(0, 180);
		_delay_ms(120);
	}
}
*/

//Test program for Servo Speed Control
int main()
{
	init_devices();
	lcd_init();
	servoEnable(&PORTB, 5, 0, 0); //Enable servo connected at PB5 and assign it channel 0 and set it to 0 degrees.
	_delay_ms(3000);
	
	servoSetSpeed(0, 400);
	servoSetTarget(0, 180);
	lcdprintf("400");
	servoWaitTillTarget(0);
	_delay_ms(3000);
	lcd_clear();

	servoSetSpeed(0, 200);
	servoSetTarget(0, 0);
	lcdprintf("200");
	servoWaitTillTarget(0);
	_delay_ms(3000);
	lcd_clear();
	
	servoSetSpeed(0, 100);
	servoSetTarget(0, 180);
	lcdprintf("100");
	servoWaitTillTarget(0);
	_delay_ms(3000);
	lcd_clear();
	
	servoSetSpeed(0, 50);
	servoSetTarget(0, 0);
	lcdprintf("50");
	servoWaitTillTarget(0);
	_delay_ms(3000);
	lcd_clear();
	
	servoSetSpeed(0, 10);
	servoSetTarget(0, 180);
	lcdprintf("10");
	while(1)
	{
		
	}
}