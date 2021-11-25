; SYSC-2003 Winter 2007 Supplied code.
; 

TRUE	EQU 1
FALSE	EQU 0
offset db 0
val dw ''
working db 0

message db 'Hello'
message1 db '1234'
num db 123
	
; void printHex (char hexNumber) 
;   D (in B) is hexNumber, a number from 0..15
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

; Returns the numeric equivalent of the decimal string. 
; @param string A null-terminated string denoting a valid positive decimal number, eg '1234' = 1234d 
; @return An unsigned int containing the decimal value equivalent to string 
convertToDecimal:
	pshy

	ldaa 0
	std val

continue:
	ldab #(val + offset)
	inc offset
	cba
	blt done		; tests for number below 0         
	ldaa 9
	cba
	bhi done		; tests for number above 9   
	ldaa '\0'
	cba
	beq done		; tests for end point   
	subb $30		; converts to number
	stab working
	ldd 10
	emul			; shifts digit over for next number
	addd working
	xgdy
	bra continue	; goes back to start
done: 
	ldaa 0
	staa offset
	xgdy			; puts final value into D
	puly
	rts

; Returns the decimal string equivalent of num, with leading zeros in  
; the hundreds and tens columns (so that the returned string is always three digits long)   
; @param num	An unsigned byte number (maximum value = 255)  
; @param string A pointer to a null-terminated string of at least 4 bytes (3 digits+1terminator)  
;		 in which the subroutine will store the string equivalent of num 
convertToString:
	pshd
	pshx
c: 
	cpd $0000
	ble d
	ldx 10
	idiv
	addd $0030
	xgdx
	stx val + offset
	inc offset
	bra d

d:
	pulx
	puld
	rts 
	
; Extracts and returns the row and column information from the code, returning true  
; if the code contains valid row/column values. Return false (-1) if not.   

; boolean getRowColumn( unsigned byte code, &unsigned byte row, & unsigned byte column)  
; @param  code is a byte divided up into two nibbles,   
;		the high nibble being the column identifier   
;		the low nibble being the row identifier   
;     Both identifiers are not numbers but are instead bit masks with a one in the bit  
;     position corresponding to the row/column, and all other bit positions zero.  
;           eg. A nibble 0001 has a one in bit position 0 and therefore identifies row/column 0  
;	      eg. A nibble 1000 has a one in bit position 3 and therefore identifies row/column 3  
; @param row  A pointer to an unsigned byte in which the subroutine will return the row, 0..3  
; @param column A pointer to an unsigned byte in which the subroutine will return the column, 0..3  
; @return  0 if the code had two valid identifiers (one 1 and rest zero).  
;	    -1 if either row/column identifier had an invalid format (eg. more than one 1 or no 1's at all)  
getRowColumn:
	stab working
	andb  #$0f      ; mask off bits  
      cmpb  #$03
	bhi notTrue

	ldab working 
	cmpb $33
	bhi notTrue 
	ldaa TRUE
	psha
	rts

notTrue:
	ldaa FALSE
	psha 
	rts

; Returns the ASCII character corresponding to the [row][column] location on the keypad 
; See the HC12 boards in the lab for the layout of the keypad.  
; @param row   An unsigned byte from 0 to 3, where 0 is the topmost row.  
; @param column An unsigned byte from 0 to 3, where 0 is the leftmost column.  
; @return ASCII character '1' to '9' or 'A' to 'F'.  
getChar:  
	ldaa $00		;Checks if key pressed was zero
	cba
	beq one
	ldaa $01		;Checks if key pressed was one
	cba
	beq two
	ldaa $02		;Checks if key pressed was two
	cba
	beq three
	ldaa $03		;Checks if key pressed was three
	cba
	beq Aa
	ldaa $10		;Checks if key pressed was four
	cba
	beq four
	ldaa $11		;Checks if key pressed was five
	cba
	beq five
	ldaa $12		;Checks if key pressed was six
	cba
	beq six
	ldaa $13		;Checks if key pressed was seven
	cba
	beq Ba
	ldaa $20		;Checks if key pressed was eigth
	cba
	beq seven
	ldaa $21		;Checks if key pressed was nine
	cba
	beq eight
	ldaa $22		;Checks if key pressed was A
	cba
	beq nine
	ldaa $23		;Checks if key pressed was B
	cba
	beq Ca
	ldaa $30		;Checks if key pressed was C
	cba
	beq Ea
	ldaa $31		;Checks if key pressed was D
	cba
	beq zero
	ldaa $32		;Checks if key pressed was E
	cba
	beq Fa
	ldaa $33		;Checks if key pressed was F
	cba
	beq Da
zero: 
	ldaa $30
	rts
one: 
	ldaa $31
	rts
two:
	ldaa $32
	rts
three: 
	ldaa $33
	rts
four: 
	ldaa $34
	rts
five: 
	ldaa $35
	rts
six: 
	ldaa $36
	rts
seven: 
	ldaa $37
	rts
eight: 
	ldaa $38
	rts
nine: 
	ldaa $39
	rts
Aa: 
	ldaa $41
	rts
Ba: 
	ldaa $42
	rts
Ca: 
	ldaa $43
	rts
Da: 
	ldaa $44
	rts
Ea: 
	ldaa $45
	rts
Fa: 
	ldaa $46
	rts

	org $4000
main: 
	ldd 10
	call printHex

	ldd 'A'
	call printChar

	ldd message
	call printStr

	ldd message1
	call convertToDecimal	;Stored value in D

	ldd num
	call convertToString	;Stores value in the stack

	ldab $21
	call getRowColumn
	pulx				;Stored boolean value in X	

	ldab $21
	call getChar		;Stored value in A
	