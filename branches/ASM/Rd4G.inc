;*******************************************************************************
; Filename:	4G_MEM.asm
; Description:	This runtine is involved by lsACPI.asm
;		Jump to big real mode.
;*******************************************************************************

;b1nggou090318	- Remove the string used to make sure we are in Big Real mode.

code segment
.586P
;*******************************************************************************
;Procedure:	JumpToBigRealMode
;Description:	Jump to big real mode from real mode
;		after call this procedure, you can use ES, FS or GS to
;		access memory up to 4G. But notice that: you cannot modify the
;		three registers unless you don't want to access 4G any more.
;Destroyed:	ES, FS, GS
;Input:		None
;Output:  	None
;*******************************************************************************
JumpToBigRealMode proc near
	jmp	ProcMainCode
;*******************************************************************************
;Struct define
Descriptor	STRUCT				;8 bytes
	LimitL	dw	0
	BaseL	dw	0
	BaseM	db	0
	Attrib	dw	0
	BaseH	db	0
Descriptor	ENDS
;*******************************************************************************
;Proc data
	gdt0	Descriptor   <>			;NULL
	
	DATASEL = $-gdt0			;Selector1
	DataDes Descriptor	<0ffffh,,,0f92h,>

	CODESEL = $-gdt0			;Selector2
	CodeDes Descriptor	<0ffffh,,,0f98h,>

	EDATASEL = $-gdt0			;Selector3
	EDataDes Descriptor	<0ffffh,0,0,0cf92h,0>

	GDTLEN = $-gdt0
	GdtPtr	dw	GDTLEN       	 	;Load for GDTR
		dd	OFFSET gdt0

;b1nggou090318	- 	ChangeOverStr	db	"Now is in Big-real mode.$"
	
	DataES	dw	0
	DataFS	dw	0
	DataGS	dw	0
;*******************************************************************************
;Main code
ProcMainCode:
	push	es					;Save ES, FS, GS
	pop	cs:[DataES]
	push	fs
	pop	cs:[DataFS]
	push	gs
	pop	cs:[DataGS]

	push	eax
	push	edx

	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	mov	cs:[DataDes].BaseL, ax			;Load DATA descriptor   
	shr	eax, 16
	mov	cs:[DataDes].BaseM, al
	mov	cs:[DataDes].BaseH, ah

	xor	eax, eax				;Load CODE descriptor
	mov	ax, cs
	shl	eax, 4
	mov	edx, eax
	add	edx, OFFSET gdt0
	mov	dword ptr cs:[GdtPtr+2], edx
	mov	cs:[CodeDes].BaseL, ax
	shr	eax, 16
	mov	cs:[CodeDes].BaseM, al
	mov	cs:[CodeDes].BaseH, ah
	call	EnableA20
	lgdt	fword PTR cs:[GdtPtr]			;Load GdtPtr to GDT   

	cli
	mov	eax, cr0                 		;Go to protected mode
	or	eax, 1
	mov	cr0, eax
	sti

	db	0eah
	dw	OFFSET GotoProtectedMode
	dw	CODESEL
GotoProtectedMode:
	mov	ax, EDATASEL
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
ReadyToReal:						;Return to real address mode
	cli
	mov	eax, cr0
	and	eax, 0fffffffeh
	mov	cr0, eax
	sti

	db	0eah
	dw	OFFSET ReturnToReal
	dw	seg   ReturnToReal
ReturnToReal:
;b1nggou090318	- push	ds
;b1nggou090318	- push	cs
;b1nggou090318	- pop	ds					;Just to make sure that we did it!
;b1nggou090318	- mov	dx, OFFSET ChangeOverStr
;b1nggou090318	- mov	ah, 9
;b1nggou090318	- int	21h
;b1nggou090318	- pop	ds

	pop	edx
	pop	eax
	ret
JumpToBigRealMode endp
;*******************************************************************************
;Procedure:	EnableA20
;Description:	Enable A20 gate
;Input:		None
;Output:  	None
;*******************************************************************************
EnableA20 proc near
	push	ax
	in	al, 92h
	or	al, 2
	out	92h, al
	pop	ax
	ret
EnableA20 endp
;*******************************************************************************
;Procedure:	DisableA20
;Description:	Disable A20 gate
;Input:		None
;Output:  	None
;*******************************************************************************
DisableA20 proc near
	push	ax
	in	al, 92h
	and	al, 0fdh
	out	92h, al
	pop	ax 
	ret
DisableA20 endp
;**************************************************************************************
;Procedure:	ExitBigRealMode
;Description:	Exit to big real mode from real mode
;		(close A20 and resore the segment registor you destroyed befor)
;Input:		None
;Output:  	None
;***************************************************************************************
ExitBigRealMode proc near
	push	cs:[DataES]					;Restore ES, FS, GS
	pop	es
	push	cs:[DataFS]
	pop	fs
	push	cs:[DataGS]
	pop	gs
	call	DisableA20
	ret	
ExitBigRealMode endp
;***********************************************************************************************
code ends