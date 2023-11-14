#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>


#define HALT_OPCODE 0x19
#define NOP_OPCODE 0x18

//defines for getting opcodes
#define OPCODE_MASK_MTH 0x80
#define OPCODE_MASK_MEM 0xf0
#define OPCODE_MASK_JMP 0xf8
#define OPCODE_CMP_MTH 0x80
#define OPCODE_CMP_MEM 0x00
#define OPCODE_CMP_JMP 0x10

#define BYTE_MASK 0xff

#define MASK_MTH_FNC 0x70
#define MASK_MTH_DST 0x0c
#define MASK_MTH_SRC 0x03
#define MASK_MEM_FNC 0x08
#define MASK_MEM_RGT 0x04
#define MASK_MEM_MTD 0x03
#define MASK_JMP_TYP 0x07


void fetchNextInstruction(void);
void executeInstruction(void);
void incrementPC();
void loadMemoryFile();

unsigned char memory[65536];
unsigned char ACC = 0;
unsigned char IR = 0;
unsigned int MAR = 0;
unsigned int PC = 0;

unsigned int PC_inc = 1;

char* filename = "mem_in.txt";

int main(int argc, char* argv[])
{
	// Execution loop. Continue fetching and executing
	// until PC points to a HALT instruction

	loadMemoryFile();

	/*
	while (memory[PC] != HALT_OPCODE)
	{
		fetchNextInstruction();
		executeInstruction();
	}*/

	return 0;
}

void fetchNextInstruction()
{
	MAR = PC;
	IR = memory[PC]; 

	incrementPC();
}

void executeInstruction()
{
	if (IR == NOP_OPCODE)
	{
		//do nothing
	}
	else if ((IR & OPCODE_MASK_MTH) == OPCODE_CMP_MTH)
	{
		//math instruction
	}
	else if ((IR & OPCODE_MASK_MEM) == OPCODE_CMP_MEM)
	{
		//memory instruction
	}
	else if ((IR & OPCODE_MASK_JMP) == OPCODE_CMP_JMP)
	{
		//branch instruction
	}
	else
	{
		//bad instruction 
	}
}

/*void executeMathInstruction()
{
	int* dst_ptr;
	int src;

	src = 1;

	switch (src)
	{
	default:
		break;
	}
}*/

void incrementPC()
{
	//i think we're gonna see a fair bit of reduntant code
	if (IR == NOP_OPCODE)
	{
		//do nothing
	}
	else if ((IR & OPCODE_MASK_MTH) == OPCODE_CMP_MTH)
	{
		//math instruction
		if ((IR & MASK_MTH_SRC) == 0b11)
		{
			PC_inc += 2;
		}
		else if ((IR & MASK_MTH_SRC) == 0b10)
		{
			if ((IR & MASK_MTH_DST) == 0b1000)
			{
				PC_inc += 2;
			}
			else
			{
				PC_inc += 1;
			}
		}

		if ((IR & MASK_MTH_DST) == 0b1100)
		{
			PC_inc += 2;
		}

	}
	else if ((IR & OPCODE_MASK_MEM) == OPCODE_CMP_MEM)
	{
		if ((IR & MASK_MEM_MTD) == 0b00)
		{
			PC_inc += 2;	//method is 16-bit (2 byte) address
		}
		else if ((IR & MASK_MEM_MTD) == 0b01)
		{
			if ((IR & MASK_MEM_RGT) == 0b100)
			{
				PC_inc += 2;	//working with MAR, 16-bits (2 bytes)
			}
			else
			{
				PC_inc += 1;	//working with ACC, 8-bist (1 byte)
			}
		}
	}
	else if ((IR & OPCODE_MASK_JMP) == OPCODE_CMP_JMP)
	{
		PC_inc += 2;	//branch always followed by 16-bit (2 byte) operand
	}
	else
	{
		//bad instruction
	}

	PC += PC_inc;
	PC_inc = 1;		//make sure to reset increment value
}

void loadMemoryFile()
{
	FILE* fp;

	fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("Memory file could not be loaded.\n");

		return;
	}

	printf("Memory file opened, loading into memory...\n");

	int i = 0;
	int loop = 1;

	while (!feof(fp) && (loop == 1))
	{
		if (fscanf(fp, "%x", memory+i) != 1)
		{
			loop = 0;
		}
		i++;
	}

	fclose(fp);
	
	if (loop == 1)
	{
		printf("Memory file loaded successfully.\n");
	}
	else
	{
		printf("An error occured while loading memory file.");
	}
}