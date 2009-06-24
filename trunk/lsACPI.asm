;*******************************************************************************
; Filename:	lsACPI.asm
; Description:	This is the main file of this project.
;		List ACPI Tables.
;*******************************************************************************

;=======================================
; Include files
;=======================================
include	proc.inc
include	Rd4G.inc
include	RdMEM.inc
include	UI.inc
include	ACPI_Str.inc

assume	cs:code, ds:data

;=======================================
; Equations
;=======================================
CR	equ	0dh
LF	equ	0ah
TAB	equ	09h

BIT0	equ	00000001b
BIT1	equ	00000010b
BIT2	equ	00000100b
BIT3	equ	00001000b
BIT4	equ	00010000b
BIT5	equ	00100000b
BIT6	equ	01000000b
BIT7	equ	10000000b

;===============================================================================
;===============================================================================
;===============================================================================
;===============================================================================

data	segment
;---------------------------------------
; Debug code
;---------------------------------------
Debug_Code		db	00000000b
;				|||||||+-------- BIT0: Find "RSD PTR" in E000
;				||||||+--------- BIT1: Find "RSD PTR" in F000
;				|||||+---------- BIT2
;				||||+----------- BIT3
;				|||+------------ BIT4
;				||+------------- BIT5
;				|+-------------- BIT6
;				+--------------- BIT7

;/*------------------------------------*/
;/* I want to find this string in Seg:Off */
;/*------------------------------------*/
FIND_STRING		db	"RSD PTR $"
FIND_STRING_SIZE	dw	08h
FIND_STRING_ENDPTR	dw	07h
FIND_IN_SEG		dw	0000h
FIND_IN_SEG_E000	dw	0e000h
FIND_IN_SEG_F000	dw	0f000h		; 'RSD PTR ' is in E000 or F000, refer to ACPI Spec.

;/*------------------------------------*/
;/* RSDP Structure information */
;/*------------------------------------*/
RSDP_STRUCT_OFF		dw	0000h
RSDP_STRUCT_START_OFF	dw	0000h
RSDP_STRUCT_END_OFF	dw	0000h
RSDP_STRUCT_LENGTH	dw	24h

;/*------------------------------------*/
;/* RSDT Structure information */
;/*------------------------------------*/
RSDT_STRUCT_START_OFF	dd	00000000h
RSDT_STRUCT_END_OFF	dd	00000000h
RSDT_STRUCT_LENGTH	dd	00000024h

;/*===================================*/
;/* Define variables to access Memory */
;/*===================================*/
;/* Find string from Seg:Off, for length. */
Find_From_Off	dw	0		; Find from this offset.
Find_Length	dw	0
Found_Off	dw	0
Char_No		dw	0		; /* Ptr to a char in this string, mov to DI. */
Found_Flag	db	0

;/* I will print MEM's content from Off, for this length, using for() */
Print_In_Off	dw	0000h
Print_Length	dw	0000h
Print_Off	db	00h		; /* use in for(). */

;/*=============================================================*/
;/* Define Variables for the interface to show every ACPI Tables*/
;/*=============================================================*/
CurrentACPITableNo	db	01h	;/* Which ACPI Table currently I was looking at. */
PressKey_Scan_Code	db	00h	;/* Which KEY pressed scan code. */
PressKey_Ascii_Code	db	00h	;/* Which KEY pressed Ascii code. */
TableAmount		db	03h	;/* The amount of all ACPI Tables found in MEM. */
data	ends

;===============================================================================
;===============================================================================
;===============================================================================
;===============================================================================

code	segment
start:
	mov	ax, data
	mov	ds, ax

	;==================================================
	; To find "RSD PTR " in Segment E000 & F000, and return the offset of "R".
	; First find in segment E000
	mov	dx, FIND_IN_SEG_E000
	mov	FIND_IN_SEG, dx
	call	FindRSDPTR
	;=======================================
	; FindRSDPTR return 3 value is showing below.
	;=======================================
	;-----------------------------------------------------------------------
	;			|  Found	|  Not_found
	;-----------------------|---------------|-------------------------------
	;	Found_Off	|  1234h	|  0ffffh
	;	Found_Flag	|  0ffh		|  00h
	;-----------------------------------------------------------------------

	.if Found_Flag==0ffh		; Find success in E000.
		or	Debug_Code, BIT0
		jmp	find_success
	.elseif Found_Flag==00h		; Find faid in E000.
		and	Debug_Code, BIT0
	.endif

	; If not find in segment E000, then in F000.
	mov	dx, FIND_IN_SEG_F000
	mov	FIND_IN_SEG, dx
	call	FindRSDPTR

	.if Found_Flag==00h		; Find fail in F000.
		and	Debug_Code, BIT1
		jmp	not_find_rsd_prt_
	.elseif Found_Flag==0ffh	; Find success in F000.
		or	Debug_Code, BIT1
	.endif

find_success:
	;===============================
	; Save RSDP address
	;===============================
	mov	ax, Found_Off
	mov	RSDP_STRUCT_OFF, ax		; RSDP_STRUCT_OFF <- Found_Off
	mov	RSDP_STRUCT_START_OFF, ax	; RSDP_STRUCT_START_OFF <- Found_OFF
	add	ax, RSDP_STRUCT_LENGTH
	add	RSDP_STRUCT_END_OFF, ax		; RSDP_STRUCT_END_OFF <- Found_OFF+RSDP_STRUCT_LENGTH

	call	Go2VideoMode80x50	; Before beginning to print, go2 80x50.
	.repeat	; to print whole window.

		;=========================
		; Set current Page Number.
		mov	al, 0		; Page Number = 0
		mov	ah, 5
		int	10h

		;==============
		; Clear screen.
		mov	al, 0		; Roll down, '0' means whole lines.
		mov	bh, 0fh		; Front Color: 0, Back Color: F
		mov	cx, 0000	; Window left-up: (0, 0)
		mov	dx, 2580h	; Window right-down: (80, 25)
		mov	ah, 7
		int	10h

		;=========
		; set cur.
		mov	dx, 0000h	; (x, y) = (0, 0)
		mov	bh, 0		; Set page NUM = 0
		mov	ah, 2
		int	10h

		;===========================
		; Show Labels on the header.
		call	PrintHeaderLabels

		;============================
		; Show select Table's detail.
		.if	CurrentACPITableNo==01
			call	PrintRSDP
		.elseif
			call	PrintRSDT
		.endif

		;===============================================================
		; Print Debug Code
		;===============================================================
		call	PrintDebugCode

;		/*=================================================*/
;		/* Determine which KEY user press, and what to do. */
;		/*=================================================*/
;		/* Wait for user to press LEFT or RIGHT to select. */
		mov	ah, 0		; Wait user press KEY.
		int	16h
		mov	PressKey_Scan_Code, ah			; Scan code.
		mov	PressKey_Ascii_Code, al			; ASCII code.
;		/* Press LEFT */
		.if	PressKey_Scan_Code==4bh
			.if	CurrentACPITableNo==1
				mov	al, TableAmount
				mov	CurrentACPITableNo, al
			.else
				dec	CurrentACPITableNo	; CurrentACPITableNo--
			.endif
		.endif
;		/* Press RIGHT */
		.if	PressKey_Scan_Code==4dh
			mov	al, TableAmount
			.if	CurrentACPITableNo==al		; CurrentACPITableNo ?= TableAmount
				mov	CurrentACPITableNo, 01h
			.else
				inc	CurrentACPITableNo	; CurrentACPITableNo++
			.endif
		.endif

	.until	PressKey_Scan_Code==01				;/* Press ESC */

print_finish:
	call	PrintEnter
	;/* At last quit to normal 80x25 mode. */
	call	Go2VideoMode80x25
	jmp	exit

;=======================================
; Not find "RSD PTR ", so print not find information.
;=======================================
not_find_rsd_prt_:
	mov	dx, offset Not_Find_RSD_PTR
	call	PrintString
	call	PrintDebugCode

exit:
	mov	ax, 4c00h
	int	21h

;===============================================================================
;===============================================================================
;===============================================================================
;===============================================================================

;*******************************************************************************
; Procedure:	FindRSDPTR
;
; Description:	Find the signature "RSD PTR ", and ret the address.
;
; Input:	None.
;
; Output:	None.
;
; Change:	.
;
;*******************************************************************************
FindRSDPTR	proc	near
	mov	ax, data
	mov	ds, ax

	;===============================
	; Initial variable
	;===============================
	mov	Find_From_Off, 0000h

	;/*====================================*/
	;/* Loop to find 'RSD PTR ' in MEM.    */
	;/*====================================*/
	.repeat
		;/* Find first char 'R' */
		mov	cx, 0ffffh
		mov	ax, Find_From_Off
		sub	cx, ax			; Input: CX - Find the char from beginning for this length.
		mov	ax, FIND_IN_SEG
		mov	es, ax			; Input: ES - Find in this segment.
		mov	al, [FIND_STRING]	; Input: AL - Find this char saved in AL
		mov	di, Find_From_Off	; Input: DI - Find this char from ES:DI
		call	SearchCharIn16bitMEM
		; Output:	NC - Found.
		;		   DI - Find this char is in this offset.
		;		CY - Not found.
		;		   DI - None

		.if CARRY?
			; Not found
			;printf("#Not find 'RSD PTR ' in %04x ! \n", FIND_IN_SEG);
			jmp	frsdptr_not_found;
		.else
			; Found
			mov	Found_Off, di		; Ptr to found char.
			mov	Find_From_Off, di
			inc	Find_From_Off		; Ptr to next address after the found char.
							; Next time, find char from this offset.
			;//printf("#Find \'%c\', and its Off is %04x .\n", FIND_STRING[0], Found_Off);
		.endif

		; Find last char "SD PTR ".
		xor	si, si
		inc	si			; SI ptr to "S"
		mov	bx, Found_Off
		mov	ax, FIND_STRING_SIZE	; = 8
		.while	si<FIND_STRING_SIZE	; = 8
			mov	dl, [FIND_STRING+si]
			.if	es:[bx+si]!=dl
				; No, I will find next "R"
				.break
			.elseif	si==FIND_STRING_ENDPTR	; If .
				; Yes, it's the last char, then set flag to Found_Off.
				mov	Found_Flag, 0ffh
			.endif
			inc	si
		.endw
	.until	Found_Flag==0ffh

	;===============================
	; Already find "RSD PTR ".
	;===============================
	; Found_Off=01234h, ptr to "RSD PTR ".
	; Found_Flag=0ffh
	jmp	frsdptr_end

frsdptr_not_found:
	mov	Found_Off, 0ffffh	; Set
	mov	Found_Flag, 00h

frsdptr_end:
	ret
FindRSDPTR	endp

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
	mov	dx, offset Memory_In16bit_Block_Line1
	call	PrintString
	mov	dx, offset Memory_In16bit_Block_Line2
	call	PrintString
	mov	dx, offset Memory_In16bit_Block_Line3
	call	PrintString

	mov	es, FIND_IN_SEG
	mov	bx, RSDP_STRUCT_START_OFF

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
		mov	dx, FIND_IN_SEG
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
			sub	si, RSDP_STRUCT_START_OFF
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
	; Get RSDT Address.
	;===============================
	mov	es, ds:FIND_IN_SEG
	mov	di, ds:RSDP_STRUCT_START_OFF
	add	di, 16d			; Off of RSDT address.
	mov	edi, dword ptr es:[di]
	mov	ds:RSDT_STRUCT_START_OFF, edi
	add	edi, RSDT_STRUCT_LENGTH
	mov	RSDT_STRUCT_END_OFF, edi

	;===============================
	; Access memory for 32bit (4GB RAM).
	;===============================
	call	JumpToBigRealMode

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
	.until	ebx>RSDT_STRUCT_END_OFF

	call	PrintEnter

	;===============================
	; Print RSDP detail infor
	;===============================
	; To be continued...
	; b1nggou

	;===============================
	; Back to 16bit memory access.
	;===============================
	call	ExitBigRealMode

	pop	es
	popad
	ret
PrintRSDT	endp

;*******************************************************************************
; Procedure:	PrintDebugCode
;
; Description:	Print Debug_Code.
;
; Input:	None.
;
; Output:	None.
;
; Change:	.
;
;*******************************************************************************
PrintDebugCode	proc	near
	push	bx
	push	dx

	call	PrintEnter
	mov	bl, Debug_Code
	call	PrintBLByBit
	call	PrintEnter
	mov	dx, offset Debug_Code_BIT0
	call	PrintString
	mov	dx, offset Debug_Code_BIT1
	call	PrintString
	mov	dx, offset Debug_Code_BIT2
	call	PrintString

	pop	dx
	pop	bx

	ret
PrintDebugCode	endp

;===============================================================================
;/*
; * Debug, set a break, 
; * press ESC to quit of the routine, 
; * press enter to jmp to the routine
; */
;debug	proc	near
;	int key;
;	while(1)
;	{
;		key = bioskey(0);
;		if(key==0x011b)
;			exit(0);
;		else if(key==0x1c0d)
;			break;
;		else
;			continue;
;	}
;debug	endp

code	ends
	end	start