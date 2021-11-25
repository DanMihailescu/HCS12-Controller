#include "hcs12dp256.h"
#include "stdio.h"
 
int i;
int j;
void delay(int n){
	int x;
	while(n > 0){
	  for(j=0; j<0x1; j++){
		for(x=0; x<0x7FF; x++){
			asm("pshx");
			asm("pulx");
			asm("pshx");
			asm("pulx");
		}
	  }
	  n--;
	}
}

void main(void){
	DDRT |= 0x60;
	DDRP |= 0x20;
	PTP  |= 0x20;	
	PTT = 0x60;
	
	/*       CW
	*      -------
	*     |PT5|PT6|
	*     | 1 | 1 |
	*     | 1 | 0 |
	*     | 0 | 0 |
	*     | 0 | 1 |
	*      -------
	*/
	for(i = 0; i < 20; i++)
	{ 
		PTT = 0x60; 
		delay(10);
		PTT = 0x40; 
		delay(10);
		PTT = 0x00; 
		delay(10);
		PTT = 0x20; 
		delay(10);
	}
	
	/*       CCW
	*      -------
	*     |PT5|PT6|
	*     | 0 | 1 |
	*     | 0 | 0 |
	*     | 1 | 0 |
	*     | 1 | 1 |
	*      -------
	*/
	for(i = 0; i < 20; i++)
	{ 
		PTT = 0x20; 
		delay(10);
		PTT = 0x00; 
		delay(10);
		PTT = 0x40; 
		delay(10);
		PTT = 0x60; 
		delay(10);
	}
}