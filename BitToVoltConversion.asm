; Value1 + (G * ((Value2 - Value1)/(2^16)))
;
	org $800
v1 db -5		;Insert lowest value
v2 db 5			;Insert highest value
G dw $00ff		;Insert wanted 16-bit quantity
size dw $ffff		;Defines 16-bit value size
final dw 0		;Stores working and final value
;
	org $4000
	LDAA v2
	SUBA v1
	STAA final
;
	LDD final
	LDX size
	IDIV
	STX final		;X is the value while D is the remainder
;
	LDY final
	LDD G
	EMUL
	STD final		;Should all be in this register (Y:D)
;
	LDD v1
	ADDD final
	STD final
;
	END