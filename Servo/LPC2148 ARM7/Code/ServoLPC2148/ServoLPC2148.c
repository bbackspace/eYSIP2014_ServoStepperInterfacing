#include <lpc214x.h>

void DelaymSec(unsigned int j)
{
	unsigned i;
	for(; j > 0; j--)
	{
		for(i = 0; i < 10000; i++);
	}
}	

void initServoPWM()
{
	PINSEL0&=0xFFFFFFFC;
	PINSEL0|=0x00000002; //Enabling P0.0 as PWM1
	PWMPR = 150; //PWM Prescaler PCLK/150 = 100KHz
	PWMPC = 0; //PWMPC increments on every PCLK
	PWMTC = 0; //PWMTC increments on every PWMPC=PWMPR
	PWMMR0 = 2000; //PWM base frequency 100KHz/2000=50Hz
	PWMMR1 = 60;
	PWMMR2 = 0;
	PWMMR3 = 0;
	PWMMR4 = 0;
	PWMMR5 = 0;
	PWMMR6 = 0;
	PWMMCR = 0x00000002;
	PWMPCR = 0x2600;
	PWMLER = 0x7F;
	PWMTCR = 0x01;
}

void Servo_1(unsigned char degrees)
{
	float regval = 0;
	regval = ((float)degrees / 1.125) + 60.0;
	PWMMR1 = (unsigned int)regval;
	PWMLER = 0x02;
}

void Servo_1_Free(void)
{
	PWMMR1 = 1999;
	PWMLER = 0x02;
}

void init_devices(void)
{
	initServoPWM(); //Initialise PWM for servo
}

int main(void)
{
	//Set all pins as GPIO
	PINSEL0 = 0x00000000;
	PINSEL1 = 0x00000000;
	PINSEL2 = 0x00000000;
	//Initialise Peripherals
	init_devices();
	DelaymSec(2000);
	
	while(1)
		{
			Servo_1(90);
			DelaymSec(600);
			Servo_1(180);
			DelaymSec(600);
			Servo_1(90);
			DelaymSec(600);
			Servo_1(0);
			DelaymSec(600);
		}
}