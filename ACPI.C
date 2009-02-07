/*
 * Devin Chen, 200901
 * ACPI.C -- List ACPI tables.
 */

#include <stdio.h>
#include <conio.h>

/* Function invoke in ex7-6i.asm */
//extern short SearchStringInSegment(short, short, short);
/*
 * SearchCharInMEM return the next BYTE of the char found in MEM. 
 * If not found, return 'FFFF'
 */
extern unsigned short SearchCharInMEM(unsigned char, unsigned short, unsigned short, unsigned short);
//extern void PrintStringInMEM(short, short, short);
extern unsigned char GetBYTEContentInMEM(short, short);
//extern void PrintBYTEAsciiInMEM(short, short);
//extern void PrintWORDContentInMEM(short, short);
//extern void PrintDWORDContentInMEM(short, short);




/* I want to find this string in Seg:Off */
unsigned char Find_String[] = "RSD PTR ";
unsigned int Find_String_Size = sizeof(Find_String) - 1;
unsigned short Find_In_Seg = 0xf000;
unsigned short Find_In_Off = 0x0000;



/* RSDP Structure information */
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



void PrintMEMBlock(unsigned short Print_Block_Off, unsigned char Print_Block_Length)
{
	unsigned int Print_Off = 0x00;
	unsigned char Byte_Content = 0x00;

	printf("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  |  0123456789ABCDEF\n");
	printf("--------------------------------------------------------------------\n");

	do
	{
		/* Print ASCII code in MEM. */
		do
		{
			Byte_Content = GetBYTEContentInMEM(0xf000,  Print_Block_Off+Print_Off);
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
			Byte_Content = GetBYTEContentInMEM(0xf000,  Print_Block_Off+Print_Off);
			if(Print_Off<Print_Block_Length)
				printf("%c", Byte_Content);
			else
				printf(" ");
			Print_Off++;
		}
		while((Print_Off%0x10)!=0);

		printf("\n");
	}
	while(Print_Off!=((Print_Block_Length&0xfff0)+0x10));

}




void main(void)
{
	/* Find string from Seg:Off, for length. */
	unsigned short Find_Length;
	unsigned short Found_Off;
	unsigned int Char_No; /* Ptr to a char in this string. */
	unsigned char Byte_Content; /* Save a BYTE getted from MEM */




	int temp;




	/* I will print MEM's content from Off, for this length, using for() */
	unsigned short Print_In_Off, Print_Length;
	int Print_Off; /* use in for(). */





	Found_Off = 0x0000;

	/* We do a loop to find 'RSD PTR ' in MEM. */
	do
	{
		/* Find first char 'R' */
		Find_Length = 0xffff - Find_In_Off;
		Found_Off = SearchCharInMEM(Find_String[0], Find_In_Seg, Find_In_Off, Find_Length);
	
		if(Found_Off==0xffff)
		{
			printf("#Not find _SM_ ! \n");
			exit(0);
		}
		else
		{
			/* Ptr to found char. */
			Find_In_Off = Found_Off;
			printf("#Find \'%c\', and its Off is %04x .\n", Find_String[0], Found_Off);
		}
	
		/* Find last chars 'SD PTR ' */
		Find_Length = 1;
		for(Char_No=1; Char_No<Find_String_Size; Char_No++)
		{
			Found_Off = SearchCharInMEM(Find_String[Char_No], Find_In_Seg, Find_In_Off, Find_Length);
			if(Found_Off==0xffff)
			{
				printf("#Not find \'%c\' ! \n", Find_String[Char_No]);
				break;
			}
			else
			{
				/* Ptr to found char. */
				Find_In_Off = Found_Off;
				printf("#Find \'%c\', and its Off is %04x .\n", Find_String[Char_No], Found_Off-1);
			}
		}
	}
	while(Found_Off==0xffff);
	/*
	 * Now Found_Off ptr to the [Off(' ')+1], the second SPACE in 'RSD PTR '.
	 * But we want the Off('R'), so SUB it with the size of string.
	 */
	RSDP_STRUCT_OFF = Found_Off - Find_String_Size;


	/* Tell user where we found it. */
	printf("\n\nThis routine will find string \'");
	for(Char_No=0; Char_No<Find_String_Size; Char_No++)
	{
		printf("%c", Find_String[Char_No]);
	}
	printf("\' in Memory in Segment %04x .\n", Find_In_Seg);
	printf("String size: %d\n", Find_String_Size);
	printf("Found it in offset %04x\n", RSDP_STRUCT_OFF);







/*====================================*/
/* Print RSDP structure */
/*====================================*/
	printf("\nPrint RSDP structure.\n");
	PrintMEMBlock(RSDP_STRUCT_OFF, RSDP_STRUCT_LENGTH);







	printf(".\n\nEND of routine.\n");
//	while(bioskey(0)!=0x011b);
}
