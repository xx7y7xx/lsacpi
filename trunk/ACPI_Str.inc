data	segment


;=======================================
; Labels on the header
;=======================================
RSDP_Label_String	db	' RSDP $'	; Not select.
RSDP_Label_String_Sel	db	'(RSDP)$'	; When select.
RSDT_Label_String	db	' RSDT $'
RSDT_Label_String_Sel	db	'(RSDT)$'
FACP_Label_String	db	' FACP $'
FACP_Label_String_Sel	db	'(FACP)$'





;=======================================
; RSDP table content
;=======================================

;======================
; Line to be printed
Memory_In16bit_Block_Line1	db	'-------------------------------------------------------------------------------', CR, LF, '$'
Memory_In16bit_Block_Line2	db	'  Seg:Off  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  |  0123456789ABCDEF', CR, LF, '$'
Memory_In16bit_Block_Line3	db	'-------------------------------------------------------------------------------', CR, LF, '$'

;=======================
; Contents to be printed
RSDP_String_Title	db	'RSDP Structure', CR, LF, '$'

RSDP_String_Pointer	db	'Root System Description Pointer: $'
RSDP_String_Line	db	'----------------------------------------------', CR, LF, '$'

RSDP_String_Signature	db	'Signature', TAB, '| $'

RSDP_String_Checksum	db	'Checksum', TAB, '| $'

RSDP_String_OEM_ID	db	'OEM ID', TAB, TAB, '| $'


RSDP_String_Revision1	db	'Revision', TAB, '| $'
RSDP_String_Revision2	db	' - ACPI $'
RSDP_String_Revision3	db	'.0$'

RSDP_String_RSDT_Address	db	'RSDT Address', TAB, '| $'

RSDP_String_RSDP_Length	db	'Length', TAB, TAB, '| $'


RSDP_String_XSDT_Address	db	'XsdtAddress', TAB, '| $'

RSDP_String_Extended_Checksum	db	'Ext Checksum', TAB, '| $'

RSDP_String_Reserved_BYTEs	db	'Reserved', TAB, '| $'



;=======================================
; RSDT table content
;=======================================

;======================
; Line to be printed
Memory_In32bit_Block_Line1	db	'-------------------------------------------------------------------------------', CR, LF, '$'
Memory_In32bit_Block_Line2	db	'  LineAddr 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  |  0123456789ABCDEF', CR, LF, '$'
Memory_In32bit_Block_Line3	db	'-------------------------------------------------------------------------------', CR, LF, '$'











data	ends