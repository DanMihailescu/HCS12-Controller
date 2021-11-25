/****************************************************************************
* SYSC 2003 -- Winter 2014
* Lab 8: Template for basic interrupt-driven temperature (A/D) program.
* MUST USE WITH APPROPRIATE VECTOR FILE
*
*****************************************************************************/

#include "hcs12dp256.h"
#include <stdio.h>
#include "basicLCD.h"

#define TRUE       1
#define FALSE      0

/****************************************************************************
* Variable declarations.
*****************************************************************************/
// Shared GLOBAL variables between main program and ISR.
int currentTemp;
int i;
char final[2] = {'0', '0'};
char string1[11] = {'T', 'e', 'm', 'p', 'e', 'a', 't', 'u', 'r', 'e',':'};
volatile unsigned char tempReady;	// Flag set by ISR to TRUE when a NEW temperature has been converted
					// 	reset by main to FALSE after grabbing the new temperature
					//	(so main knows NOT to grab this same value again,
					//	but to wait until another new temperature is written)

/****************************************************************************
* Function prototypes.
*****************************************************************************/
void init(void);
void adcISR(void);

/****************************************************************************
* Main.
*****************************************************************************/
void main(void) {

    int temp, previousTemp;		// Local state variables

    // System initialization.
    init();
    previousTemp = 0;			// Arbitrary initial values
	Lcd2PP_Init();
	for (i = 0; i<=11; i++)        //Displays "Temperature:"
	{
	 	LCD_display(string1[i]);
	}
    // Loop forever:  In each loop, 
    //     Wait until a NEW temperature value is available.  
    //     Print the temperature either to the LCD or the host's console (your choice).
    //     ONLY AFTER YOU GET A VALID TEMPERATURE READING (i.e. room temperature = 70-80)
    //     extend your loop to also control the heater:  Turn on the heater
    //	   IF and ONLY IF THE TEMPERATURE IS LESS THAN 90.  
    //     PLEASE DO NOT BURN OUT THE HEATER -- DO NOT LEAVE IT ON PERMANENTLY.
    //     Heater???  It is controlled by Port M Bit 7. 1 is on. 0 is off.
    //		
    while (TRUE) {
        if ((tempReady) && (previousTemp != currentTemp)){
		   final[0] = currentTemp / 10 + '0';
	 	   final[1] = currentTemp % 10 + '0';
		   LCD_instruction(0x8B);
		   LCD_display(final[0]);
		   LCD_display(final[1]);
		   previousTemp = currentTemp;
		   tempReady = FALSE;
		}
		if (currentTemp >= 90){
		   PTM = 0x00;
		}
		else if (currentTemp < 90){
		   PTM = 0x80;
		}
    }
}


/****************************************************************************
* Initialization routine.
*****************************************************************************/
void init(void) {

    ATD0CTL2 = 0xFA;         // Enable ADC and interrupt.
    ATD0CTL3 = 0x00;         // Conversion sequence of 8.
    ATD0CTL4 = 0x60;         // 10-bit resolution, Right justified, unsigned.
    ATD0CTL5 = 0x86;	     // Single conversion, AN6 only
    
    DDRM |= 0x80;            // Port M Bit 7 controls the heater.
			     //    Set bit 7 for output.

    // Initialize variables shared between main program and ISR.
    currentTemp = 0;
    tempReady = FALSE;    

    asm("CLI");
}

/****************************************************************************
* ADC Interrupt Service Routine.
*****************************************************************************/
#pragma interrupt_handler adcISR()
void adcISR(void) {

    int digitalValue;
	
    digitalValue = ATD0DR6 & 0x03FF;
    currentTemp = (digitalValue / 8) - 5;

    // Set flag to indicate that a NEW temperature value is ready.
    tempReady = TRUE;

    // Start new conversion.
    ATD0CTL5 = 0x86;		 // Single conversion, AN6 only

}