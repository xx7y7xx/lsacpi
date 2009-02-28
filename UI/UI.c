#include <stdio.h>
#include <bios.h>
#include <conio.h>

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
		printf("RSDP table contents.\n");
	else if(CurrentACPITableNo==2)
		printf("RSDT table contents.\n");
	else if(CurrentACPITableNo==3)
		printf("FACP table contents.\n");
	else if(CurrentACPITableNo==4)
		printf("APIC table contents.\n");
}

void main(void)
{
	/*==================*/
	/* Define Variables */
	/*==================*/
	int CurrentACPITableNo = 1;	/* Which ACPI Table currently I was looking at. */
	short PressKey;			/* Which KEY user press. */
	int TableAmount = 4;		/* The amount of all ACPI Tables found in MEM. */

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

}
