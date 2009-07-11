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
			.if	si<ds:RSDP_STRUCT_LENGTH
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
			sub	si, ds:RSDP_STRUCT_START_OFF
			.if	si<ds:RSDP_STRUCT_LENGTH
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
	.until	bx>ds:RSDP_STRUCT_END_OFF

	call	PrintEnter

	;===============================
	; Print RSDP detail infor
	;===============================
	mov	dx, offset ds:RSDP_String_Title
	call	PrintString

	; "Root System Description Pointer:"
	mov	dx, offset ds:RSDP_String_Pointer
	call	PrintString
	mov	dx, ds:FIND_IN_SEG
	call	PrintDXContent
	mov	dl, ':'
	call	PrintDLAscii
	mov	dx, ds:RSDP_STRUCT_START_OFF
	call	PrintDXContent
	call	PrintEnter
	mov	dx, offset ds:Table_String_Line
	call	PrintString

	;/* Print RSDP Signature. */
	mov	dx, offset ds:RSDP_String_Signature
	call	PrintString
	mov	es, ds:FIND_IN_SEG		; ES - Segment of RSDP table
	mov	di, ds:RSDP_STRUCT_START_OFF	; DI - RSDP_STRUCT_START_OFF+0
	mov	cx, 8				; CX - Length 8Bytes
	call	PrintStringIn16bitMEM
	call	PrintEnter	

	;/* Print RSDP Checksum. */
	;/* The offset&length of Checksum is define in ACPI Spec */
	mov	dx, offset ds:RSDP_String_Checksum
	call	PrintString
	mov	di, ds:RSDP_STRUCT_START_OFF
	mov	dl, es:[di+8]
	call	PrintDLContent
	call	PrintEnter

	;/* Print RSDP OEM ID. */
	mov	dx, offset ds:RSDP_String_OEM_ID
	call	PrintString
	mov	es, ds:FIND_IN_SEG		; ES - Segment of RSDP table
	mov	di, ds:RSDP_STRUCT_START_OFF
	add	di, 9				; DI - RSDP_STRUCT_START_OFF+9
	mov	cx, 6				; CX - Length 6Bytes
	call	PrintStringIn16bitMEM
	call	PrintEnter

	;/* Print RSDP Revision. */
	mov	dx, offset ds:RSDP_String_Revision1
	call	PrintString
	mov	di, ds:RSDP_STRUCT_START_OFF
	mov	dl, es:[di+15]
	call	PrintDLContent
	push	dx
	mov	dx, offset ds:RSDP_String_Revision2
	call	PrintString
	pop	dx
	add	dl, 31h
	call	PrintDLAscii
	mov	dx, offset ds:RSDP_String_Revision3
	call	PrintString
	call	PrintEnter

	;/* Print RSDP RSDT Address. */
	mov	dx, offset ds:RSDP_String_RSDT_Address
	call	PrintString
	mov	di, ds:RSDP_STRUCT_START_OFF
	add	di, 16d				; Input - RSDP_STRUCT_START_OFF+16d
	mov	cx, 4				; Input - Length 4Bytes
	call	PrintContentIn16bitMEMBigEnding
	call	PrintEnter

	;/* Print RSDP Length. */
	mov	dx, offset ds:RSDP_String_RSDP_Length
	call	PrintString
	mov	di, ds:RSDP_STRUCT_START_OFF
	add	di, 20d				; Input - RSDP_STRUCT_START_OFF+16d
	mov	cx, 4				; Input - Length 4Bytes
	call	PrintContentIn16bitMEMBigEnding
	call	PrintEnter

	;/* Print RSDP RSDT Address. */
	mov	dx, offset ds:RSDP_String_XSDT_Address
	call	PrintString
	mov	di, ds:RSDP_STRUCT_START_OFF
	add	di, 24d				; Input - RSDP_STRUCT_START_OFF+24d
	mov	cx, 8				; Input - Length 8Bytes
	call	PrintContentIn16bitMEMBigEnding
	call	PrintEnter

	;/* Print RSDP Extended Checksum. */
	mov	dx, offset ds:RSDP_String_Extended_Checksum
	call	PrintString
	mov	di, ds:RSDP_STRUCT_START_OFF
	mov	dl, es:[di+32d]
	call	PrintDLContent
	call	PrintEnter

	;/* Print RSDP Reserved BYTEs. */
	mov	dx, offset ds:RSDP_String_Reserved_BYTEs
	call	PrintString
	mov	di, ds:RSDP_STRUCT_START_OFF
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

;*******************************************************************************
; Procedure:	PrintRSDT
;
; Description:	Print all the RSDT info.
;
; Input:	None.
;
; Output:	None.
;
; Change:	.
;
;*******************************************************************************
PrintRSDT	proc	near
	pushad
	push	es

	;===============================
	; Get RSDT Start Address.
	;===============================
	mov	es, ds:FIND_IN_SEG
	mov	di, ds:RSDP_STRUCT_START_OFF
	add	di, 16d			; Off of RSDT address.
	mov	edi, dword ptr es:[di]
	mov	ds:RSDT_STRUCT_START_OFF, edi

	;===============================
	; Access memory for 32bit (4GB RAM).
	;===============================
	call	JumpToBigRealMode

	;===============================
	; Get RSDT length & End Address.
	;===============================
	mov	edi, ds:RSDT_STRUCT_START_OFF
	mov	ecx, fs:[edi+4]			; length of RSDT table.
	add	edi, ecx
	mov	ds:RSDT_STRUCT_LENGTH, ecx
	mov	ds:RSDT_STRUCT_END_OFF, edi

	;===============================
	; Print table's header.
	;===============================
	; *********************************************************************************
	; *-------------------------------------------------------------------------------*
	; * LineAddr  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  |  0123456789ABCDEF*
	; *-------------------------------------------------------------------------------*
	; *********************************************************************************
	mov	dx, offset ds:Memory_In32bit_Block_Line1
	call	PrintString
	mov	dx, offset ds:Memory_In32bit_Block_Line2
	call	PrintString
	mov	dx, offset ds:Memory_In32bit_Block_Line3
	call	PrintString

	mov	ebx, ds:RSDT_STRUCT_START_OFF

	;===============================
	; Print memory in 32bit block content.
	;===============================
	; *********************************************************************************
	; * 3F7A0000 52 53 44 20 50 54 52 20 7D 41 43 50 49 41 4D 00  |  RSD.PTR.}ACPIAM. *
	; * 3F7A0010 00 00 7A 3F 24 00 00 00 00 01 7A 3F 00 00 00 00  |  ..z?$.....z?.... *
	; * 3F7A0020 22 00 00 00 08                                   |  "....            *
	; *********************************************************************************
	; EBX - RSDT_STRUCT_START_OFF
	.repeat	; Print 1 line per times.
		;=======================
		; Print "  3F7A0010 "
		;=======================
		mov	dx, '  '
		call	PrintDXAscii
		mov	edx, ebx
		call	PrintEDXContent
		mov	dl, ' '
		call	PrintDLAscii

		xor	edi, edi			; 00h - 0Fh
		;=======================
		; Print ASCII code. "40"
		;=======================
		.repeat
			mov	esi, ebx
			add	esi, edi
			sub	esi, ds:RSDT_STRUCT_START_OFF
			.if	esi<ds:RSDT_STRUCT_LENGTH
				mov	dl, fs:[ebx+edi]
				call	PrintDLContent
			.else
				mov	dx, '  '	; Dont print contents more than 24Bytes.
				call	PrintDXAscii
			.endif
			mov	dl, ' '
			call	PrintDLAscii
			inc	edi
		.until	di==10h

		mov	edx, ' |  '
		call	PrintEDXAscii

		xor	edi, edi			; 00h - 0Fh

		;=======================
		; Print Char. "A"
		;=======================
		.repeat
			mov	esi, ebx
			add	esi, edi
			sub	esi, ds:RSDT_STRUCT_START_OFF
			.if	esi<ds:RSDT_STRUCT_LENGTH
				mov	dl, fs:[ebx+edi]
				.if	dl>20h		; Dont print Cntl char.
					call	PrintDLAscii
				.else
					mov	dl, '.'
					call	PrintDLAscii
				.endif
			.else
				mov	dx, ' '
				call	PrintDLAscii
			.endif
			inc	edi
		.until	edi==10h

		add	ebx, 10h			; Print next 10Byte.
		call	PrintEnter
	.until	ebx>=ds:RSDT_STRUCT_END_OFF

	call	PrintEnter

	;===============================
	; Print RSDT detail infor
	;===============================
	mov	dx, offset ds:RSDT_String_Title
	call	PrintString

	; "Root System Description Table:"
	mov	dx, offset ds:RSDT_String_Pointer
	call	PrintString
	mov	edx, ds:RSDT_STRUCT_START_OFF
	call	PrintEDXContent
	call	PrintEnter
	mov	dx, offset ds:Table_String_Line
	call	PrintString

	; "RSDT Signature"
	mov	dx, offset ds:RSDT_String_Signature
	call	PrintString
	mov	edi, ds:RSDT_STRUCT_START_OFF	; EDI - RSDT_STRUCT_START_OFF
	mov	ecx, 4				; ECX - Signature length is 4B
	call	PrintStringIn32bitMEM
	call	PrintEnter

	; "RSDT Length"
	mov	dx, offset ds:RSDT_String_Length
	call	PrintString
	mov	edi, ds:RSDT_STRUCT_START_OFF
	add	edi, 4				; EDI - RSDT_STRUCT_START_OFF+4
	mov	ecx, 4				; ECX - RSDT Length is 4Byte
	call	PrintContentIn32bitMEMBigEnding
	call	PrintEnter

	; "RSDT Revision"
	mov	dx, offset ds:RSDT_String_Revision
	call	PrintString
	mov	edi, ds:RSDT_STRUCT_START_OFF
	mov	dl, fs:[edi+8]
	call	PrintDLContent
	call	PrintEnter

	; "RSDT Checksum"
	mov	dx, offset ds:RSDT_String_Checksum
	call	PrintString
	mov	edi, ds:RSDT_STRUCT_START_OFF
	mov	dl, fs:[edi+9]
	call	PrintDLContent
	call	PrintEnter

	; "RSDT OEM ID"
	mov	dx, offset ds:RSDT_String_OEM_ID
	call	PrintString
	mov	edi, ds:RSDT_STRUCT_START_OFF
	add	edi, 10			; EDI - RSDT_STRUCT_START_OFF+10
	mov	ecx, 6			; ECX - length is 6B
	call	PrintStringIn32bitMEM
	call	PrintEnter

	; "RSDT OEM Table ID"
	mov	dx, offset ds:RSDT_String_OEM_Table_ID
	call	PrintString
	mov	edi, ds:RSDT_STRUCT_START_OFF
	add	edi, 16			; EDI - RSDT_STRUCT_START_OFF+16
	mov	ecx, 8			; ECX - length is 8B
	call	PrintStringIn32bitMEM
	call	PrintEnter

	; "RSDT OEM Revision"
	mov	dx, offset ds:RSDT_String_OEM_Revision
	call	PrintString
	mov	edi, ds:RSDT_STRUCT_START_OFF
	add	edi, 24			; EDI - RSDT_STRUCT_START_OFF+24
	mov	ecx, 4			; ECX - Length is 4Byte
	call	PrintContentIn32bitMEMBigEnding
	call	PrintEnter

	; "RSDT Creator ID"
	mov	dx, offset ds:RSDT_String_Creator_ID
	call	PrintString
	mov	edi, ds:RSDT_STRUCT_START_OFF
	add	edi, 28			; EDI - RSDT_STRUCT_START_OFF+28
	mov	ecx, 4			; ECX - length is 4B
	call	PrintStringIn32bitMEM
	call	PrintEnter

	; "RSDT Creator Revision"
	mov	dx, offset ds:RSDT_String_Creator_Revision
	call	PrintString
	mov	edi, ds:RSDT_STRUCT_START_OFF
	add	edi, 32			; EDI - RSDT_STRUCT_START_OFF+32
	mov	ecx, 4			; ECX - Length is 4Byte
	call	PrintContentIn32bitMEMBigEnding
	call	PrintEnter

	; ==== "RSDT Entry0~10" Start ==== ;
	xor	cx, cx
	mov	edi, ds:RSDT_STRUCT_START_OFF
	add	edi, 36			; ptr to "Entry0".
	.repeat	; One Entry per line.
		mov	dx, offset ds:RSDT_String_Entry1
		call	PrintString	; "Entry"
		mov	dx, cx
		call	PrintDLContent	; the number of this Entry, e.g."0"
		mov	dx, offset ds:RSDT_String_Entry2
		call	PrintString	; "		| "

		mov	edx, fs:[edi]
		call	PrintEDXContent
		call	PrintEnter
		add	edi, 4		; ptr to next Entry.
	.until	edi>=ds:RSDT_STRUCT_END_OFF


;	mov	edi, ds:RSDT_STRUCT_START_OFF
;	add	edi, 36			; EDI - RSDT_STRUCT_START_OFF+36
;	mov	ecx, 4			; ECX - Length is 4Byte
;	call	PrintContentIn32bitMEMBigEnding
;	call	PrintEnter



	; ==== "RSDT Entry0~10" End ==== ;

	;===============================
	; Back to 16bit memory access.
	;===============================
	call	ExitBigRealMode

	pop	es
	popad
	ret
PrintRSDT	endp

code	ends