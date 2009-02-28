/*
 * Devin Chen, 200901
 * ACPI.C -- List ACPI tables.
 */

#include <stdio.h>
#include <conio.h>
#include <bios.h>


/*============================================================================*/
/* Function invoke in ACPIi.asm */
/*============================================================================*/
//extern short SearchStringInSegment(short, short, short);
/*
 * _SearchCharIn16bitMEM return the next BYTE of the char found in MEM. 
 * If not found, return 'FFFF'
 */
extern unsigned short _SearchCharIn16bitMEM(unsigned char, unsigned short, unsigned short, unsigned short);
//extern void PrintStringInMEM(short, short, short);
/*
 * _GetBYTEContentIn16bitMEM()
 * Input :	Seg
 * 		Off
 * Output :	A BYTE of this address of the MEM.
 */
extern unsigned char _GetBYTEContentIn16bitMEM(short, short);



/*============================================================================*/
/* This functions help me access Memory to display ACPI Table content. */
/*============================================================================*/

/*====================================*/
/* I want to find this string in Seg:Off */
/*====================================*/
unsigned char FIND_STRING[] = "RSD PTR ";
unsigned int FIND_STRING_SIZE = sizeof(FIND_STRING) - 1;
unsigned short FIND_IN_SEG = 0xf000;
unsigned short FIND_IN_OFF = 0x0000;

/*====================================*/
/* RSDP Structure information */
/*====================================*/
unsigned short RSDP_STRUCT_OFF;
int RSDP_STRUCT_LENGTH = 0x24;

/*
 * Debug, set a break, 
 * press ESC to quit of the routine, 
 * press enter to jmp to the routine
 */
void debug(void)
{
	int key;
	while(1)
	{
		key = bioskey(0);
		if(key==0x011b)
			exit(0);
		else if(key==0x1c0d)
			break;
		else
			continue;
	}
}

void PrintCharRmCtrlAscii(unsigned char Byte_Content)
{
	if(Byte_Content<0x20)
		printf(".");
	else
		printf("%c", Byte_Content);
}

void PrintStringIn16bitMEM(unsigned short Print_String_Seg, unsigned short Print_String_Off, unsigned short Print_String_Length)
{
	unsigned int Print_Off = 0x00;
	unsigned char Byte_Content = 0x00;
	for(; Print_Off<Print_String_Length; Print_Off++)
	{
		Byte_Content = _GetBYTEContentIn16bitMEM(Print_String_Seg, Print_String_Off+Print_Off);
		printf("%c", Byte_Content);
	}
}

/*
 * PrintContentIn16bitMEMLittleEnding()
 * Input :	Seg	Seg of Byte to print.
 * 		Off	Off of Byte to print.
 * 		Length	No of Byte to Print.
 * Output :	Print Byte contents from "Seg:Off" for "Length".
 * 		Print for LittleEnding.
 */
void PrintContentIn16bitMEMLittleEnding(unsigned short Print_Content_Seg, unsigned short Print_Content_Off, unsigned short Print_Content_Length)
{
	unsigned int Print_Off = 0x00;
	unsigned char Byte_Content = 0x00;
	for(; Print_Off<Print_Content_Length; Print_Off++)
	{
		Byte_Content = _GetBYTEContentIn16bitMEM(Print_Content_Seg, Print_Content_Off+Print_Off);
		printf("%02x", Byte_Content);
	}
}

/*
 * PrintContentIn16bitMEMBigEnding()
 * Input :	Seg	Seg of Byte to print.
 * 		Off	Off of Byte to print.
 * 		Length	No of Byte to Print.
 *
 * Output :	Print Byte contents from "Seg:Off" for "Length".
 * 		Print for BigEnding.
 *
 * Note :	If content in MEM like this:
 *			|--------|---------------|
 *			|Address | 01 02 03 04 05|
 *			|--------|---------------|
 *			|Content | 00 01 7a 3f 00|
 *			|--------|---------------|
 *		Then print "003F7A0100" for BigEnding.
 */
void PrintContentIn16bitMEMBigEnding(unsigned short Print_Content_Seg, unsigned short Print_Content_Off, unsigned short Print_Content_Length)
{
	unsigned int Print_Off = Print_Content_Length;
	unsigned char Byte_Content = 0x00;
	do
	{
		Print_Off--;
		Byte_Content = _GetBYTEContentIn16bitMEM(Print_Content_Seg, Print_Content_Off+Print_Off);
		printf("%02x", Byte_Content);
	}
	while(Print_Off!=0);
}

void PrintBlockIn16bitMEM(unsigned short Print_Block_Seg, unsigned short Print_Block_Off, unsigned short Print_Block_Length)
{
	unsigned int Print_Off = 0x00;
	unsigned char Byte_Content = 0x00;

	printf("-------------------------------------------------------------------------------\n");
	printf("  Seg:Off  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  |  0123456789ABCDEF\n");
	printf("-------------------------------------------------------------------------------\n");

	do
	{
		/* Start of this line. */
		printf("");
		if(Print_Off<Print_Block_Length)
			printf(" %04x:%04x ", Print_Block_Seg, Print_Block_Off+Print_Off);
		else
			printf("     ");

		/* Print ASCII code in MEM. */
		do
		{
			Byte_Content = _GetBYTEContentIn16bitMEM(Print_Block_Seg,  Print_Block_Off+Print_Off);
			if(Print_Off<Print_Block_Length)
				printf("%02x ", Byte_Content);
			else
				printf("   ");
			Print_Off++;
		}
		while((Print_Off%0x10)!=0);

		/* Put the ptr back 10H */
		Print_Off -= 0x10;
		printf(" |  ");

		/* Print the char according to the ASCII code. */
		do
		{
			Byte_Content = _GetBYTEContentIn16bitMEM(Print_Block_Seg,  Print_Block_Off+Print_Off);
			if(Print_Off<Print_Block_Length)
				//printf("%c", Byte_Content);
				PrintCharRmCtrlAscii(Byte_Content);
			else
				printf(" ");
			Print_Off++;
		}
		while((Print_Off%0x10)!=0);

		/* End of this line. */
		printf("\n");
	}
	while(Print_Off!=((Print_Block_Length&0xfff0)+0x10));

	printf("-------------------------------------------------------------------------------\n");
}

void PrintRSDPTableContent(void)
{
	unsigned char Byte_Content; /* Save a BYTE getted from MEM */

	/*======================================*/
	/* Print RSDP structure content in MEM. */
	/*======================================*/
	printf("RSDP Structure\n");
	PrintBlockIn16bitMEM(FIND_IN_SEG, RSDP_STRUCT_OFF, RSDP_STRUCT_LENGTH);
	printf("\n");
	printf("Root System Description Pointer: %04x:%04x\n", FIND_IN_SEG, RSDP_STRUCT_OFF);
	printf("----------------------------------------------\n");

	/* Print RSDP Signature. */
	printf("Signature\t| ");
	//printf("%c \n", _GetBYTEContentIn16bitMEM(FIND_IN_SEG, RSDP_STRUCT_OFF));
	PrintStringIn16bitMEM(FIND_IN_SEG, RSDP_STRUCT_OFF, 8);
	printf("\n");

	/* Print RSDP Checksum. */
	/* The offset&length of Checksum is define in ACPI Spec */
	Byte_Content = _GetBYTEContentIn16bitMEM(FIND_IN_SEG, RSDP_STRUCT_OFF+8);
	printf("Checksum\t| %02x\n", Byte_Content);

	/* Print RSDP OEM ID. */
	printf("OEM ID\t\t| ");
	PrintStringIn16bitMEM(FIND_IN_SEG, RSDP_STRUCT_OFF+9, 6);
	printf("\n");

	/* Print RSDP Revision. */
	Byte_Content = _GetBYTEContentIn16bitMEM(FIND_IN_SEG, RSDP_STRUCT_OFF+15);
	printf("Revision\t| %02d - ACPI %d.0\n", Byte_Content, Byte_Content);

	/* Print RSDP RSDT Address. */
	printf("RSDT Address\t| ");
	PrintContentIn16bitMEMBigEnding(FIND_IN_SEG, RSDP_STRUCT_OFF+16, 4);
	printf("\n");

	/* Print RSDP Length. */
	printf("Length\t\t| ");
	PrintContentIn16bitMEMBigEnding(FIND_IN_SEG, RSDP_STRUCT_OFF+20, 4);
	printf("\n");

	/* Print RSDP XSDT Address. */
	printf("XsdtAddress\t| ");
	PrintContentIn16bitMEMBigEnding(FIND_IN_SEG, RSDP_STRUCT_OFF+24, 8);
	printf("\n");

	/* Print RSDP Extended Checksum. */
	Byte_Content = _GetBYTEContentIn16bitMEM(FIND_IN_SEG, RSDP_STRUCT_OFF+32);
	printf("Ext Checksum\t| %02x\n", Byte_Content);

	/* Print RSDP Reserved BYTEs. */
	printf("Reserved\t| ");
	PrintContentIn16bitMEMBigEnding(FIND_IN_SEG, RSDP_STRUCT_OFF+33, 3);
	printf("\n");
}



/*============================================================================*/
/* This functions give my little routine a good looks. */
/*============================================================================*/
struct ACPITableStruct
{
	int ACPITabelNo;
	char ACPITableName[4];
}
ACPITableStruct[] =
{
	{1, "RSDP"},
	{2, "RSDT"},
	{3, "FACP"},
	{4, "APIC"},
};

void PrintLabels(int CurrentLabelNo)
{
	/* Highlight the current LABEL I selected. */
	textbackground(GREEN);

	/* Print LABELs, the LABEL select with highlight. */
	if(CurrentLabelNo==1)
		cprintf("  RSDP  ");
	else
		printf("  RSDP  ");
	if(CurrentLabelNo==2)
		cprintf("  RSDT  ");
	else
		printf("  RSDT  ");
	if(CurrentLabelNo==3)
		cprintf("  FACP  ");
	else
		printf("  FACP  ");
	if(CurrentLabelNo==4)
		cprintf("  APIC  ");
	else
		printf("  APIC  ");
	cprintf("\n\r                                                                                \r\n");
}

void PrintCurrentACPITableContent(int CurrentACPITableNo)
{
	if(CurrentACPITableNo==1)
		PrintRSDPTableContent();
	else if(CurrentACPITableNo==2)
		printf("RSDT table contents.\n");
	else if(CurrentACPITableNo==3)
		printf("FACP table contents.\n");
	else if(CurrentACPITableNo==4)
		printf("APIC table contents.\n");
}



/*============================================================================*/
/* Main */
/*============================================================================*/
void main(void)
{
	/*===================================*/
	/* Define variables to access Memory */
	/*===================================*/
	/* Find string from Seg:Off, for length. */
	unsigned short Find_Length;
	unsigned short Found_Off;
	unsigned int Char_No; /* Ptr to a char in this string. */

	/* I will print MEM's content from Off, for this length, using for() */
	unsigned short Print_In_Off, Print_Length;
	int Print_Off; /* use in for(). */


	/*=============================================================*/
	/* Define Variables for the interface to show every ACPI Tables*/
	/*=============================================================*/
	int CurrentACPITableNo = 1;	/* Which ACPI Table currently I was looking at. */
	short PressKey;			/* Which KEY user press. */
	int TableAmount = 4;		/* The amount of all ACPI Tables found in MEM. */


	/*====================================*/
	/* Loop to find 'RSD PTR ' in MEM. */
	/*====================================*/
	Found_Off = 0x0000;
	do
	{
		/* Find first char 'R' */
		Find_Length = 0xffff - FIND_IN_OFF;
		Found_Off = _SearchCharIn16bitMEM(FIND_STRING[0], FIND_IN_SEG, FIND_IN_OFF, Find_Length);
	
		if(Found_Off==0xffff)
		{
			printf("#Not find 'RSD PTR ' in %04x ! \n", FIND_IN_SEG);
			exit(0);
		}
		else
		{
			/* Ptr to found char. */
			FIND_IN_OFF = Found_Off;
			//printf("#Find \'%c\', and its Off is %04x .\n", FIND_STRING[0], Found_Off);
		}
	
		/* Find last chars 'SD PTR ' */
		Find_Length = 1;
		for(Char_No=1; Char_No<FIND_STRING_SIZE; Char_No++)
		{
			Found_Off = _SearchCharIn16bitMEM(FIND_STRING[Char_No], FIND_IN_SEG, FIND_IN_OFF, Find_Length);
			if(Found_Off==0xffff)
			{
				//printf("#Not find \'%c\' ! \n", FIND_STRING[Char_No]);
				break;
			}
			else
			{
				/* Ptr to found char. */
				FIND_IN_OFF = Found_Off;
				//printf("#Find \'%c\', and its Off is %04x .\n", FIND_STRING[Char_No], Found_Off-1);
			}
		}
	}
	while(Found_Off==0xffff);
	/*
	 * Now Found_Off ptr to the [Off(' ')+1], the second SPACE in 'RSD PTR '.
	 * But we want the Off('R'), so SUB it with the size of string.
	 */
	RSDP_STRUCT_OFF = Found_Off - FIND_STRING_SIZE;


	/*====================================*/
	/* Tell user where we found it. */
	/*====================================*/
	//printf("\n\nThis routine will find string \'");
	//for(Char_No=0; Char_No<FIND_STRING_SIZE; Char_No++)
	//{
	//	printf("%c", FIND_STRING[Char_No]);
	//}
	//printf("\' in Memory in Segment %04x .\n", FIND_IN_SEG);
	//printf("String size: %d\n", FIND_STRING_SIZE);
	//printf("Found it in offset %04x\n", RSDP_STRUCT_OFF);


	/*===========================================================*/
	/* Below is my beautiful interface to show every ACPI Table. */
	/*===========================================================*/
	while(1)
	{
		/* At all first, paint Screen with BLACK. */
		textbackground(BLACK);
		clrscr();


		/*=============================================================*/
		/* Print the current ACPI Table's content which user selected. */
		/*=============================================================*/
		/* Print Labels with current Tabel's LABEL highlight. */
		PrintLabels(CurrentACPITableNo);
		/* Print ACPI Table's content which user select. */
		PrintCurrentACPITableContent(CurrentACPITableNo);


		/*=================================================*/
		/* Determine which KEY user press, and what to do. */
		/*=================================================*/
		/* Wait for user to press LEFT or RIGHT to select. */
		PressKey = bioskey(0);
		/* Press LEFT */
		if(PressKey==0x4b00)
		{
			if(CurrentACPITableNo==1)
				CurrentACPITableNo = TableAmount;
			else
				CurrentACPITableNo--;
		}
		/* Press RIGHT */
		if(PressKey==0x4d00)
		{
			if(CurrentACPITableNo==TableAmount)
				CurrentACPITableNo = 1;
			else
				CurrentACPITableNo++;
		}
		/* Press ESC */
		if(PressKey==0x011b)
			exit(0);
	}


/*====================================*/
/* End of this routine. */
/*====================================*/
	printf(".\n\nEND of routine.\n");
}
