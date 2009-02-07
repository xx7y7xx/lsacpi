;*******************************************************************************
; Filename:	ACPIi.ASM
; Description:	This runtine is involved by ACPI.C
;		List ACPI Tables.
;*******************************************************************************
assume	cs:code

; C file will invoke this function
;***************************************
;public	_SearchStringInSegment
public	_SearchCharInMEM
;public	_PrintStringInMEM
public	_GetBYTEContentInMEM
;public	_PrintBYTEAsciiInMEM
;public	_PrintWORDContentInMEM
;public	_PrintDWORDContentInMEM


code	segment
.386


;*******************************************************************************
; Procedure:	_SearchCharInMEM
;
; Description:	Search the char given by user from 0000 to FFFF in a Seg in MEM.
;		Output the offset of the char.
;
; Input:
;
; Output:
;
; Change:
;
;*******************************************************************************
_SearchCharInMEM	proc near FindChar:byte, FindInSeg:word, FindInOff:word, FindLength:word
	push	bp
	mov	bp, sp

	push	cx
	push	dx
	push	es
	push	di

	mov	al, FindChar		; Find char saved in AL
	mov	es, FindInSeg
	mov	di, FindInOff		; Find this char in ES:DI
	mov	cx, FindLength		; Find from ES:[DI] to ES:[DI+CX]
	mov	dx, cx
	repnz	scasb
	mov	ax, di

	jz	scim_found_char
	mov	ax, 0ffffh		; If not found, return 'ffff'.

scim_found_char:
	pop	di
	pop	es
	pop	dx
	pop	cx

	pop	bp

;	Return	ax
	ret
_SearchCharInMEM	endp



























;*******************************************************************************
; Procedure:	_SearchStringInSegment
;
; Description:	Search the string given by user from 0000 to FFFF in a Segment. 
;		Output the offset of the string.
;
; Input:	BX -	The string is saved at this Off in MEM.
;		DX -	No of char in string.
;		ES -	Find string in this Seg.
;
; Output:	AX -	The offset of the string in this Seg.
;
; Change:	AX, CX, DX, DI, SI.
;
;*******************************************************************************
_SearchStringInSegment	proc near StringInMEMOff:word, StringCharNo:word, FindInSeg:word
	push	bp
	mov	bp, sp

	push	cx
	push	dx
	push	di
	push	si

	mov	bx, StringInMEMOff
	mov	dx, StringCharNo
	mov	es, FindInSeg

	sub	dx, 1			; No of char in string, sub 1 to ptr
					; last char.
	mov	al, [bx+0]		; Put the first char of the stirng in AL
	mov	cx, 0ffffh		; repeat from 0000~ffff
	xor	di, di			; Ptr to Off '0' of this Seg.

	cld
ssis_find_first_char:
	repnz	scasb			; Find first char of the stirng (in AL).
					; DI ptr to next BYTE after first char 
					; found in this Seg,
; Found first char of the string in MEM.
	xor	si, si			; SI ptr to first char in string.
ssis_find_next_char:
	inc	di			; DI ptr to next BYTE in MEM
	inc	si			; SI ptr to next char in string.
	mov	ah, es:[di]
	cmp	ah, [bx+si]
	jne	ssis_find_first_char	; If next char is not fit, then JMP.

	cmp	si, dx			; If SI ptr to last char in the string,
					; then JMP.
	je	found_all_chars
	jmp	ssis_find_next_char

found_all_chars:
; Now DI ptr to last char of the string.
; So we should sub DX, to ptr the first char.
	sub	di, dx
	mov	ax, di
;	return	di			; Return DI as ptr to the string.

	pop	si
	pop	di
	pop	dx
	pop	cx

	pop	bp

	ret
_SearchStringInSegment	endp

;*******************************************************************************
; Procedure:	_PrintStringInMEM
;
; Description:	Print a string in MEM, its SEg:Off is ES:DI, its length CX.
;
; Input:	ES:DI -	Seg:Off ptr to string in MEM
;		CX -	No of chars in string.
;
; Output:	None
;
; Change:	DX, DI
;
;*******************************************************************************
_PrintStringInMEM	proc near
	push	dx
	push	di

; CX - Loop times, to print every char in string.
psim_print_next_byte:
	mov	dl, es:[di]
	call	PrintDLAscii
	inc	di
	loop	psim_print_next_byte

	pop	di
	pop	dx

	ret
_PrintStringInMEM	endp

;*******************************************************************************
; Procedure:	_GetBYTEContentInMEM
;
; Description:	Get a BYTE's content in MEM, its SEg:Off is ES:DI.
;
; Input:	ES:DI -	Seg:Off ptr to BYTE in MEM
;
; Output:	AL -	Return this BYTE's content.
;
; Change:	DX
;
;*******************************************************************************
_GetBYTEContentInMEM	proc near FindInSeg:word, FindInOff:word
	push	bp
	mov	bp, sp

	push	es
	push	di

	mov	es, FindInSeg
	mov	di, FindInOff
	mov	al, es:[di]

	pop	di
	pop	es

	pop	bp
	ret
_GetBYTEContentInMEM	endp

;*******************************************************************************
; Procedure:	_PrintBYTEAsciiInMEM
;
; Description:	Print a BYTE's ASCII code in MEM, its SEg:Off is ES:DI.
;
; Input:	ES:DI -	Seg:Off ptr to BYTE in MEM
;
; Output:	None
;
; Change:	DX
;
;*******************************************************************************
_PrintBYTEAsciiInMEM	proc near pbaim_FindInSeg:word, pbaim_FindInOff:word
	push	bp
	mov	bp, sp

	push	dx

	mov	es, pbaim_FindInSeg
	mov	di, pbaim_FindInOff

	mov	dl, es:[di]
	call	PrintDLAscii

	mov	al, dl

	pop	dx

	pop	bp
	ret
_PrintBYTEAsciiInMEM	endp

;*******************************************************************************
; Procedure:	_PrintWORDContentInMEM
;
; Description:	Print a WORD in MEM, its SEg:Off is ES:DI.
;
; Input:	ES:DI -	Seg:Off ptr to WORD in MEM
;
; Output:	None
;
; Change:	DX
;
;*******************************************************************************
;_PrintWORDContentInMEM	proc near
;	push	di
;
;	call	_PrintBYTEContentInMEM
;	inc	di
;	call	_PrintBYTEContentInMEM
;
;	pop	di
;	ret
;_PrintWORDContentInMEM	endp

;*******************************************************************************
; Procedure:	_PrintDWORDContentInMEM
;
; Description:	Print a DWORD in MEM, its SEg:Off is ES:DI.
;
; Input:	ES:DI -	Seg:Off ptr to DWORD in MEM
;
; Output:	None
;
; Change:	DX
;
;*******************************************************************************
;_PrintDWORDContentInMEM	proc near
;	push	di
;
;	call	_PrintBYTEContentInMEM
;	inc	di
;	call	_PrintBYTEContentInMEM
;	inc	di
;	call	_PrintBYTEContentInMEM
;	inc	di
;	call	_PrintBYTEContentInMEM
;
;	pop	di
;	ret
;_PrintDWORDContentInMEM	endp












;-------------------------------------------------------------------------------
;File Name:	proc.inc
;Function List:
;
;		*** Output ***
;
;	PrintString:	Print string with its offset in DX
;	PrintDLAscii:	Print ASCII code in DL
;	PrintDXAscii:	Print ASCII code in DX
;	PrintEDXAscii:	Print ASCII code in EDX
;
;	PrintDLLowContent:	Print Register Low 4bits in DL
;	PrintDLContent:	Print Register content in DL
;	PrintDXContent:	Print Register content in DX
;	PrintEDXContent:	Print Register content in EDX
;
;	PrintBLByBit:	Print BL in binary
;	PrintEnter:	Print CR+LF
;
;		*** Input ***
;
;	Input2HexCharInBL:	Input 2 hex number in BL(change BL content)
;
;		*** Convert ***
;
;	HexInAL2Dec:Add20081112:	Convert AL content to Decimal, print
;	HexInAX2Dec:	Convert AX content to Decimal, print
;
;-------------------------------------------------------------------------------

; **************************************************************************** ;
; Procedure:	PrintDLAscii
; Description:	Print ASCII code in DL, if DL=41h, print "A".
; Input:	DL.
; Output:	None
; Change:	AH
; **************************************************************************** ;
PrintDLAscii	proc    near
	push	ax
	mov     ah, 2
	int     21h
	pop	ax
	ret
PrintDLAscii	endp

; **************************************************************************** ;
; Procedure:	PrintString
; Description:	Print string, its first address is in DX.
; Input:	DX, string's first address in DX
; Output:	None
; Example:	If define mes2='Hello.$' in data segment,
;		then "MOV DX,OFFSET mes2", last print "Hello." on screen
; Change:	AH
; **************************************************************************** ;
PrintString	proc    near
	push	ax
	mov     ah, 9
	int     21h
	pop	ax
	ret
PrintString	endp

; **************************************************************************** ;
; Procedure:	PrintDLContent
; Description:	Print content in DL
; Input:	DL
; Output:	None
; Example:	DL=0ABh, print "AB" on screen
; Change:	BL, CL, DL
; **************************************************************************** ;
PrintDLContent	proc	near
	push	bx
	push	cx
	push	dx

	mov	bl, dl			;DL change in RCR
	mov	cl, 4
	rcr	dl, cl
	call	PrintDLLowContent	;print high 4bits 'A' of 'AB'
	mov	dl, bl
	call	PrintDLLowContent	;print low 4bits 'B' of 'AB'

	pop	dx
	pop	cx
	pop	bx
	ret
PrintDLContent	endp

; **************************************************************************** ;
; Procedure:	PrintDLLowContent
; Description:	Print low content in DL, it only used by PROC: PrintDLContent
; Input:	DL
; Output:	None
; Example:	DL=0ABh, print "B"
; Change:	DL
; **************************************************************************** ;
PrintDLLowContent	proc	near
	push	dx
	and	dl, 0fh
	add	dl, 30h
	cmp	dl, '9'
	jbe	ok
	add	dl, 7
ok:
	call	PrintDLAscii
	pop	dx
	ret
PrintDLLowContent	endp

; **************************************************************************** ;
; Procedure:	PrintBLByBit
; Decription:	Print content in BL in binary mode.
;		Rotate BL to left bit by bit.
; Input:	BL
; Output:	None
; Example:	BL=0ABh, print"10101011"
; Change:	BL, CL, DL, 
; **************************************************************************** ;
PrintBLByBit	proc	near
	push	bx
	push	cx
	push	dx

	mov	cl, 8
loop_bit:
	sub	dl, dl
	rcl	bl, 1		;the first bit of BL is to C flag
	rcl	dl, 1
	add	dl, 30h
	call	PrintDLAscii
	loop	loop_bit

	pop	dx
	pop	cx
	pop	bx
	ret
PrintBLByBit	endp

; **************************************************************************** ;
; Procedure:	PrintEnter
; Description:	Print 'CR' & 'LF' just like you type 'Enter'
; Input:	None
; Output:	None
; Change:	DL
; **************************************************************************** ;
PrintEnter	proc    near
	push	dx
	mov	dl, 0dh			;0dh=LF
	call	PrintDLAscii
	mov	dl, 0ah			;0ah=CR
	call	PrintDLAscii
	pop	dx
	ret
PrintEnter	endp

; **************************************************************************** ;
; Procedure:	Input2HexCharInBL
; Description:	User input 2 hex number, saved in BL
; Input:	None
; Output:	BL
; Change:	AX,BX,CX,DX
; **************************************************************************** ;
Input2HexCharInBL	proc	near
	push	ax
	push	cx
	push	dx

;initial CX and BX
	sub	bx, bx			;the 2 chars inputed is saved in it	
	mov	cx, 2			;how many times "next" is loop

;'next' is a loop to input 2 chars
next:
	sub	ah, ah
	int	16h
	mov	dl, al
	cmp	al, '0'
	jb	next
	cmp	al, '9'
	ja	alpha
	sub	al, '0'
	jmp	short input_ok

;If input is "A~F"
alpha:
	and	al, 0dfh		;"a~f" to "A~F"
	mov	dl, al
	sub	al, 37h
	cmp	al, 0ah
	jb	next
	cmp	al, 0fh
	ja	next

;If input a char is over, and the char is ok
;the 2 chars is saved in BL
input_ok:
	push	ax
	mov	ah, 2
	int	21h		;display the char you input
	pop	ax
	push	cx
	mov	cl, 4
	shl	bl, cl
	pop	cx	
	add	bl, al
	loop	next

	pop	dx
	pop	cx
	pop	ax
	ret
Input2HexCharInBL	endp

; **************************************************************************** ;
; Procedure:	HexInAL2Dec
; Description:	Convert content(hex) in AL to Decimal number
; Input:	AL
; Output:	None
; Example:	AL=10h, print "16" on screen
; Change:	AL, BL, CL, DL
; **************************************************************************** ;
HexInAL2Dec	proc	near
	push	ax
	push	bx
	push	cx
	push	dx
	mov	cl, 100d
	sub	ah, ah
	div	cl
	mov	dl, al
	mov	bl, ah		;AH will change in 'print'
	add	dl, 30h
	call	PrintDLAscii

	mov	cl, 10d
	mov	al, bl
	sub	ah, ah
	div	cl
	mov	dl, al
	mov	bl, ah		;AH will change in 'print'
	add	dl, 30h
	call	PrintDLAscii
	mov	dl, bl
	add	dl, 30h
	call	PrintDLAscii
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	ret
HexInAL2Dec	endp

; **************************************************************************** ;
; Procedure:	PrintDXContent
; Description:	Print content in DX
; Input:	DX
; Output:	None
; Change:	DX
; Example:	DX=4142h, print "AB".
; **************************************************************************** ;
PrintDXContent	proc	near
	push	dx
	ror	dx, 8d
	call	PrintDLContent
	ror	dx, 8d
	call	PrintDLContent
	pop	dx
	ret
PrintDXContent	endp

; **************************************************************************** ;
; Procedure:	PrintEDXContent
; Description:	Print content in EDX
; Input:	EDX
; Output:	None
; Change:	EDX
; Example:	EDX=0ABCDh, print "ABCD" on screen
; **************************************************************************** ;
PrintEDXContent	proc	near
	push	edx
	ror	edx, 16d
	call	PrintDXContent
	ror	edx, 16d
	call	PrintDXContent
	pop	edx
	ret
PrintEDXContent	endp

; **************************************************************************** ;
; Procedure:	PrintDXAscii
; Description:	Print ASCII code in DX, if DX=4142h, print "AB".
; Input:	DX.
; Output:	None.
; Change:	DX.
; **************************************************************************** ;
PrintDXAscii	proc    near
	push	dx
	ror	dx, 8d
	call	PrintDLAscii
	ror	dx, 8d
	call	PrintDLAscii
	pop	dx
	ret
PrintDXAscii	endp

; **************************************************************************** ;
; Procedure:	PrintEDXAscii
; Description:	Print ASCII code in EDX, if EDX=41424344h, print "ABCD".
; Input:	EDX.
; Output:	None.
; Change:	EDX.
; **************************************************************************** ;
PrintEDXAscii	proc    near
	push	edx
	ror	edx, 16d
	call	PrintDXAscii
	ror	edx, 16d
	call	PrintDXAscii
	pop	edx
	ret
PrintEDXAscii	endp

; **************************************************************************** ;
; Procedure:	HexInAX2Dec
; Description:	Convert content(hex) in AX to Decimal number
; Input:	AX
; Output:	None
; Example:	AX=0010h, print "16" on screen
; Change:	AX, BX, CX, DX
; **************************************************************************** ;
HexInAX2Dec	proc	near
	push	ax
	push	bx
	push	cx
	push	dx

	mov	cx, 10000d		; (DX:AX)/CX=AX...DX
	sub	dx, dx
	div	cx
	mov	bx, dx			; save YUSHU to BX
	mov	dx, ax
	add	dx, 30h
	call	PrintDLAscii

	mov	cx, 1000d		; (DX:AX)/CX=AX...DX
	mov	ax, bx			; move YUSHU to BEICHUSHU
	sub	dx, dx
	div	cx
	mov	bx, dx			; save YUSHU to BX
	mov	dx, ax
	add	dx, 30h
	call	PrintDLAscii

	mov	cx, 100d		; (DX:AX)/CX=AX...DX
	mov	ax, bx
	sub	dx, dx
	div	cx
	mov	bx, dx			; save YUSHU to BX
	mov	dx, ax
	add	dx, 30h
	call	PrintDLAscii

	mov	cx, 10d			; (DX:AX)/CX=AX...DX
	mov	ax, bx
	sub	dx, dx
	div	cx
	mov	bx, dx			; save YUSHU to BX
	mov	dx, ax
	add	dx, 30h
	call	PrintDLAscii		; Print SHANG
	mov	dx, bx
	add	dx, 30h
	call	PrintDLAscii		; Print YUSHU

	pop	dx
	pop	cx
	pop	bx
	pop	ax
	ret
HexInAX2Dec	endp

;-------------------------------------------------------------------------------

code	ends
	end
