;*******************************************************************************
; Filename:	PrtTab.asm
; Description:	This runtine is involved by lsACPI.asm
;		Print RSDP, RSDT ... tables.
;*******************************************************************************

assume	cs:code

code	segment

;*******************************************************************************
; Procedure:	PrintRSDP
;
; Description:	Print all the RSDP info.
;
; Input:	BX - Off of RSDP
;		AX - Seg
;
; Output:	None.
;
; Change:	DX, ES, DI.
;
;*******************************************************************************
PrintRSDP	proc	near
	push	dx
	push	es
	push	di
	push	si

	;===================
	; Print these lines:
	;===================
	; Line1: -------------------------------------------------------------------------------
	; Line2:   Seg:Off  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  |  0123456789ABCDEF
	; Line3: -------------------------------------------------------------------------------
	mov	dx, offset ds:Memory_In16bit_Block_Line1
	call	PrintString
	mov	dx, offset ds:Memory_In16bit_Block_Line2
	call	PrintString
	mov	dx, offset ds:Memory_In16bit_Block_Line3
	call	PrintString

	mov	es, ds:FIND_IN_SEG
	mov	bx, ds:RSDP_STRUCT_START_OFF

	;===============================
	; Print memory block content
	;===============================
	; Line1: F000:B030 52 53 44 20 50 54 52 20 7D 41 43 50 49 41 4D 00  |  RSD.PTR.}ACPIAM.
	; Line1: F000:B040 00 00 7A 3F 24 00 00 00 00 01 7A 3F 00 00 00 00  |  ..z?$.....z?....
	; Line1: F000:B050 22 00 00 00 08                                   |  "....
	.repeat	; Print 1 line per times.
		;=======================
		; Print " F000:B030 "
		mov	dl, ' '
		call	PrintDLAscii
		mov	dx, ds:FIND_IN_SEG
		call	PrintDXContent
		mov	dl, ':'
		call	PrintDLAscii
		mov	dx, bx
		call	PrintDXContent
		mov	dl, ' '
		call	PrintDLAscii

		xor	di, di				; 00h - 0Fh
		;=======================
		; Print ASCII code.
		.repeat
			mov	si, bx
			add	si, di
			sub	si, ds:RSDP_STRUCT_START_OFF
			.if	si<RSDP_STRUCT_LENGTH
				mov	dl, es:[bx+di]
				call	PrintDLContent
			.else
				mov	dx, '  '	; Dont print contents more than 24Bytes.
				call	PrintDXAscii
			.endif
			mov	dl, ' '
			call	PrintDLAscii
			inc	di
		.until	di==10h

		mov	edx, ' |  '
		call	PrintEDXAscii

		xor	di, di				; 00h - 0Fh
		;=======================
		; Print Char.
		.repeat
			mov	si, bx
			add	si, di
			sub	si, RSDP_STRUCT_START_OFF
			.if	si<RSDP_STRUCT_LENGTH
				mov	dl, es:[bx+di]
				.if	dl>20h	; Dont print Cntl char.
					call	PrintDLAscii
				.else
					mov	dl, '.'
					call	PrintDLAscii
				.endif
			.else
				mov	dx, ' '
				call	PrintDLAscii
			.endif
			inc	di
		.until	di==10h

		add	bx, 10h			; Print next 10Byte.
		call	PrintEnter
	.until	bx>RSDP_STRUCT_END_OFF

	call	PrintEnter

	;===============================
	; Print RSDP detail infor
	;===============================
	; ES = 0F000h

	mov	dx, offset RSDP_String_Title
	call	PrintString

	; "Root System Description Pointer:"
	mov	dx, offset RSDP_String_Pointer
	call	PrintString
	mov	dx, FIND_IN_SEG
	call	PrintDXContent
	mov	dl, ':'
	call	PrintDLAscii
	mov	dx, RSDP_STRUCT_START_OFF
	call	PrintDXContent
	call	PrintEnter
	mov	dx, offset RSDP_String_Line
	call	PrintString

	;/* Print RSDP Signature. */
	mov	dx, offset RSDP_String_Signature
	call	PrintString
	mov	di, RSDP_STRUCT_START_OFF	; Input - RSDP_STRUCT_START_OFF+0
	mov	cx, 8				; Input - Length 8Bytes
	call	PrintStringIn16bitMEM
	call	PrintEnter	

	;/* Print RSDP Checksum. */
	;/* The offset&length of Checksum is define in ACPI Spec */
	mov	dx, offset RSDP_String_Checksum
	call	PrintString
	mov	di, RSDP_STRUCT_START_OFF
	mov	dl, es:[di+8]
	call	PrintDLContent
	call	PrintEnter

	;/* Print RSDP OEM ID. */
	mov	dx, offset RSDP_String_OEM_ID
	call	PrintString
	mov	di, RSDP_STRUCT_START_OFF
	add	di, 9				; Input - RSDP_STRUCT_START_OFF+9
	mov	cx, 6				; Input - Length 6Bytes
	call	PrintStringIn16bitMEM
	call	PrintEnter

	;/* Print RSDP Revision. */
	mov	dx, offset RSDP_String_Revision1
	call	PrintString
	mov	di, RSDP_STRUCT_START_OFF
	mov	dl, es:[di+15]
	call	PrintDLContent
	push	dx
	mov	dx, offset RSDP_String_Revision2
	call	PrintString
	pop	dx
	add	dl, 31h
	call	PrintDLAscii
	mov	dx, offset RSDP_String_Revision3
	call	PrintString
	call	PrintEnter

	;/* Print RSDP RSDT Address. */
	mov	dx, offset RSDP_String_RSDT_Address
	call	PrintString
	mov	di, RSDP_STRUCT_START_OFF
	add	di, 16d				; Input - RSDP_STRUCT_START_OFF+16d
	mov	cx, 4				; Input - Length 4Bytes
	call	PrintContentIn16bitMEMBigEnding
	call	PrintEnter

	;/* Print RSDP Length. */
	mov	dx, offset RSDP_String_RSDP_Length
	call	PrintString
	mov	di, RSDP_STRUCT_START_OFF
	add	di, 20d				; Input - RSDP_STRUCT_START_OFF+16d
	mov	cx, 4				; Input - Length 4Bytes
	call	PrintContentIn16bitMEMBigEnding
	call	PrintEnter

	;/* Print RSDP RSDT Address. */
	mov	dx, offset RSDP_String_XSDT_Address
	call	PrintString
	mov	di, RSDP_STRUCT_START_OFF
	add	di, 24d				; Input - RSDP_STRUCT_START_OFF+24d
	mov	cx, 8				; Input - Length 8Bytes
	call	PrintContentIn16bitMEMBigEnding
	call	PrintEnter

	;/* Print RSDP Extended Checksum. */
	mov	dx, offset RSDP_String_Extended_Checksum
	call	PrintString
	mov	di, RSDP_STRUCT_START_OFF
	mov	dl, es:[di+32d]
	call	PrintDLContent
	call	PrintEnter

	;/* Print RSDP Reserved BYTEs. */
	mov	dx, offset RSDP_String_Reserved_BYTEs
	call	PrintString
	mov	di, RSDP_STRUCT_START_OFF
	add	di, 33d				; Input - Print Off
	mov	cx, 3				; Input - Print Length
	call	PrintContentIn16bitMEMBigEnding
	call	PrintEnter

	pop	si
	pop	di
	pop	es
	pop	dx
	ret
PrintRSDP	endp

code	ends