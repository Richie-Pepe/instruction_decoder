#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

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
void executeMathInstruction(void);
void executeMemoryInstruction(void);
void executeBranchInstruction(void);
void incrementPC(void);
void loadMemoryFile(void);

unsigned char memory[65536];
unsigned char ACC = 0;
unsigned char IR = 0;
unsigned int MAR = 0;
unsigned int PC = 0;

unsigned int PC_old = 0;
unsigned int PC_inc = 1;

char* filename = "mem_in.txt";

int main(int argc, char* argv[])
{
	// Execution loop. Continue fetching and executing
	// until PC points to a HALT instruction

	loadMemoryFile();

	while (memory[PC] != HALT_OPCODE)
	{
		fetchNextInstruction();
		executeInstruction();
	}

	return 0;
}

void fetchNextInstruction()
{
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
		executeMathInstruction();
	}
	else if ((IR & OPCODE_MASK_MEM) == OPCODE_CMP_MEM)
	{
		//memory instruction
		executeMemoryInstruction();
	}
	else if ((IR & OPCODE_MASK_JMP) == OPCODE_CMP_JMP)
	{
		//branch instruction
		executeBranchInstruction();
	}
	else
	{
		//bad instruction 
	}
}

void executeMathInstruction()
{
		unsigned char function = (IR & MASK_MTH_FNC) >> 4;
        unsigned char dst_code = (IR & MASK_MTH_DST) >> 2;
        unsigned char src_code = IR & MASK_MTH_SRC;
		//int* dst_ptr;
		int src;

		switch (src_code)
		{
		case 0:
			src = (unsigned char) memory[MAR];  // Indirect 
			break;
		case 1:
			src = (unsigned char) ACC;  // Accumulator
			break;
		case 2:
			//Constant
			if (dst_code == 0b10)
			{
				src = (unsigned short int) ((memory[PC_old + 1] & BYTE_MASK) << 8) + (memory[PC_old + 2] & BYTE_MASK);
			}
			else
			{
				src = (unsigned char) memory[PC_old + 1] & BYTE_MASK;
			}

			break;
		case 3:
			src = (unsigned char )memory[((memory[PC_old + 1] & BYTE_MASK) << 8) + (memory[PC_old + 2] & BYTE_MASK)];  // Memory 
			break;
		default:
			src = NULL;
			break;
		}

		//need different pointer type for destination depending on destination
		if (dst_code == 0b10)
		{
			unsigned short int* dst_ptr = &MAR;

			switch (function)
			{
			case 0:  // AND
				*dst_ptr &= src;
				break;
			case 1:  // OR
				*dst_ptr |= src;
				break;
			case 2:  // XOR
				*dst_ptr ^= src;
				break;
			case 3:  // ADD
				*dst_ptr += src;
				break;
			case 4:  // SUB
				*dst_ptr -= src;
				break;
			case 5:  // INC
				*dst_ptr += 1;
				break;
			case 6:  // DEC
				*dst_ptr -= 1;
				break;
			case 7:  // NOT
				*dst_ptr = ~src;
				break;
			default:
				// error
				break;
			}
		}
		else
		{
			unsigned char* dst_ptr;
			switch (dst_code)
			{
			case 0:
				dst_ptr = &memory[MAR];  // Indirect 
				break;
			case 1:
				dst_ptr = (short int*)&ACC;  // Accumulator
				break;
			case 3:
				dst_ptr = &memory[((memory[PC_old + 1] & BYTE_MASK) << 8) + (memory[PC_old + 2] & BYTE_MASK)];  // Memory
				break;
			default:
				dst_ptr = NULL;
				break;
			}


			switch (function)
			{
			case 0:  // AND
				*dst_ptr &= src;
				break;
			case 1:  // OR
				*dst_ptr |= src;
				break;
			case 2:  // XOR
				*dst_ptr ^= src;
				break;
			case 3:  // ADD
				*dst_ptr += src;
				break;
			case 4:  // SUB
				*dst_ptr -= src;
				break;
			case 5:  // INC
				*dst_ptr += 1;
				break;
			case 6:  // DEC
				*dst_ptr -= 1;
				break;
			case 7:  // NOT
				*dst_ptr = ~src;
				break;
			default:
				// error
				break;
			}
		}
}


//STORE - REGISTER TO MEMORY
//LOAD - MEMORY TO REGISTER 
void executeMemoryInstruction()
{
	unsigned char function = (IR & MASK_MEM_FNC) >> 3;
	unsigned char reg_code = (IR & MASK_MEM_RGT) >> 2;
	unsigned char mtd_code = IR & MASK_MEM_MTD;

	//int* reg_ptr;
	//int mtd;

	if (reg_code == 0b0)
	{
		//ACC
		unsigned char* reg_ptr = &ACC;
		unsigned char* mtd_ptr = 0;

		//get method
		switch (mtd_code)
		{
		case 0:
			mtd_ptr = (unsigned char*) &memory[((memory[PC_old + 1] & BYTE_MASK) << 8) + (memory[PC_old + 2] & BYTE_MASK)];
			break;
		case 1:
			mtd_ptr = (unsigned char*)malloc(sizeof(unsigned char));


			*mtd_ptr = (unsigned char)memory[PC_old + 1] & BYTE_MASK;;
			break;
		case 2:
			mtd_ptr = (unsigned char*) &memory[MAR];
			break;
		default:
			break;
		}


		//load or store
		if (function == 0b0)
		{
			//store
			*mtd_ptr = *reg_ptr;
		}
		else if (function == 0b1)
		{
			*reg_ptr = *mtd_ptr;
		}
	}
	else if(reg_code == 0b1)
	{
		//MAR 
		unsigned short int* reg_ptr = &MAR;
		unsigned short int* mtd_ptr = 0; 

		switch (mtd_code)
		{
		case 0:
			mtd_ptr = (unsigned short int*)&memory[((memory[PC_old + 1] & BYTE_MASK) << 8) + (memory[PC_old + 2] & BYTE_MASK)];
			break;
		case 1:
			mtd_ptr = (unsigned short int*)malloc(sizeof(unsigned short int));

			*mtd_ptr = (unsigned short int) ((memory[PC_old + 1] & BYTE_MASK) << 8) + (memory[PC_old + 2] & BYTE_MASK);
			break;
		case 2:
			mtd_ptr = (unsigned short int*)memory[MAR];
			break;
		default:
			break;
		}

		//load or store
		if (function == 0b0)
		{
			//store
			//SPECIAL CASE: due to weird endianess mixing, have to store it in reverse order
			unsigned char* mtd_ptr_chr = (unsigned char*)mtd_ptr;
			*mtd_ptr_chr = (unsigned char) ((*reg_ptr & (BYTE_MASK << 8)) >> 8);
			*(mtd_ptr_chr + 1) = (unsigned char)(*reg_ptr & BYTE_MASK);
			
		}
		else if (function == 0b1)
		{
			*reg_ptr = *mtd_ptr;
		}
	}
}

void executeBranchInstruction()
{
	unsigned int jmp_code = (IR & MASK_JMP_TYP);
	unsigned int jmp_address = (memory[PC_old + 1] << 8) + (memory[PC_old + 2]);

	switch (jmp_code)
	{
	case 0b000:
		//unconditional branch
		PC = jmp_address;
		break;
	case 0b001:
		//branch if acc = 0
		if (ACC == 0)
		{
			PC = jmp_address;
		}
		break;
	case 0b010:
		//branch if acc != 0
		if (ACC != 0)
		{
			PC = jmp_address;
		}
		break;
	case 0b011:
		//branch if acc < 0
		if (ACC < 0)
		{
			PC = jmp_address;
		}
		break;
	case 0b100:
		//branch if acc <= 0
		if (ACC <= 0)
		{
			PC = jmp_address;
		}
		break;
	case 0b101:
		//branch if acc > 0
		if (ACC > 0)
		{
			PC = jmp_address;
		}
		break;
	case 0b110:
		//branch if acc >= 0
		if (ACC >= 0)
		{
			PC = jmp_address;
		}
		break;
	default:
		break;
	}
}

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

	PC_old = PC;
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
	int temp = 0;

	while (!feof(fp) && (loop == 1))
	{
		if (fscanf(fp, "%x", &temp) != 1)
		{
			loop = 0;
		}
		temp &= BYTE_MASK;
		memory[i++] = (char)temp;
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