#include "basicLCD.h"
#include "hcs12dp256.h"
#include <stdio.h>

int motorRotate = 1;
int engineSpeed = 0;
int temperature = 0;
int i, count, j;
int n = 25;
char c;
char final[2] = {'0', '0'};
char string1[6] = {'S', 'p', 'e', 'e', 'd', ':'};
char string2[11] = {'T', 'e', 'm', 'p', 'e', 'a', 't', 'u', 'r', 'e', ':'};

//<<<<< Auxillary Functions here >>>>>>>
//Converts int value to a array of chars
void toChar(int value)
{
	 final[1] = value / 10 + '0';
	 final[0] = value % 10 + '0';
}

//Delays for approximately 1 second
void delay(int h)
{
 	int x;
	int y;
	//Compares keypad register
	if((PTH & 0x80) == 0x80) {motorRotate = 0;}
	else{motorRotate = 1;}
	if((PTH & 0x10) == 0x10) n = 12;
	else if((PTH & 0x20) == 0x20) n = 25;
	else if((PTH & 0x40)  == 0x40) n = 50;
	
	while(h > 0)
	{
	  for(y=0; y<0x1; y++)
	  {
		for(x=0; x<0x7FF; x++)
		{
			asm("pshx");
			asm("pulx");
			asm("pshx");
			asm("pulx");
		}
	  }
		
	  h--;
	}
}


//Setup for the changing of motor speed
void speedSetup(void)
{
	DDRP = 0x2F;
	DDRH = 0xF0;
 	PTP = 0x21;
	PTH = 0x20;
}

//Setup for the motor
void motorSetup(void)
{
	SPI1CR1 = 0x00;
	DDRT |= 0x60;
	PTT = 0x60;
}
// <<<<< End of Auxillary Functions >>>>>

// <<<<< Start of Critical Functions >>>>>
void check(int k)
{
	 j += k;
	 if (j >= 100)
	 {
	 	j = 0;
		temperature++;
	 }
}

//Spins motor CW
void spinMotor(void)
{
 	 	PTT = 0x60; 
		check(n);
		delay(n); 
	 	PTT = 0x40; 
		check(n);
	 	delay(n);
	 	PTT = 0x00; 
		check(n);
	 	delay(n);
		PTT = 0x20; 
		check(n);
	 	delay(n);
		count++;
}

//Spins motor CCW
void reverseMotor(void)
{
 	 	PTT = 0x20; 
		check(n);
		delay(n); 
	 	PTT = 0x00; 
		check(n);
	 	delay(n);
	 	PTT = 0x40; 
		check(n);
	 	delay(n);
		PTT = 0x60; 
		check(n);
	 	delay(n);
		count++;
}

//Increments both the temperature and engine speed periodically
void increment(void)
{
	motorSetup();
	speedSetup();
	CRGINT |= 0x80;
	RTICTL |= 0x7f;
	while(1)
	{
		if(temperature >= 99)   //Resets temp once it hits the limit
		{
		 	temperature = 0;
		}
		if (engineSpeed >= 99)  //Resets engine speed once it hits the limit
		{
			engineSpeed = 0;
		}
	// <<<< Display for Eng.speed and temp. >>>>	
		//Does actions for the engine speed
		toChar(engineSpeed);        //Converts the engine speed to a array of chars
	 	LCD_instruction(0x87);
		c = final[1];				//Displays upper half of the int
		LCD_display(c);
		c = final[0];				//Displays lower half of the int
		LCD_display(c);        
		
		//Does actions for the temperature
		LCD_instruction(0xCD);
		toChar(temperature); 		//Converts the temperature to a array of chars
		c = final[1];				//Displays upper half of the int
		LCD_display(c);
		c = final[0];				//Displays lower half of the int
		LCD_display(c);
	// <<<<<>>>>>
	
	// <<<< Motor control >>>>
		if(motorRotate == 0) 
		{
			reverseMotor();	
		}
		else
		{ 
			spinMotor();
		}
		if (count == 6){
			count = 0;
			if(motorRotate == 0) 
			{
				reverseMotor();	
			}
			else
			{ 
				spinMotor();
			} 
			engineSpeed++;	 	   //Increases current engine speed
		}
	// <<<<<>>>>>
	}
// <<<< End of while loop (never exits) >>>>
}

//Initializes the board with the labels
void initialize(void)
{
	//Top LCD
	Lcd2PP_Init();                //Initializes LCD
	for (i = 0; i<6; i++)         //Displays "Speed:"
	{        
		LCD_display(string1[i]);
	}
	 
	//Bottom LCD
	LCD_instruction(0xC0);        //Sets display pointer to bottom row
	for (i = 0; i<11; i++)        //Displays "Temperature:"
	{
	 	LCD_display(string2[i]);
	}
}

//Main call order
void main(void)
{ 
	 initialize();    //Initializes
	 increment();	  //Increments
}
