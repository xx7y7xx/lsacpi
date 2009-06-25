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
include	PrtTab.inc
include	DBGR.inc
include	Str.inc

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

code	ends
	end	start