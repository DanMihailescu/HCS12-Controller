/****************************************************
Assignment 5
		   	 			Name        Student Number					 	
 	 	Student 1: Dan Mihailescu     100972565
		Student 2: Ben Palko		  100964652
****************************************************/

/****************************************************
Notes: --> Keypad detection doesnt seem to work(should based on notes), but set
	   up on what would happen if it did get detected is done.
*****************************************************/
	   

#include "basicLCD.h"
#include "hcs12dp256.h"
#include <stdio.h>

#define TRUE       1
#define FALSE      0

int motorRotate = 1;  //Direction
int engineSpeed = 0;  //Engine speed
int temperature = 0;  //Temperature
int i, count, j;
int n = 25;
int previousTemp = 0;
char c;
char final[2] = {'0', '0'};
char string1[6] = {'S', 'p', 'e', 'e', 'd', ':'};
char string2[11] = {'T', 'e', 'm', 'p', 'e', 'a', 't', 'u', 'r', 'e', ':'};
volatile unsigned char tempReady;

//<<<<< Auxillary Functions here >>>>>>>
//Converts int value to a array of chars
void toChar(int value)
{
	 final[1] = value / 10 + '0';
	 final[0] = value % 10 + '0';
}

//Delays for approximately 1 second and checks for which key is pressed
void delay(int h)
{
 	int x;
	int y;
	//Checks if key is pressed each time delay is called(frequent)
	if((PTH & 0x80) == 0x80) {
		if(motorRotate == 1) {motorRotate = 0;}  //Rotates CCW
		else{motorRotate = 1;}	 			  	 //Rotates CW
	}
	if((PTH & 0x10) == 0x10) n = 50;			 //Sets speed to half
	else if((PTH & 0x20) == 0x20) n = 25;		 //Sets speed to normal
	else if((PTH & 0x40)  == 0x40) n = 12;		 //Sets speed to double
	PTH &= 0x00;   		  	 	   	   			 //Clears Port H
	
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

void tempSetup(void)
{
	ATD0CTL2 = 0xFA;         // Enable ADC and interrupt.
    ATD0CTL3 = 0x00;         // Conversion sequence of 8.
    ATD0CTL4 = 0x60;         // 10-bit resolution, Right justified, unsigned.
    ATD0CTL5 = 0x86;	     // Single conversion, AN6 only
    
    DDRM |= 0x80;            // Port M Bit 7 controls the heater.
			    			 //    Set bit 7 for output.

    tempReady = FALSE;    

    asm("CLI");
}
// <<<<< End of Auxillary Functions >>>>>

// <<<<< Start of Critical Functions >>>>>
void check(int k)
{
	 j += k;
	 if (j >= 100)		   //Checks if 1 second has passed (100 = 1 second)
	 {
	 	j = 0;
		temperature++;	   //Increases the temperature count
	 }
}

//Spins motor CW
void spinMotor(void)
{
 	 	PTT = 0x60; 
		check(n);		   //Checks if a second has passes after the latest delay
		delay(n); 
	 	PTT = 0x40; 
		check(n);		   //Checks if a second has passes after the latest delay
	 	delay(n);
	 	PTT = 0x00; 
		check(n);		   //Checks if a second has passes after the latest delay
	 	delay(n);
		PTT = 0x20; 
		check(n);		   //Checks if a second has passes after the latest delay
	 	delay(n);
		count++;
}

//Spins motor CCW
void reverseMotor(void)
{
 	 	PTT = 0x20; 
		check(n);		   //Checks if a second has passes after the latest delay
		delay(n); 
	 	PTT = 0x00; 
		check(n);		   //Checks if a second has passes after the latest delay
	 	delay(n);
	 	PTT = 0x40; 
		check(n);		   //Checks if a second has passes after the latest delay
	 	delay(n);
		PTT = 0x60; 
		check(n);		   //Checks if a second has passes after the latest delay
	 	delay(n);
		count++;
}

//Increments both the temperature and engine speed periodically
void increment(void)
{
	motorSetup();  //Sets up the motor
	speedSetup();  //Sets up the speed
	tempSetup();   //Sets up the temp
	
	while(1)
	{
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
		if ((tempReady) && (previousTemp != currentTemp)){
			toChar(temperature); 		//Converts the temperature to a array of chars
			LCD_display(final[0]);		//Displays upper half of the int
			LCD_display(final[1]);		//Displays lower half of the int
			previousTemp = currentTemp;
			tempReady = FALSE;
		}
		if (currentTemp >= 90){			//Keeps temp at 90 degrees
		   PTM = 0x00;
		}
		else if (currentTemp < 90){
		   PTM = 0x80;
		}
	// <<<<<>>>>>
	
	// <<<< Motor control >>>>
		if(motorRotate == 0)  //Runs motor in reverse(CCW)
		{
			reverseMotor();	
		}
		else				  //Runs motor in normal direction(CW)
		{ 
			spinMotor();
		}
		if (count == 6){
			count = 0;
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

//ADC Interrupt Service Routine.
#pragma interrupt_handler adcISR()
void adcISR(void) {

    int digitalValue;
	
    digitalValue = ATD0DR6 & 0x03FF;
    temperature = (digitalValue / 8) - 5;

    // Set flag to indicate that a NEW temperature value is ready.
    tempReady = TRUE;

    // Start new conversion.
    ATD0CTL5 = 0x86;		 // Single conversion, AN6 only

}