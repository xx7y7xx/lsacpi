;*******************************************************************************
; Filename:	rMEM.asm
; Description:	This runtine is involved by lsACPI.asm
;		Search, read MEM.
;*******************************************************************************

assume	cs:code

code	segment
.386


;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------


;*******************************************************************************
; Procedure:	SearchCharIn16bitMEM
;
; Description:	Search the char given by user from 0000 to FFFF in a Seg in MEM.
;		Output the offset of the char.
;
; Input:	FindChar - I want to find this char, like "A", in memory.
;		FindInSeg - Find the char in this segment.
;		FindInOff - Find the char from this offset.
;		FindLength - Find the char from beginning for this length.
;
; Output:	NC - Found.
;		   DI - Find in this offset.
;		CY - Not found.
;		   DI - None.
;
; Change:	None.
;
;*******************************************************************************
SearchCharIn16bitMEM	proc near
	repnz	scasb			; Find AL from ES:DI for CX times.

	jz	scim_found_char		; Found, then jmp
	stc				; Not found, return set CY.
	jmp	return

scim_found_char:
	sub	di, 01h			; DI ptr to the offset after the found char, so sub one to ptr the found address.
	clc				; If found, set NC.

return:
;	Return	di
	ret
SearchCharIn16bitMEM	endp



;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------


;;*******************************************************************************
;; Procedure:	GetBYTEContentIn16bitMEM
;;
;; Description:	Get a BYTE's content in MEM, its SEg:Off is ES:DI.
;;		It is 16bit MEM, could access 1MB.
;;
;; Input:	ES:DI -	Seg:Off ptr to BYTE in MEM
;;
;; Output:	AL -	Return this BYTE's content.
;;
;; Change:	None
;;
;;*******************************************************************************
;GetBYTEContentIn16bitMEM	proc near FindInSeg:word, FindInOff:word
;	push	bp
;	mov	bp, sp
;
;	push	es
;	push	di
;
;	mov	es, FindInSeg
;	mov	di, FindInOff
;	mov	al, es:[di]
;
;	pop	di
;	pop	es
;
;	pop	bp
;	ret
;GetBYTEContentIn16bitMEM	endp


;*******************************************************************************
; Procedure:	PrintStringIn16bitMEM
;
; Description:	Print from ES:DI for the length CX.
;
; Input:	di - Off
;		cx - Length
;
; Output:	None.
;
; Change:	AX, CX, DX, ES, DI.
;
;*******************************************************************************
PrintStringIn16bitMEM	proc	near
	push	ax
	push	cx
	push	dx
	push	es
	push	di

	mov	ax, ds:FIND_IN_SEG
	mov	es, ax

	mov	ax, di
	add	ax, cx			; AX <- The end offset

	.repeat
		mov	dl, es:[di]
		call	PrintDLAscii
		inc	di
	.until	di==ax

	pop	di
	pop	es
	pop	dx
	pop	cx
	pop	ax
	ret	
PrintStringIn16bitMEM	endp


;*******************************************************************************
; Procedure:	PrintContentIn16bitMEMBigEnding
;
; Description:	Print Byte contents from "Seg:Off" for "Length".
;		Print for BigEnding.
;
; Input:	di - Off
;		cx - Length
;
; Output:	None.
;
; Change:	BX, CX, DX, SI.
;
; * Note :	If content in MEM like this:
; *			|--------|---------------|
; *			|Address | 01 02 03 04 05|
; *			|--------|---------------|
; *			|Content | 00 01 7a 3f 00|
; *			|--------|---------------|
; *		Then print "003F7A0100" for BigEnding.
;*******************************************************************************
PrintContentIn16bitMEMBigEnding	proc	near
	push	bx
	push	cx
	push	dx
	push	si

	mov	bx, di
	mov	si, cx

	.repeat
		dec	si
		mov	dl, es:[bx+si]
		call	PrintDLContent
	.until	si==0

	pop	si
	pop	dx
	pop	cx
	pop	bx
	ret
PrintContentIn16bitMEMBigEnding	endp

;*******************************************************************************
; Procedure:	PrintStringIn32bitMEM
;
; Description:	Print from ES:DI for the length CX.
;
; Input:	edi - Start Off
;		ecx - Length
;
; Output:	None.
;
; Change:	AX, CX, DX, ES, DI.
;
;*******************************************************************************
PrintStringIn32bitMEM	proc	near
	push	eax
	push	ecx
	push	dx
	push	edi

	mov	eax, edi
	add	eax, ecx		; EAX <- The end offset

	.repeat
		mov	dl, fs:[edi]
		call	PrintDLAscii
		inc	edi
	.until	edi==eax

	pop	edi
	pop	dx
	pop	ecx
	pop	eax
	ret	
PrintStringIn32bitMEM	endp

;*******************************************************************************
; Procedure:	PrintStringIn32bitMEM
;
; Description:	Print byte contents from "Seg:Off" for "Length" by BigEnding.
;
; Input:	edi - Start Off
;		ecx - Length
;
; Output:	None.
;
; Change:	AX, CX, DX, ES, DI.
;
; * Note :	If content in MEM like this:
; *			|--------|---------------|
; *			|Address | 01 02 03 04 05|
; *			|--------|---------------|
; *			|Content | 00 01 7a 3f 00|
; *			|--------|---------------|
; *		Then print "003F7A0100" for BigEnding.
;*******************************************************************************
PrintContentIn32bitMEMBigEnding	proc	near
	push	ebx
	push	ecx
	push	dx
	push	esi

	mov	ebx, edi
	mov	esi, ecx

	.repeat
		dec	esi
		mov	dl, fs:[ebx+esi]
		call	PrintDLContent
	.until	esi==0

	pop	esi
	pop	dx
	pop	ecx
	pop	ebx
	ret
PrintContentIn32bitMEMBigEnding	endp

;;*******************************************************************************
;; Procedure:	GetBYTEContentIn32bitMEM
;;
;; Description:	Get a BYTE's content in MEM, its SEg:Off is ES:DI.
;;		It is 32bit MEM, could access 4GB.
;;
;; Input:	ES:DI -	Seg:Off ptr to BYTE in MEM
;;
;; Output:	AL -	Return this BYTE's content.
;;
;; Change:	None
;;
;;*******************************************************************************
;GetBYTEContentIn32bitMEM	proc near FindInOff:dword
;	push	bp
;	mov	bp, sp
;
;	push	esi
;
;	call	JumpToBigRealMode
;	mov	esi, FindInOff
;	mov	eax, fs:[esi]
;	call	ExitBigRealMode
;
;	pop	esi
;
;	pop	bp
;	ret
;GetBYTEContentIn32bitMEM	endp
;

;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------


code	ends