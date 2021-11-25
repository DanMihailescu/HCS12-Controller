; SYSC-2003 Winter 2007 Supplied code.
; 

TRUE	EQU 1
FALSE	EQU 0

	
; void printHex (char hexNumber)
;   D (in B) is hexNumber, a number from 0..15
;
printHex:
       andb  #$0f      ; mask off bits
       cmpb  #$09      ; compare to number
       bhi   above9    ; branch if a thru f
       addb  #$30      ; add standard offset
       bra hex
above9:
       addb  #$37      ; change a-f to ascii
hex:
       jsr   printChar
       rts
	   
REGBS  EQU $0000 
SC0SR1 EQU REGBS+$CC
SC0DRL EQU REGBS+$CF

; void printChar( char character)
;    D (in B) is character.
printChar:
 	pshd
here:   
    brCLR  SC0SR1,#$80,here
;	LDAB	SC0SR1	; read status
;	BITB	#$80  	; test Transmit Data Register Empty bit
;	BEQ	OUTSCI2	; loop if TDRE=1
;	ANDA	#$7F   	; mask parity
	STAB	SC0DRL	; send character
	puld
    rts
		 
; void printStr( char* string)
;    D contains address of null-terminated string.
printStr:
     pshd
     pshy
	 xgdy
nextCharInStr:
	 ldab 1,y+
	 cmpb #0x00
	 beq printStrDone
	 jsr printChar	
	bra nextCharInStr
printStrDone:
    puly
	puld
    rts


