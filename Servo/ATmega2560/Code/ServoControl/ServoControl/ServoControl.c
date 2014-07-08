/*
 * ServoControl.c
 *
 * Created: 5/28/2014 4:13:06 PM
 * Author: Joel
 * Note: Do not let servos be excited for long durations to avoid heating up
 * 		 and to reduce current draw. Free servos when not using them
 */ 

typedef enum { false, true } bool;
#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void port_init()
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

void timer1_init()
{
	TCCR1A = 0x00;
	
	ICR1 = 1023; //TOP = 1023
	TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
	TCNT1L = 0x01; //Counter low value to which OCR1xH value is to be compared with
	OCR1A = 1023;
	OCR1B = 1023;
	OCR1C = 1023;
	TCCR1A = 0xAB;
	//COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0
	//For Overriding normal port functionality to OCRnA outputs. WGM11=1, WGM10=1. Along With
	//WGM12 in TCCR1B for Selecting FAST PWM Mode

	TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)
}

//Sets servo 1 to the specified angle in degrees
void servo_1(unsigned char degrees)
{
	float regval = ((float)degrees * 0.512) + 34.56;
	OCR1A = (uint16_t) regval;
}

//Sets servo 2 to the specified angle in degrees
void servo_2(unsigned char degrees)
{
	float regval = ((float)degrees * 0.512) + 34.56;
	OCR1B = (uint16_t) regval;
}

//Sets servo 3 to the specified angle in degrees
void servo_3(unsigned char degrees)
{
	float regval = ((float)degrees * 0.512) + 34.56;
	OCR1C = (uint16_t) regval;
}

//Frees (relaxes) servo 1 by sending a continuous on signal
void servo_1_free (void)
{
	OCR1A = 1023;
}

//Frees (relaxes) servo 2 by sending a continuous on signal
void servo_2_free (void)
{
	OCR1B = 1023;
}

//Frees (relaxes) servo 3 by sending a continuous on signal
void servo_3_free (void)
{
	OCR1C = 1023;
}

//Initialises devices
void init()
{
	cli();
	port_init();
	lcd_port_config();
	timer1_init();
	sei();
}

int main(void)
{
	init();
	servo_2(0);
	_delay_ms(1000);
	servo_2(90);
	_delay_ms(1000);
	servo_2(180);
	_delay_ms(1000);
	
	while(1);
	
	return 0;
}
