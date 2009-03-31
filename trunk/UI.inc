;*******************************************************************************
; Filename:	UI.asm
; Description:	This runtine is involved by lsACPI.asm
;		Display 80x50 or 80x25 video mode.
;*******************************************************************************

assume	cs:code

code	segment


;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------


;*******************************************************************************
; Procedure:	Go2VideoMode80x50
;
; Description:	Go to Video mode: 80x50 to show more lines, also more contents.
;
; Input:	None.
;
; Output:	None.
;
; Change:	AX, BX.
;
;*******************************************************************************
Go2VideoMode80x50	proc
	push	ax
	push	bx
	mov	ax, 1112h
	xor	bl, bl
	int	10h
	pop	bx
	pop	ax
	ret
Go2VideoMode80x50	endp


;*******************************************************************************
; Procedure:	Go2VideoMode80x25
;
; Description:	Go to Video mode: 80x50 to show more lines, also more contents.
;
; Input:	None.
;
; Output:	None.
;
; Change:	AX.
;
;*******************************************************************************
Go2VideoMode80x25	proc
	push	ax
	mov	ax, 0003h
	int	10h
	pop	ax
	ret
Go2VideoMode80x25	endp


;*******************************************************************************
; Procedure:	PrintHeaderLabels
;
; Description:	Print Labels on the header.
;		When you select items like "RSDP", it shows as "*RSDP*".
;
; Input:	None.
;
; Output:	None.
;
; Change:	DX.
;
;*******************************************************************************
PrintHeaderLabels	proc
	push	dx

	;==============================
	; RSDP Label
	.if	ds:CurrentACPITableNo==01
		mov	dx, offset RSDP_Label_String_Sel
	.else
		mov	dx, offset RSDP_Label_String
	.endif
	call	PrintString

	;==============================
	; RSDT Label
	.if	ds:CurrentACPITableNo==02
		mov	dx, offset RSDT_Label_String_Sel
	.else
		mov	dx, offset RSDT_Label_String
	.endif
	call	PrintString

	;==============================
	; RSDT Label
	.if	ds:CurrentACPITableNo==03
		mov	dx, offset FACP_Label_String_Sel
	.else
		mov	dx, offset FACP_Label_String
	.endif
	call	PrintString

	call	PrintEnter

	pop	dx
	ret
PrintHeaderLabels	endp









;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------


code	ends