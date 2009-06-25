;*******************************************************************************
; Filename:	PrtTab.asm
; Description:	This runtine is involved by lsACPI.asm
;		Print RSDP, RSDT ... tables.
;*******************************************************************************

assume	cs:code

code	segment

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
	mov	bl, ds:Debug_Code
	call	PrintBLByBit
	call	PrintEnter
	mov	dx, offset ds:Debug_Code_BIT0
	call	PrintString
	mov	dx, offset ds:Debug_Code_BIT1
	call	PrintString
	mov	dx, offset ds:Debug_Code_BIT2
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