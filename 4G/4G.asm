include	proc.inc
include	4G.inc

assume	cs:code

code	segment
start:
	call	JumpToBigRealMode
	mov	esi, 1bfa0000h
	mov	edx, fs:[esi]
	call	PrintEDXContent
	call	ExitBigRealMode

	mov	ax, 4c00h
	int	21h
code	ends
	end start

