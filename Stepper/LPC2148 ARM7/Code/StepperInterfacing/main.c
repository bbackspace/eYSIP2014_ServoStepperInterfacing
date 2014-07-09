/*
    main.c
		Stepper Motor Interfacing with ARM7 microcontroller LPC2148
		Author: Joel M. Pinto
*/

#include<lpc214x.h>
#include "stepper.h"

void DelaymSec(unsigned int j)		  //Generates 1mSec delay
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<10000; i++);
 } 
}

int direction = 1;
int count = 0;
int main(void)
{
	stepper_port_init(); //Initialize ports
	while(1)
	{
		full_step(direction);
		DelaymSec(10); //10 ms between steps equals 100Hz
		if(count++ >= 200) //Change direction every revolution
		{
			direction = -1 * direction;
			count = 0;
		}
	}
}
