data	segment
;===============================================================================
; Labels on the header
;===============================================================================
RSDP_Label_String	db	' RSDP $'	; Not select.
RSDP_Label_String_Sel	db	'(RSDP)$'	; When select.
RSDT_Label_String	db	' RSDT $'
RSDT_Label_String_Sel	db	'(RSDT)$'
FACP_Label_String	db	' FACP $'
FACP_Label_String_Sel	db	'(FACP)$'

;===============================================================================
; RSDP table content
;===============================================================================

;=======================================
; Line to be printed
;=======================================
Memory_In16bit_Block_Line1	db	'-------------------------------------------------------------------------------', CR, LF, '$'
Memory_In16bit_Block_Line2	db	'  Seg:Off  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  |  0123456789ABCDEF', CR, LF, '$'
Memory_In16bit_Block_Line3	db	'-------------------------------------------------------------------------------', CR, LF, '$'

;=======================================
; Contents to be printed
;=======================================
RSDP_String_Title		db	'RSDP Structure', CR, LF, '$'
RSDP_String_Pointer		db	'Root System Description Pointer: $'
RSDP_String_Line		db	'----------------------------------------------', CR, LF, '$'
RSDP_String_Signature		db	'Signature', TAB, '| $'
RSDP_String_Checksum		db	'Checksum', TAB, '| $'
RSDP_String_OEM_ID		db	'OEM ID', TAB, TAB, '| $'
RSDP_String_Revision1		db	'Revision', TAB, '| $'
RSDP_String_Revision2		db	' - ACPI $'
RSDP_String_Revision3		db	'.0$'
RSDP_String_RSDT_Address	db	'RSDT Address', TAB, '| $'
RSDP_String_RSDP_Length		db	'Length', TAB, TAB, '| $'
RSDP_String_XSDT_Address	db	'XsdtAddress', TAB, '| $'
RSDP_String_Extended_Checksum	db	'Ext Checksum', TAB, '| $'
RSDP_String_Reserved_BYTEs	db	'Reserved', TAB, '| $'

;===============================================================================
; RSDT table content
;===============================================================================

;=======================================
; Line to be printed
;=======================================
Memory_In32bit_Block_Line1	db	'-------------------------------------------------------------------------------', CR, LF, '$'
Memory_In32bit_Block_Line2	db	'  LineAddr 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  |  0123456789ABCDEF', CR, LF, '$'
Memory_In32bit_Block_Line3	db	'-------------------------------------------------------------------------------', CR, LF, '$'

;=======================================
; Contents to be printed
;=======================================
RSDT_String_Title		db	'RSDT Structure', CR, LF, '$'
RSDT_String_Pointer		db	'Root System Description Table: $'
RSDT_String_Line		db	'----------------------------------------------', CR, LF, '$'
RSDT_String_Signature		db	'Signature', TAB, '| $'
RSDT_String_Length		db	'Length', TAB, '| $'
RSDT_String_Revision		db	'Revision', TAB, '| $'
RSDT_String_Checksum		db	'Checksum', TAB, '| $'
RSDT_String_OEM_ID		db	'OEM ID', TAB, '| $'
RSDT_String_OEM_Table_ID	db	'OEM Table ID', TAB, '| $'
RSDT_String_OEM_Revision	db	'OEM Revision', TAB, '| $'
RSDT_String_Creator_ID		db	'Creator ID', TAB, '| $'
RSDT_String_Creator_Revision	db	'Creator Revision', TAB, '| $'
RSDT_String_Entry0		db	'Entry0', TAB, '| $'
RSDT_String_Entry1		db	'Entry1', TAB, '| $'
RSDT_String_Entry2		db	'Entry2', TAB, '| $'
RSDT_String_Entry3		db	'Entry3', TAB, '| $'
RSDT_String_Entry4		db	'Entry4', TAB, '| $'
RSDT_String_Entry5		db	'Entry5', TAB, '| $'
RSDT_String_Entry6		db	'Entry6', TAB, '| $'
RSDT_String_Entry7		db	'Entry7', TAB, '| $'
RSDT_String_Entry8		db	'Entry8', TAB, '| $'
RSDT_String_Entry9		db	'Entry9', TAB, '| $'
RSDT_String_Entry10		db	'Entry10', TAB, '| $'
data	ends