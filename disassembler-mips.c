/**************************************************************************
 * Assessment Title:
 *
 *
 * Number of Submitted C Files: 2 (+3 .txt files with example programs)
 *
 * 
 * Date: 26.11.2017
 *
 * 
 * Author: Pavel Dimitrov Dimitrov, Reg no: 201644319
 *
 *
 * Personal Statement: I confirm that this submission is all my own work.
 *
 *          (Signed) Pavel Dimitrov Dimitrov
 *
 * 
 *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MEMORY_SIZE 134217728
#define REGISTER_SIZE 32
#define INSTRUCTION_FILE "mips_program.txt"
#define HF 0 // Halt Flag
#define OF 1 // Overflow Flag

char memory[MEMORY_SIZE][9];
int memory_labels[MEMORY_SIZE / 4];
char function_labels[MEMORY_SIZE / 4];
char registers[32][33];
char pc[33];
char hi[33];
char lo[33];
char flagreg[8] = {"0000000"};

char register_names[32][6] = {"$zero",
						      "$at", 
							  "$v0", "$v1", 
							  "$a0", "$a1", "$a2", "$a3", 
							  "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", 
							  "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 
							  "$t8", "$t9", 
							  "$k0", "$k1", 
							  "$gp", 
							  "$sp", 
							  "$fp", 
							  "$ra"
};

char r_type[44][9] = {"sll", "", "srl", "sra", "sllv", "", // 0 - 5
					  "srlv", "srav", "jr", "jalr", "",    // 6 - 10
					  "", "syscall", "break", "", "",	   // 11 - 15
					  "mfhi", "mthi", "mflo", "mtlo", "",  // 16 - 20
					  "", "", "", "mult", "multu",		   // 21 - 25
					  "div", "divu", "", "", "",		   // 26 - 30
					  "", "add", "addu", "sub", "subu",	   // 31 - 35
					  "and", "or", "xor", "nor", "",	   // 36 - 40
					  "", "slt", "sltu"					   // 41 - 43	
};

char i_type[58][6] = {"", "", "", "", "beq", "bne", 			// 0 - 5
					  "blez", "bgtz", "addi", "addiu", "slti",  // 6 - 10
					  "sltiu", "andi", "ori", "xori", "lui",	// 11 - 15
					  "", "", "", "", "",  						// 16 - 20
					  "", "", "", "", "",		   				// 21 - 25
					  "", "", "", "", "",		   				// 26 - 30
					  "", "lb", "lh", "", "lw",	   				// 31 - 35
					  "lbu", "lhu", "", "", "sb",	   			// 36 - 40
					  "sh", "", "sw", "", "",				   	// 41 - 45	
					  "", "", "", "lwc1", "",					// 46 - 50
					  "", "", "", "", "", 						// 51 - 55
					  "", "swc1" 								// 56 - 57
};

void init_registers(void);
void int_to_bin(int number, int bits, char* destination);
int bin_to_int(char* binary, int bits, int twoscomplement);
void display_memory(void);
void find_labels(void);
void display_assembly(void);
void load_default(void);
void load_user(void);
void load_file(void);
void check_flagreg(void);
void execute_program(void);

void add(int rd, int rs, int rt);
void addi(int rt, int rs, int imm);
void addiu(int rt, int rs, int imm);
void addu(int rd, int rs, int rt);
void and_inst(int rd, int rs, int rt);
void andi(int rt, int rs, int imm);
void lui(int rt, int imm);
void nor(int rd, int rs, int rt);
void or_inst(int rd, int rs, int rt);
void ori(int rt, int rs, int imm);
void slt(int rd, int rs, int rt);
void slti(int rt, int rs, int imm);
void sltiu(int rt, int rs, int imm);
void sltu(int rd, int rs, int rt);
void sub(int rd, int rs, int rt);
void subu(int rd, int rs, int rt);
void xor_inst(int rd, int rs, int rt);
void xori(int rt, int rs, int imm);
void sll(int rd, int rt, int sa);
void sllv(int rd, int rt, int rs);
void sra(int rd, int rt, int sa);
void srav(int rd, int rt, int rs);
void srl(int rd, int rt, int sa);
void srlv(int rd, int rt, int rs);
void div_inst(int rs, int rt);
void divu(int rs, int rt);
void mfhi(int rd);
void mflo(int rd);
void mthi(int rs);
void mtlo(int rs);
void mult(int rs, int rt);
void multu(int rs, int rt);
int beq(int rs, int rt, int offset);
int bgez(int rs, int offset);
int bgezal(int rs, int offset);
int bgtz(int rs, int offset);
int blez(int rs, int offset);
int bltz(int rs, int offset);
int bltzal(int rs, int offset);
int bne(int rs, int rt, int offset);
//void break(...)
void j(int target);
void jal(int target);
void jalr(int rd, int rs);
void jr(int rs);
//void mfc0(...);
//void mtc0(...);
void syscall(void);
void lb(int rt, int offset, int rs);
void lbu(int rt, int offset, int rs);
void lh(int rt, int offset, int rs);
void lhu(int rt, int offset, int rs);
void lw(int rt, int offset, int rs);
void sb(int rt, int offset, int rs);
void sh(int rt, int offset, int rs);
void sw(int rt, int offset, int rs);

/*
	This program takes in one argument at the start and then executes a MIPS program
	with instructions determined by the argument itself.
	If the given argument is -d it loads a default assembly program already hard-coded;
	if the given argument is -c it allows the user to enter each 16-bit field one-by-one; 
	if the given argument is -f it loads the assembly program from a provided file named "ace3_program.txt".
	If the user enters zero or more than one argument, the program terminates with a useful message to the user.
*/
int main (int argc, char *argv[])
{
	if(argc == 1)
	{
		printf("No argument entered ! \nArguments:\n-d Load default program\n-c Load program from user input\n-f Load program from file\n");
		printf("Terminating...\n");
		return 0;
	}
	else
	{
		if(strcmp(argv[1], "-d") == 0)
			load_default();
		else if(strcmp(argv[1], "-c") == 0)
			load_user();
		else if(strcmp(argv[1], "-f") == 0)
			load_file();
		else
		{
			printf("Invalid argument !\nArguments:\n-d Load default program\n-c Load program from user input\n-f Load program from file\n");
			printf("Terminating...\n");
			return 0;
		}
	}
	
	init_registers();
	
	printf("\nMemory before execution of program:\n");
	display_memory();
	
	printf("\nMemory converted to assembly:\n");
	display_assembly();
	
	execute_program();

	printf("\nMemory after execution of program:\n");
	display_memory();
	
	return 0;
}

/*
	Initializes all registers to 0
*/
void init_registers(void)
{
	for(int i = 0 ; i < 32 ; i++)
	{
		strcpy(registers[i], "00000000000000000000000000000000");
	}
	
	strcpy(pc, "00000000000000000000000000000000");
	strcpy(hi, "00000000000000000000000000000000");
	strcpy(lo, "00000000000000000000000000000000");
}

/*
	Converts a decimal integer to binary.
	
	parameters:
	number - the number to be converted
	bits - how many bits we want
	destination - where should the binary number be placed
*/
void int_to_bin(int number, int bits, char *destination)
{
	char binary[bits + 1];

	for (int c = 0 ; c < bits ; c++)
   	{
      	if (number & 1)
         	binary[bits - 1 - c] = 1 + '0';
      	else
        	binary[bits - 1 - c] = 0 + '0';
        	
        number = number >> 1;
   	}
   	
   	binary[bits] = '\0';
   	strcpy(destination, binary);  	
}

/*
	Converts a binary integer to decimal.
	Two's complement included.
	
	parameters:
	binary - the number to be converted
	bits - how many bits is the number
	twoscomplement - is it or is it not
*/
int bin_to_int(char* binary, int bits, int twoscomplement)
{
	int number = 0;
	char temp[bits+1];
	int i = 0;
	
	for (i = 0 ; i < (bits - strlen(binary)) ; i++)
	{
		temp[i] = '0';
	}
	
	temp[i] = '\0';
	strcat(temp, binary);
	
	if(twoscomplement == 1)
		number += (temp[0] - '0') * -1;
	else
		number += (temp[0] - '0');
	
	for (int j = 1 ; j < bits ; j++)
	{
		number = number * 2 + (temp[j] - '0');
	}
	
	return number;
}

/*
	This function displays the contents of the whole memory 
	in an orderly fashion.
*/
void display_memory(void)
{
	printf("Memory Loc | Memory contents\n");
	
	for (int counter = 0 ; counter < MEMORY_SIZE ; counter++)
	{
		if (!strcmp(memory[counter] , "") == 0)
		{
			printf("%10d - %s\n", counter, memory[counter]);
		}
	}
}

/*
	This function reads the whole memory and finds any labels
	in the program and marks them so that they can be displayed later.
*/
void find_labels(void)
{
	char temp_instruction[33];
	char temp_opcode[6];
	char temp_immediate[17];
	char temp_target_address[27];
	
	int temp_int_opcode;
	int temp_int_immediate;
	int temp_int_target_address;
	int temp_count = 1;
	
	for (int counter = 0 ; counter < MEMORY_SIZE ; counter+=4)
	{
		if(strcmp(memory[counter], "") == 0)
			continue;

		strcpy(temp_instruction, memory[counter]);
		strcat(temp_instruction, memory[counter]+9);
		strcat(temp_instruction, memory[counter]+18);
		strcat(temp_instruction, memory[counter]+27);
		temp_instruction[32] = '\0';
		
		memcpy(temp_opcode, temp_instruction, 6);
		temp_opcode[7] = '\0';
		temp_int_opcode = bin_to_int(temp_opcode, 6, 0);
		
		if (temp_int_opcode == 2 || temp_int_opcode == 3) // J type
		{
			memcpy(temp_target_address, temp_instruction+6, 26);
			temp_target_address[26] = '\0';
			temp_int_target_address = bin_to_int(temp_target_address, 26, 0);
			
			memory_labels[temp_int_target_address / 4] = temp_count++;
			
		}
		else if (temp_int_opcode != 0)// I type
		{			
			memcpy(temp_immediate, temp_instruction+16, 16);
			temp_immediate[16] = '\0';
			temp_int_immediate = bin_to_int(temp_immediate, 16, 1);
			
			if (temp_int_opcode == 1) // bgez, bltz
				memory_labels[(counter / 4 ) + temp_int_immediate] = temp_count++;
			else if (temp_int_opcode == 4 || temp_int_opcode == 5) // beq, bne
				memory_labels[(counter / 4 ) + temp_int_immediate] = temp_count++;
			else if (temp_int_opcode == 6 || temp_int_opcode == 7) // bgtz, blez
				memory_labels[(counter / 4 ) + temp_int_immediate] = temp_count++;
		}
	}
}

/*
	This function translates all of the instructions in memory
	to assembly instructions and then displays them on screen
	in an orderly fashion.
*/
void display_assembly(void)
{
	find_labels();
	
	char temp_instruction[33];
	char temp_opcode[7];
	char temp_rs[6];
	char temp_rt[6];
	char temp_rd[6];
	char temp_shamt[6];
	char temp_funct[7];	
	char temp_immediate[17];
	char temp_target_address[27];
	char temp_label[16];
	
	int temp_int_opcode;
	int temp_int_rs;
	int temp_int_rt;
	int temp_int_rd;
	int temp_int_shamt;
	int temp_int_funct;
	int temp_int_immediate;
	int temp_int_target_address;
	int label_count = 1;
	
	printf("Memory Loc | Translated Memory\n");
	
	for (int counter = 0 ; counter < MEMORY_SIZE ; counter+=4)
	{
		if(strcmp(memory[counter], "") == 0)
			continue;
			
		printf("%10d - ", counter);

		strcpy(temp_instruction, memory[counter]);
		strcat(temp_instruction, memory[counter]+9);
		strcat(temp_instruction, memory[counter]+18);
		strcat(temp_instruction, memory[counter]+27);
		temp_instruction[32] = '\0';
		
		memcpy(temp_opcode, temp_instruction, 6);
		temp_opcode[6] = '\0';
		temp_int_opcode = bin_to_int(temp_opcode, 6, 0);
		
		if(memory_labels[counter / 4])
		{
			sprintf(temp_label, "label%d:", memory_labels[counter / 4]);
			printf("%-15s ", temp_label);
		}
		else
			printf("                ");

		if (temp_int_opcode == 0) // R type
		{
			memcpy(temp_rs, temp_instruction+6, 5);
			temp_rs[5] = '\0';
			temp_int_rs = bin_to_int(temp_rs, 5, 0);
			
			memcpy(temp_rt, temp_instruction+11, 5);
			temp_rt[5] = '\0';
			temp_int_rt = bin_to_int(temp_rt, 5, 0);
			
			memcpy(temp_rd, temp_instruction+16, 5);
			temp_rd[5] = '\0';
			temp_int_rd = bin_to_int(temp_rd, 5, 0);
			
			memcpy(temp_shamt, temp_instruction+21, 5);
			temp_shamt[5] = '\0';
			temp_int_shamt = bin_to_int(temp_shamt, 5, 0);
			
			memcpy(temp_funct, temp_instruction+26, 6);
			temp_funct[6] = '\0';
			temp_int_funct = bin_to_int(temp_funct, 6, 0);
			
			if(temp_int_funct == 12 || temp_int_funct == 13) // syscall, break
				printf("%s\n", r_type[temp_int_funct]);
			else if (temp_int_funct >= 24 && temp_int_funct <= 27) // mult, multu, div, divu
				printf("%s %s %s\n", r_type[temp_int_funct], register_names[temp_int_rs], register_names[temp_int_rt]);
			else if (temp_int_funct == 16 || temp_int_funct == 18) // mfhi, mflo
				printf("%s %s\n", r_type[temp_int_funct], register_names[temp_int_rd]);
			else if (temp_int_funct == 8 || temp_int_funct == 17 || temp_int_funct == 19) // jr, mthi, mtlo
				printf("%s %s\n", r_type[temp_int_funct], register_names[temp_int_rs]);
			else if (temp_int_funct == 0 || temp_int_funct == 2 || temp_int_funct == 3) // sll, sra, srl
				printf("%s %s %s %s\n", r_type[temp_int_funct], register_names[temp_int_rd], register_names[temp_int_rt], register_names[temp_int_shamt]);
			else if (temp_int_funct == 4 || temp_int_funct == 6 || temp_int_funct == 7) // sllv, srav, srlv
				printf("%s %s %s %s\n", r_type[temp_int_funct], register_names[temp_int_rd], register_names[temp_int_rt], register_names[temp_int_rs]);
			else if (temp_int_funct == 9) // jalr
				printf("%s %s %s\n", r_type[temp_int_funct], register_names[temp_int_rd], register_names[temp_int_rs]);
			else if (temp_int_funct >= 32)
				printf("%s %s %s %s\n", r_type[temp_int_funct], register_names[temp_int_rd], register_names[temp_int_rs], register_names[temp_int_rt]);
		}
		else if (temp_int_opcode == 2 || temp_int_opcode == 3) // J type
		{
			memcpy(temp_target_address, temp_instruction+6, 26);
			temp_target_address[26] = '\0';
			temp_int_target_address = bin_to_int(temp_target_address, 26, 0);
			
			printf("%s label%d\n", temp_int_opcode == 2 ? "j" : "jal", label_count++);
		}
		else // I type
		{
			memcpy(temp_rs, temp_instruction+6, 5);
			temp_rs[5] = '\0';
			temp_int_rs = bin_to_int(temp_rs, 5, 0);
			
			memcpy(temp_rt, temp_instruction+11, 5);
			temp_rt[5] = '\0';
			temp_int_rt = bin_to_int(temp_rt, 5, 0);
			
			memcpy(temp_immediate, temp_instruction+16, 16);
			temp_immediate[16] = '\0';
			temp_int_immediate = bin_to_int(temp_immediate, 16, 1);
			
			if (temp_int_opcode == 1) // bgez, bltz
				printf("%s %s label%d\n", temp_int_rt == 1 ? "bgez" : "bltz", register_names[temp_int_rs], label_count++); // label
			else if (temp_int_opcode == 4 || temp_int_opcode == 5) // beq, bne
				printf("%s %s %s label%d\n", i_type[temp_int_opcode], register_names[temp_int_rs], register_names[temp_int_rt], label_count++); // label	
			else if (temp_int_opcode == 6 || temp_int_opcode == 7) // bgtz, blez
				printf("%s %s label%d\n", i_type[temp_int_opcode], register_names[temp_int_rs], label_count++); // label
			else if (temp_int_opcode >= 8 && temp_int_opcode <= 14) // addi, addiu, andi, ori, slti, sltiu, xori
			{
			 	if(temp_int_opcode == 9 || temp_int_opcode == 11)
			 		temp_int_immediate = bin_to_int(temp_immediate, 16, 0);
			 	
				printf("%s %s %s %d\n", i_type[temp_int_opcode], register_names[temp_int_rt], register_names[temp_int_rs], temp_int_immediate);
			}
			else if (temp_int_opcode == 15) // lui
				printf("%s %s %d\n", i_type[temp_int_opcode], register_names[temp_int_rt], temp_int_immediate);		
			else if (temp_int_opcode >= 32)	
				printf("%s %s %d(%s)\n", i_type[temp_int_opcode], register_names[temp_int_rt], temp_int_immediate, register_names[temp_int_rs]);
		}
	}
}

/*
	Loads a default program in memory.
*/
void load_default(void)
{
	strcpy(memory[0], "00100000"); // addi $v0 $zero 5
	strcpy(memory[1], "00000010");
	strcpy(memory[2], "00000000");
	strcpy(memory[3], "00000101");
	
	strcpy(memory[4], "00000000"); // syscall
	strcpy(memory[5], "00000000");
	strcpy(memory[6], "00000000");
	strcpy(memory[7], "00001100");
	
	strcpy(memory[8], "00000000"); // add $t0 $v0 $zero
	strcpy(memory[9], "01000000");
	strcpy(memory[10], "01000000");
	strcpy(memory[11], "00100000");
	
	strcpy(memory[12], "00000000"); // add $t1 &zero $zero
	strcpy(memory[13], "00000000");
	strcpy(memory[14], "01001000");
	strcpy(memory[15], "00100000");
	
	strcpy(memory[16], "00000001"); // add $t2 &t2 $t1
	strcpy(memory[17], "01001001");
	strcpy(memory[18], "01010000");
	strcpy(memory[19], "00100000");
	
	strcpy(memory[20], "00100001"); // addi $t1 &t1 1
	strcpy(memory[21], "00101001");
	strcpy(memory[22], "00000000");
	strcpy(memory[23], "00000001");
	
	strcpy(memory[24], "00010101"); // bne $t0 &t1 loop
	strcpy(memory[25], "00001001");
	strcpy(memory[26], "11111111");
	strcpy(memory[27], "11111110");
	
	
	strcpy(memory[80], "00000001"); // add $a0 $t2 $zero
	strcpy(memory[81], "01000000");
	strcpy(memory[82], "00100000");
	strcpy(memory[83], "00100000");
	
	strcpy(memory[84], "00100000"); // addi $v0 &zero 1
	strcpy(memory[85], "00000010");
	strcpy(memory[86], "00000000");
	strcpy(memory[87], "00000001");
	
	strcpy(memory[88], "00000000"); // syscall
	strcpy(memory[89], "00000000");
	strcpy(memory[90], "00000000");
	strcpy(memory[91], "00001100");
	
	strcpy(memory[92], "00100000"); // addi $v0 $zero 10
	strcpy(memory[93], "00000010");
	strcpy(memory[94], "00000000");
	strcpy(memory[95], "00001010");
	
	strcpy(memory[96], "00000000"); // syscall
	strcpy(memory[97], "00000000");
	strcpy(memory[98], "00000000");
	strcpy(memory[99], "00001100");
	
	printf("Default program loaded !\n");	
}

/*
	Loads a program from the user input.
	With Error checking.
*/
void load_user(void)
{
	int ROW_POINTER = 0;
	int COLUMN_POINTER = 0;
	char input[10];
	int flag = 0;
	
	while(1)
	{
		printf("Enter (\'stop\' to end) memory content (8 bits) at memory location %d: ", ROW_POINTER);
		scanf("%s", input);
		
		if (!strcmp(input, "stop"))
			break;
			
		if(ROW_POINTER >= MEMORY_SIZE)
		{
			printf("Not enough memory !\n");
			continue;
		}
			
		if (strlen(input) < 8)
		{
			printf("Instruction too short !\n");
			continue;
		}
			
		if (strlen(input) > 8)
		{
			printf("Instruction too long !\n");
			continue;
		}
		
		flag = 0;
			
		for (COLUMN_POINTER = 0 ; COLUMN_POINTER < 8 ; COLUMN_POINTER++)
		{
			if (input[COLUMN_POINTER] == '0' || input[COLUMN_POINTER] == '1')
				memory[ROW_POINTER][COLUMN_POINTER] = input[COLUMN_POINTER];
			else
			{
				printf("Invalid character at pos %d !\n", (COLUMN_POINTER + 1));
				flag = 1;
				break;
			}
		}
		
		if(flag)
			continue;

		ROW_POINTER++;
	}
	
	printf("Reading user input completed !\n%d line%s loaded\n", ROW_POINTER, ROW_POINTER == 1 ? "" : "s");	
}

/*
	Opens new file and loops through each character in each line
	and saves it into memory. If line is not of length 16 then returns
	an error message. It also checks for memory overflow and
	if the line contains any invalid characters.
*/
void load_file(void)
{
	FILE *file_ptr;
	file_ptr = fopen(INSTRUCTION_FILE ,"r");

	if(file_ptr == NULL)
	{
		printf("Cannot find file at - %s\n", INSTRUCTION_FILE);
		printf("Terminating...\n");
		exit(EXIT_FAILURE);
	}

	int ROW_POINTER = 0;
	int COLUMN_POINTER = 0;
	char c;

	while((c = fgetc(file_ptr)) != EOF)
	{
		if(c == '\r')
			continue;
			
		if (c == '\n')
        {
        	if(COLUMN_POINTER != 8)
        	{
        		printf("Error reading program!\nLine %d\nPos %d\nInstruction too short !\n", (ROW_POINTER + 1), (COLUMN_POINTER + 1));
        		printf("Terminating...\n");
				exit(EXIT_FAILURE);
			}
        	
        	ROW_POINTER++;
        	COLUMN_POINTER = 0;
		}
		else if(ROW_POINTER < MEMORY_SIZE)
		{
			if(COLUMN_POINTER < 8)
			{
				if (c == '0' || c == '1')
					memory[ROW_POINTER][COLUMN_POINTER++] = c;
				else
				{
					printf("Error reading program!\nLine %d\nPos %d\nInvalid character !\n", (ROW_POINTER + 1), (COLUMN_POINTER + 1));
					printf("Terminating...\n");
    				exit(EXIT_FAILURE);
				}
			}
			else
			{
				printf("Error reading program!\nLine %d\nPos %d\nInstruction too long !\n", (ROW_POINTER + 1), (COLUMN_POINTER + 1));
				printf("Terminating...\n");
    			exit(EXIT_FAILURE);
			}       			
		}
		else
		{
			printf("Error reading program!\nLine %d\nPos %d\nNot enough memory !\n", (ROW_POINTER + 1), (COLUMN_POINTER + 1));
			printf("Terminating...\n");
    		exit(EXIT_FAILURE);
		}
	}
	
	printf("Reading from file \"%s\" completed !\n%d line%s loaded\n", INSTRUCTION_FILE, (ROW_POINTER + 1), (ROW_POINTER + 1) == 1 ? "" : "s");	
	fclose(file_ptr);
}

/*
	Calculates the flag registers in the flagreg variable.
*/
void check_flagreg(void)
{		
	if (flagreg[OF] == '1')
	{
		printf("Overflow detected at memory location: %d\n", (bin_to_int(pc, 32, 0) - 1));
		printf("Terminating...\n");
    	exit(EXIT_FAILURE);
	}
}

/*
	Loops through the memory, reads instruction on memory location PC,
	translates the instruction and executes it. Terminates when PC reaches
	end of memory or when HALT flag is raised.
*/
void execute_program(void)
{
	char temp_instruction[33];
	char temp_opcode[7];
	char temp_rs[6];
	char temp_rt[6];
	char temp_rd[6];
	char temp_shamt[6];
	char temp_funct[7];	
	char temp_immediate[17];
	char temp_target_address[27];
	char temp_label[16];
	
	int temp_int_opcode;
	int temp_int_rs;
	int temp_int_rt;
	int temp_int_rd;
	int temp_int_shamt;
	int temp_int_funct;
	int temp_int_immediate;
	int temp_int_target_address;
	int temp_int_pc;
	
	//int_to_bin(0, 32, pc); // Set PC = 0
	
	while(flagreg[HF] == '0') // While HF is not 1
	{
		check_flagreg();
		temp_int_pc = bin_to_int(pc, 32, 0);
		
		if(strcmp(memory[temp_int_pc], "") == 0)
		{
			int_to_bin((bin_to_int(pc, 32, 0) + 4), 32, pc); // PC = PC + 1
			continue;
		}

		if(temp_int_pc == MEMORY_SIZE) // Check if we have reached end of memory
			break;

		strcpy(temp_instruction, memory[temp_int_pc]);
		strcat(temp_instruction, memory[temp_int_pc]+9);
		strcat(temp_instruction, memory[temp_int_pc]+18);
		strcat(temp_instruction, memory[temp_int_pc]+27);
		temp_instruction[32] = '\0';
		
		memcpy(temp_opcode, temp_instruction, 6);
		temp_opcode[6] = '\0';
		temp_int_opcode = bin_to_int(temp_opcode, 6, 0);
		
		if (temp_int_opcode == 0) // R type
		{
			memcpy(temp_rs, temp_instruction+6, 5);
			temp_rs[5] = '\0';
			temp_int_rs = bin_to_int(temp_rs, 5, 0);
			
			memcpy(temp_rt, temp_instruction+11, 5);
			temp_rt[5] = '\0';
			temp_int_rt = bin_to_int(temp_rt, 5, 0);
			
			memcpy(temp_rd, temp_instruction+16, 5);
			temp_rd[5] = '\0';
			temp_int_rd = bin_to_int(temp_rd, 5, 0);
			
			memcpy(temp_shamt, temp_instruction+21, 5);
			temp_shamt[5] = '\0';
			temp_int_shamt = bin_to_int(temp_shamt, 5, 0);
			
			memcpy(temp_funct, temp_instruction+26, 6);
			temp_funct[6] = '\0';
			temp_int_funct = bin_to_int(temp_funct, 6, 0);
			
			switch(temp_int_funct)
			{
				case 0: // sll
				{
					sll(temp_int_rd, temp_int_rt, temp_int_shamt);
					break;
				}
				case 2: // srl
				{
					srl(temp_int_rd, temp_int_rt, temp_int_shamt);
					break;
				}
				case 3: // sra
				{
					sra(temp_int_rd, temp_int_rt, temp_int_shamt);
					break;
				}
				case 4: // sllv
				{
					sllv(temp_int_rd, temp_int_rt, temp_int_rs);
					break;
				}
				case 6: // srlv
				{
					srlv(temp_int_rd, temp_int_rt, temp_int_rs);
					break;
				}
				case 7: // srav
				{
					srav(temp_int_rd, temp_int_rt, temp_int_rs);
					break;
				}
				case 8: // jr
				{
					jr(temp_int_rs);
					continue;
				}
				case 9: // jalr
				{
					jalr(temp_int_rd, temp_int_rs);
					continue;
				}
				case 12: // syscall
				{
					syscall();
					break;
				}
				case 13: // break
				{
					// To be continued...
					break;
				}
				case 16: // mfhi
				{
					mfhi(temp_int_rd);
					break;
				}
				case 17: // mthi
				{
					mthi(temp_int_rs);
					break;
				}
				case 18: // mflo
				{
					mflo(temp_int_rd);
					break;
				}
				case 19: // mtlo
				{
					mtlo(temp_int_rs);
					break;
				}
				case 24: // mult
				{
					mult(temp_int_rs, temp_int_rt);
					break;
				}
				case 25: // multu
				{
					multu(temp_int_rs, temp_int_rt);
					break;
				}
				case 26: // div
				{
					div(temp_int_rs, temp_int_rt);
					break;
				}
				case 27: // divu
				{
					divu(temp_int_rs, temp_int_rt);
					break;
				}
				case 32: // add
				{
					add(temp_int_rd, temp_int_rs, temp_int_rt);
					break;
				}
				case 33: // addu
				{
					addu(temp_int_rd, temp_int_rs, temp_int_rt);
					break;
				}
				case 34: // sub
				{
					sub(temp_int_rd, temp_int_rs, temp_int_rt);
					break;
				}
				case 35: // subu
				{
					subu(temp_int_rd, temp_int_rs, temp_int_rt);
					break;
				}
				case 36: // and
				{
					and_inst(temp_int_rd, temp_int_rs, temp_int_rt);
					break;
				}
				case 37: // or
				{
					or_inst(temp_int_rd, temp_int_rs, temp_int_rt);
					break;
				}
				case 38: // xor
				{
					xor_inst(temp_int_rd, temp_int_rs, temp_int_rt);
					break;
				}
				case 39: // nor
				{
					nor(temp_int_rd, temp_int_rs, temp_int_rt);
					break;
				}
				case 42: // slt
				{
					slt(temp_int_rd, temp_int_rs, temp_int_rt);
					break;
				}
				case 43: // sltu
				{
					sltu(temp_int_rd, temp_int_rs, temp_int_rt);
					break;
				}
				default:
				{
					printf("Something went wrong.\n");
				}
			}
		}
		else if (temp_int_opcode == 2 || temp_int_opcode == 3) // J type
		{
			memcpy(temp_target_address, temp_instruction+6, 26);
			temp_target_address[26] = '\0';
			temp_int_target_address = bin_to_int(temp_target_address, 26, 0);
			
			if(temp_int_opcode == 3)
				jal(temp_int_target_address);
			else
				j(temp_int_target_address);
				
			continue;
		}
		else // I type
		{
			memcpy(temp_rs, temp_instruction+6, 5);
			temp_rs[5] = '\0';
			temp_int_rs = bin_to_int(temp_rs, 5, 0);
			
			memcpy(temp_rt, temp_instruction+11, 5);
			temp_rt[5] = '\0';
			temp_int_rt = bin_to_int(temp_rt, 5, 0);
			
			memcpy(temp_immediate, temp_instruction+16, 16);
			temp_immediate[16] = '\0';
			temp_int_immediate = bin_to_int(temp_immediate, 16, 1);
			
			switch(temp_int_opcode)
			{
				case 1: // bgez, bltz
				{
					if (temp_int_rt == 0)
					{
						if (bltz(temp_int_rs, temp_int_immediate))
							continue;
					}
					else if (temp_int_rt == 1)
					{
						if (bgez(temp_int_rs, temp_int_immediate))
							continue;
					}
					else if (temp_int_rt == 16)
					{
						if (bltzal(temp_int_rs, temp_int_immediate))
							continue;
					}
					else if (temp_int_rt == 17)
					{
						if (bgezal(temp_int_rs, temp_int_immediate))
							continue;
					}

					break;
				}
				case 4: // beq
				{
					if (beq(temp_int_rs, temp_int_rt, temp_int_immediate))
						continue;
				}
				case 5: // bne
				{
					if (bne(temp_int_rs, temp_int_rt, temp_int_immediate))
						continue;
					
					break;
				}
				case 6: // blez
				{
					if (blez(temp_int_rs, temp_int_immediate))
						continue;
				}
				case 7: // bgtz
				{
					if (bgtz(temp_int_rs, temp_int_immediate))
						continue;
				}
				case 8: // addi
				{
					addi(temp_int_rt, temp_int_rs, temp_int_immediate);
					break;
				}
				case 9: // addiu
				{
					addiu(temp_int_rt, temp_int_rs, temp_int_immediate);
					break;
				}
				case 10: // slti
				{
					slti(temp_int_rt, temp_int_rs, temp_int_immediate);
					break;
				}
				case 11: // sltiu
				{
					temp_int_immediate = bin_to_int(temp_immediate, 16, 0);
					slti(temp_int_rt, temp_int_rs, temp_int_immediate);
					break;
				}
				case 12: // andi
				{
					temp_int_immediate = bin_to_int(temp_immediate, 16, 0);
					andi(temp_int_rt, temp_int_rs, temp_int_immediate);
					break;
				}
				case 13: // ori
				{
					temp_int_immediate = bin_to_int(temp_immediate, 16, 0);
					ori(temp_int_rt, temp_int_rs, temp_int_immediate);
					break;
				}
				case 14: // xori
				{
					temp_int_immediate = bin_to_int(temp_immediate, 16, 0);
					xori(temp_int_rt, temp_int_rs, temp_int_immediate);
					break;
				}
				case 15: // lui
				{
					lui(temp_int_rt, temp_int_immediate);
					break;
				}
				case 32: // lb
				{
					lb(temp_int_rt, temp_int_immediate, temp_int_rs);
					break;
				}
				case 33: // lh
				{
					lh(temp_int_rt, temp_int_immediate, temp_int_rs);
					break;
				}
				case 35: // lw
				{
					lw(temp_int_rt, temp_int_immediate, temp_int_rs);
					break;
				}
				case 36: // lbu
				{
					lbu(temp_int_rt, temp_int_immediate, temp_int_rs);
					break;
				}
				case 37: // lhu
				{
					lhu(temp_int_rt, temp_int_immediate, temp_int_rs);
					break;
				}
				case 40: // sb
				{
					sb(temp_int_rt, temp_int_immediate, temp_int_rs);
					break;
				}
				case 41: // sh
				{
					sh(temp_int_rt, temp_int_immediate, temp_int_rs);
					break;
				}
				case 43: // sw
				{
					sw(temp_int_rt, temp_int_immediate, temp_int_rs);
					break;
				}
				case 49: // lwc1
				{
					// To be continued...
					break;
				}
				case 57: // swc1
				{
					// To be continued...
					break;
				}
				default:
				{
					printf("Something went wrong.\n");
				}
			}
		}

		int_to_bin((bin_to_int(pc, 32, 0) + 4), 32, pc); // PC = PC + 1
	}
}

/*
	Function implementing the 'add' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void add(int rd, int rs, int rt)
{
	int temp_rs = bin_to_int(registers[rs], 32, 1);
	int temp_rt = bin_to_int(registers[rt], 32, 1);
	int temp_result = temp_rs + temp_rt;
	
	if ((temp_rs > 0 && temp_rt > 0 && temp_result < 0) || (temp_rs < 0 && temp_rt < 0 && temp_result > 0))
	{
		flagreg[OF] = '1';
	}
	
	int_to_bin(temp_result, 32, registers[rd]);
}

/*
	Function implementing the 'addi' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int imm - value of the immediate
*/
void addi(int rt, int rs, int imm)
{
	int temp_rs = bin_to_int(registers[rs], 32, 1);
	int temp_result = temp_rs + imm;
	
	if ((temp_rs > 0 && imm > 0 && temp_result < 0) || (temp_rs < 0 && imm < 0 && temp_result > 0))
	{
		flagreg[OF] = '1';
	}
	
	int_to_bin(temp_result, 32, registers[rt]);
}

/*
	Function implementing the 'addiu' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int imm - value of the immediate
*/
void addiu(int rt, int rs, int imm)
{
	int temp_result = bin_to_int(registers[rs], 32, 1) + imm;
	
	int_to_bin(temp_result, 32, registers[rt]);
}

/*
	Function implementing the 'addu' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void addu(int rd, int rs, int rt)
{
	int temp_result = bin_to_int(registers[rs], 32, 1) + bin_to_int(registers[rt], 32, 1);
	
	int_to_bin(temp_result, 32, registers[rd]);
}

/*
	Function implementing the 'and' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void and_inst(int rd, int rs, int rt)
{
	char temp_reg1[REGISTER_SIZE + 1];
	char temp_reg2[REGISTER_SIZE + 1];
	
	strcpy(temp_reg1, registers[rs]);
	strcpy(temp_reg2, registers[rt]);
	registers[rd][0] = '\0';
	
	for (int i = 0 ; i < REGISTER_SIZE ; i++)
	{
		if((temp_reg1[i] == '1') && (temp_reg2[i] == '1'))
		{
			strcat(registers[rd], "1");
		}
		else
		{
			strcat(registers[rd], "0");
		}
	}
}

/*
	Function implementing the 'andi' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int imm - value of the immediate
*/
void andi(int rt, int rs, int imm)
{
	char temp_reg1[REGISTER_SIZE + 1];
	char temp_reg2[REGISTER_SIZE + 1];
	char temp[16];
	
	strcpy(temp_reg1, registers[rs]);

	strcpy(temp_reg2, "0000000000000000");
	int_to_bin(imm, 16, temp);
	memcpy(temp_reg2+16, temp, 16);
	temp_reg2[32] = '\0';
	
	registers[rt][0] = '\0';
	
	for (int i = 0 ; i < REGISTER_SIZE ; i++)
	{
		if((temp_reg1[i] == '1') && (temp_reg2[i] == '1'))
		{
			strcat(registers[rt], "1");
		}
		else
		{
			strcat(registers[rt], "0");
		}
	}
}

/*
	Function implementing the 'lui' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int imm - value of the immediate
*/
void lui(int rt, int imm)
{
	char temp[16];
	
	int_to_bin(imm, 16, temp);

	memcpy(registers[rt], temp, 16);
	registers[rt][16] = '\0';
	strcat(registers[rt], "0000000000000000");
	registers[rt][32] = '\0';	
}

/*
	Function implementing the 'nor' instruction.
	
    parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void nor(int rd, int rs, int rt)
{
	char temp_reg1[REGISTER_SIZE + 1];
	char temp_reg2[REGISTER_SIZE + 1];	
	
	strcpy(temp_reg1, registers[rs]);
	strcpy(temp_reg2, registers[rt]);
	registers[rd][0] = '\0';
	
	for (int i = 0; i < REGISTER_SIZE ; i++)
	{
		if ((temp_reg1[i] == '0') && (temp_reg2[i] == '0'))
		{
			strcat(registers[rd], "1");
		}
		else
		{
			strcat(registers[rd], "0");
		}
	}
}

/*
	Function implementing the 'or' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void or_inst(int rd, int rs, int rt)
{	
	char temp_reg1[REGISTER_SIZE + 1];
	char temp_reg2[REGISTER_SIZE + 1];
	
	strcpy(temp_reg1, registers[rs]);
	strcpy(temp_reg2, registers[rt]);

	registers[rd][0] = '\0';
	
	for (int i = 0 ; i < REGISTER_SIZE ; i++)
	{
		if((temp_reg1[i] == '0') && (temp_reg2[i] == '0'))
		{
			strcat(registers[rd], "0");
		}
		else
		{
			strcat(registers[rd], "1");
		}
	}
}

/*
	Function implementing the 'ori' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int imm - value of the immediate
*/
void ori(int rt, int rs, int imm)
{
	char temp_reg1[REGISTER_SIZE + 1];
	char temp_reg2[REGISTER_SIZE + 1];
	char temp[16];
	
	strcpy(temp_reg1, registers[rs]);

	strcpy(temp_reg2, "0000000000000000");
	int_to_bin(imm, 16, temp);
	memcpy(temp_reg2+16, temp, 16);
	temp_reg2[32] = '\0';
	
	registers[rt][0] = '\0';
	
	for (int i = 0 ; i < REGISTER_SIZE ; i++)
	{
		if((temp_reg1[i] == '0') && (temp_reg2[i] == '0'))
		{
			strcat(registers[rt], "0");
		}
		else
		{
			strcat(registers[rt], "1");
		}
	}
}

/*
	Function implementing the 'slt' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void slt(int rd, int rs, int rt)
{
	int temp_result1 = bin_to_int(registers[rs], 32, 1);
	int temp_result2 = bin_to_int(registers[rt], 32, 1);

	if(temp_result1 < temp_result2)
	{
		strcpy(registers[rd], "00000000000000000000000000000001");
	}
	else
	{
		strcpy(registers[rd], registers[0]);
	}
}

/*
	Function implementing the 'slti' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int imm - value of the immediate
*/
void slti(int rt, int rs, int imm)
{
	int temp_result1 = bin_to_int(registers[rs], 32, 1);

	if(temp_result1 < imm)
	{
		strcpy(registers[rt], "00000000000000000000000000000001");
	}
	else
	{
		strcpy(registers[rt], registers[0]);
	}
}

/*
	Function implementing the 'sltiu' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int imm - value of the immediate
*/
void sltiu(int rt, int rs, int imm)
{
	int temp_result1 = bin_to_int(registers[rs], 32, 0);

	if(temp_result1 < imm)
	{
		strcpy(registers[rt], "00000000000000000000000000000001");
	}
	else
	{
		strcpy(registers[rt], registers[0]);
	}
}

/*
	Function implementing the 'sltu' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - target register
*/
void sltu(int rd, int rs, int rt)
{
	int temp_result1 = bin_to_int(registers[rs], 32, 0);
	int temp_result2 = bin_to_int(registers[rt], 32, 0);

	if(temp_result1 < temp_result2)
	{
		strcpy(registers[rd], "00000000000000000000000000000001");
	}
	else
	{
		strcpy(registers[rd], registers[0]);
	}
}

/*
	Function implementing the 'sub' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void sub(int rd, int rs, int rt)
{
	int temp_rs = bin_to_int(registers[rs], 32, 1);
	int temp_rt = bin_to_int(registers[rt], 32, 1);
	int temp_result = temp_rs - temp_rt;
	
	if ((temp_rs > 0 && temp_rt < 0 && temp_result < 0) || (temp_rs < 0 && temp_rt > 0 && temp_result > 0))
	{
		flagreg[OF] = '1';
	}
	
	int_to_bin(temp_result, 32, registers[rd]);
}

/*
	Function implementing the 'subu' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void subu(int rd, int rs, int rt)
{
	int temp_result = bin_to_int(registers[rs], 32, 1) - bin_to_int(registers[rt], 32, 1);

	int_to_bin(temp_result, 32, registers[rd]);
}

/*
	Function implementing the 'xor' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void xor_inst(int rd, int rs, int rt)
{
	char temp_reg1[REGISTER_SIZE + 1];
	char temp_reg2[REGISTER_SIZE + 1];
	
	strcpy(temp_reg1, registers[rs]);
	strcpy(temp_reg2, registers[rt]);

	registers[rd][0] = '\0';
	
	for (int i = 0 ; i < REGISTER_SIZE ; i++)
	{
		if((temp_reg1[i] == '0' && temp_reg2[i] == '0') || (temp_reg1[i] == '1' && temp_reg2[i] == '1'))
		{
			strcat(registers[rd], "0");
		}
		else
		{
			strcat(registers[rd], "1");
		}
	}
}

/*
	Function implementing the 'xori' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int imm - value of the immediate
*/
void xori(int rt, int rs, int imm)
{
	char temp_reg1[REGISTER_SIZE + 1];
	char temp_reg2[REGISTER_SIZE + 1];
	char temp[16];
	
	strcpy(temp_reg1, registers[rs]);

	strcpy(temp_reg2, "0000000000000000");
	int_to_bin(imm, 16, temp);
	memcpy(temp_reg2+16, temp, 16);
	temp_reg2[32] = '\0';
	
	registers[rt][0] = '\0';
	
	for (int i = 0 ; i < REGISTER_SIZE ; i++)
	{
		if((temp_reg1[i] == '0' && temp_reg2[i] == '0') || (temp_reg1[i] == '1' && temp_reg2[i] == '1'))
		{
			strcat(registers[rt], "0");
		}
		else
		{
			strcat(registers[rt], "1");
		}
	}
}

/*
	Function implementing the 'sll' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int sa - shift amount
	int rt - integer representation of the target register
*/
void sll(int rd, int rt, int sa)
{
	char temp_reg1[REGISTER_SIZE + 1];
	
	strcpy(temp_reg1, registers[rt]);
	
	registers[rd][0] = '\0';
	memcpy(registers[rd], temp_reg1 + sa, REGISTER_SIZE - sa);
	
	for (int i = strlen(registers[rd]) ; i < REGISTER_SIZE ; i++)
	{
		registers[rd][i] = '0';
	}
	
	registers[rd][REGISTER_SIZE] = '\0';
}

/*
	Function implementing the 'sllv' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void sllv(int rd, int rt, int rs)
{
	char temp_reg1[REGISTER_SIZE + 1];
	int temp_sa = bin_to_int(registers[rs], 32, 0);
	
	strcpy(temp_reg1, registers[rt]);
	
	registers[rd][0] = '\0';
	memcpy(registers[rd], temp_reg1 + temp_sa, REGISTER_SIZE - temp_sa);
	
	for (int i = strlen(registers[rd]) ; i < REGISTER_SIZE ; i++)
	{
		registers[rd][i] = '0';
	}
	
	registers[rd][REGISTER_SIZE] = '\0';
}

/*
	Function implementing the 'sra' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int sa - shift amount
	int rt - integer representation of the target register
*/
void sra(int rd, int rt, int sa)
{
	char temp_reg1[REGISTER_SIZE + 1];
	char one_or_zero;
		
	strcpy(temp_reg1, registers[rt]);
	
	registers[rd][0] = '\0';
	memcpy(registers[rd] + sa, temp_reg1, REGISTER_SIZE - sa);
	
	if(temp_reg1[0] == '0')
	{
		one_or_zero = '0';
	}
	else if(temp_reg1[0] == '1')
	{
		one_or_zero = '1';
	}
	
	for (int i = 0 ; i < sa ; i++)
	{
		registers[rd][i] = one_or_zero;
	}
	
	registers[rd][REGISTER_SIZE] = '\0';
}

/*
	Function implementing the 'srav' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void srav(int rd, int rt, int rs)
{
	char temp_reg1[REGISTER_SIZE + 1];
	int temp_sa = bin_to_int(registers[rs], 32, 0);
	char one_or_zero;
		
	strcpy(temp_reg1, registers[rt]);
	
	registers[rd][0] = '\0';
	memcpy(registers[rd] + temp_sa, temp_reg1, REGISTER_SIZE - temp_sa);
	
	if(temp_reg1[0] == '0')
	{
		one_or_zero = '0';
	}
	else if(temp_reg1[0] == '1')
	{
		one_or_zero = '1';
	}
	
	for (int i = 0 ; i < temp_sa ; i++)
	{
		registers[rd][i] = one_or_zero;
	}
	
	registers[rd][REGISTER_SIZE] = '\0';
}

/*
	Function implementing the 'srl' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int sa - shift amount
	int rt - integer representation of the target register
*/
void srl(int rd, int rt, int sa)
{
	char temp_reg1[REGISTER_SIZE + 1];

	strcpy(temp_reg1, registers[rt]);
	
	registers[rd][0] = '\0';
	memcpy(registers[rd] + sa, temp_reg1, REGISTER_SIZE - sa);
	
	for (int i = 0; i < sa; i++)
	{
		registers[rd][i] = '0';
	}
	
	registers[rd][REGISTER_SIZE] = '\0';
}

/*
	Function implementing the 'srlv' instruction.
	
	parameters:
	int rd - integer representation of the destination register
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void srlv(int rd, int rt, int rs)
{
	char temp_reg1[REGISTER_SIZE + 1];
	int temp_sa = bin_to_int(registers[rs], 32, 0);

	strcpy(temp_reg1, registers[rt]);
	
	registers[rd][0] = '\0';
	memcpy(registers[rd] + temp_sa, temp_reg1, REGISTER_SIZE - temp_sa);
	
	for (int i = 0; i < temp_sa; i++)
	{
		registers[rd][i] = '0';
	}
	
	registers[rd][REGISTER_SIZE] = '\0';
}

/*
	Function implementing the 'div' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void div_inst(int rs, int rt)
{
	int temp_hi = bin_to_int(registers[rs], 32, 1) % bin_to_int(registers[rt], 32, 1);
	int temp_lo = bin_to_int(registers[rs], 32, 1) / bin_to_int(registers[rt], 32, 1);
	
	int_to_bin(temp_hi, 32, hi);
	int_to_bin(temp_lo, 32, lo);	
}

/*
	Function implementing the 'divu' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void divu(int rs, int rt)
{
	int temp_hi = bin_to_int(registers[rs], 32, 0) % bin_to_int(registers[rt], 32, 0);
	int temp_lo = bin_to_int(registers[rs], 32, 0) / bin_to_int(registers[rt], 32, 0);
	
	int_to_bin(temp_hi, 32, hi);
	int_to_bin(temp_lo, 32, lo);	
}

/*
	Function implementing the 'mfhi' instruction.
	
	parameters:
	int rd - integer representation of the destination register
*/
void mfhi(int rd)
{
	strcpy(registers[rd], hi);
}

/*
	Function implementing the 'mfhi' instruction.
	
	parameters:
	int rd - integer representation of the destination register
*/
void mflo(int rd)
{
	strcpy(registers[rd], lo);
}

/*
	Function implementing the 'mthi' instruction.
	
	parameters:
	int rs - integer representation of the source register
*/
void mthi(int rs)
{
	strcpy(hi, registers[rs]);
}

/*
	Function implementing the 'mtlo' instruction.
	
	parameters:
	int rs - integer representation of the source register
*/
void mtlo(int rs)
{
	strcpy(lo, registers[rs]);
}

/*
	Function implementing the 'mult' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void mult(int rs, int rt)
{
	uint32_t temp_reg1 = bin_to_int(registers[rs], 32, 1);
	uint32_t temp_reg2 = bin_to_int(registers[rt], 32, 1);
	uint32_t temp_hi, temp_lo;
	   
	int64_t temp_result = (int64_t)(int32_t)temp_reg1 * (int64_t)(int32_t)temp_reg2;
	
	temp_hi = (uint32_t)((temp_result>>32) & 0xFFFFFFFF);
	temp_lo = (uint32_t)(temp_result & 0xFFFFFFFF);
	
	int_to_bin(temp_hi, 32, hi);
	int_to_bin(temp_lo, 32, lo);
}

/*
	Function implementing the 'multu' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
*/
void multu(int rs, int rt)
{
	uint32_t temp_reg1 = bin_to_int(registers[rs], 32, 1);
	uint32_t temp_reg2 = bin_to_int(registers[rt], 32, 1);
	uint32_t temp_hi, temp_lo;
	   
	uint64_t temp_result = (uint64_t)temp_reg1 * (uint64_t)temp_reg2;
	
	temp_hi = (uint32_t)((temp_result>>32) & 0xFFFFFFFF);
	temp_lo = (uint32_t)(temp_result & 0xFFFFFFFF);
	
	int_to_bin(temp_hi, 32, hi);
	int_to_bin(temp_lo, 32, lo);
}

/*
	Function implementing the 'beq' instruction.
	Returns 1 if condition is satisfied. Otherwise - 0
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int offset - integer representation of the offset
*/
int beq(int rs, int rt, int offset)
{
	if(strcmp(registers[rs], registers[rt]) == 0)
	{
		int_to_bin((bin_to_int(pc, 32, 0) + 4*offset), 32, pc); // PC = PC + 4*offset
		return 1;
	}
	
	return 0;
}

/*
	Function implementing the 'bgez' instruction.
	Returns 1 if condition is satisfied. Otherwise - 0
	
	parameters:
	int rs - integer representation of the source register
	int offset - integer representation of the offset
*/
int bgez(int rs, int offset)
{
	if(bin_to_int(registers[rs], 5, 1) >= 0)
	{
		int_to_bin((bin_to_int(pc, 32, 0) + 4*offset), 32, pc); // PC = PC + 4*offset
		return 1;
	}
	
	return 0;
}

/*
	Function implementing the 'bgezal' instruction.
	Returns 1 if condition is satisfied. Otherwise - 0
	
	parameters:
	int rs - integer representation of the source register
	int offset - integer representation of the offset	
*/
int bgezal(int rs, int offset)
{
	if(bin_to_int(registers[rs], 5, 1) >= 0)
	{
		strcpy(registers[31], pc); // $ra = pc
		int_to_bin((bin_to_int(pc, 32, 0) + 4*offset), 32, pc); // PC = PC + 4*offset
		return 1;
	}
	
	return 0;
}

/*
	Function implementing the 'bgtz' instruction.
	Returns 1 if condition is satisfied. Otherwise - 0
	
	parameters:
	int rs - integer representation of the source register
	int offset - integer representation of the offset
*/
int bgtz(int rs, int offset)
{
	if(bin_to_int(registers[rs], 5, 1) > 0)
	{
		int_to_bin((bin_to_int(pc, 32, 0) + 4*offset), 32, pc); // PC = PC + 4*offset
		return 1;
	}
	
	return 0;
}

/*
	Function implementing the 'blez' instruction.
	Returns 1 if condition is satisfied. Otherwise - 0
	
	parameters:
	int rs - integer representation of the source register
	int offset - integer representation of the offset
*/
int blez(int rs, int offset)
{
	if(bin_to_int(registers[rs], 5, 1) <= 0)
	{
		int_to_bin((bin_to_int(pc, 32, 0) + 4*offset), 32, pc); // PC = PC + 4*offset
		return 1;
	}
	
	return 0;
}

/*
	Function implementing the 'bltz' instruction.
	Returns 1 if condition is satisfied. Otherwise - 0
	
	parameters:
	int rs - integer representation of the source register
	int offset - integer representation of the offset
*/
int bltz(int rs, int offset)
{
	if(bin_to_int(registers[rs], 5, 1) < 0)
	{
		int_to_bin((bin_to_int(pc, 32, 0) + 4*offset), 32, pc); // PC = PC + 4*offset
		return 1;
	}
	
	return 0;
}

/*
	Function implementing the 'bltzal' instruction.
	Returns 1 if condition is satisfied. Otherwise - 0
	
	parameters:
	int rs - integer representation of the source register
	int offset - integer representation of the offset
*/
int bltzal(int rs, int offset)
{
	if(bin_to_int(registers[rs], 5, 1) < 0)
	{
		strcpy(registers[31], pc); // $ra = pc
		int_to_bin((bin_to_int(pc, 32, 0) + 4*offset), 32, pc); // PC = PC + 4*offset
		return 1;
	}
	
	return 0;
}

/*
	Function implementing the 'bne' instruction.
	Returns 1 if condition is satisfied. Otherwise - 0
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int offset - integer representation of the offset
*/
int bne(int rs, int rt, int offset)
{
	if(strcmp(registers[rs], registers[rt]))
	{
		int_to_bin((bin_to_int(pc, 32, 0) + 4*offset), 32, pc); // PC = PC + 4*offset
		return 1;
	}
	
	return 0;
}

/*
	Function implementing the 'j' instruction.
	
	parameters:
	int target - integer representation of the target of the jump
*/
void j(int target)
{
	int_to_bin(target, 26, pc);
}

/*
	Function implementing the 'jal' instruction.
	
	parameters:
	int target - integer representation of the target of the jump
*/
void jal(int target)
{
	strcpy(registers[31], pc); // $ra = pc
	int_to_bin(target, 26, pc);
}

/*
	Function implementing the 'jalr' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rd - integer representation of the destination register
*/
void jalr(int rd, int rs)
{
	strcpy(registers[rd], pc); // $rd = pc
	int_to_bin(bin_to_int(registers[rs], 32, 0), 32, pc);
}

/*
	Function implementing the 'jr' instruction.
	
	parameters:
	int rs - integer representation of the source register
*/
void jr(int rs)
{
	int_to_bin(bin_to_int(registers[rs], 32, 0), 32, pc);
}

/*
	Function implementing the 'syscall' instruction.
*/
void syscall(void)
{
	int temp_v0 = bin_to_int(registers[2], 32, 0);
	
	switch (temp_v0)
	{
		case 1: // print integer
		{
			printf("%d\n", bin_to_int(registers[4], 32, 1)); // print $a0
			break;
		}
		case 2: // print float
		{
			// To be continued...
			break;
		}
		case 3: // print double
		{
			// To be continued...
			break;
		}
		case 4: // print string
		{
			// To be continued...
			break;
		}
		case 5: // read integer
		{
			int temp_inp = 0;
	
			printf("Please enter a number to be further used: ");
			scanf("%d", &temp_inp);
			
			if (temp_inp > 2147483647 || temp_inp < -2147483648)
			{
				flagreg[OF] = '1';
			}
			
			int_to_bin(temp_inp, 32, registers[2]);
			
			break;
		}
		case 6: // read float
		{
			// To be continued...
			break;
		}
		case 7: // read double
		{
			// To be continued...
			break;
		}
		case 8: // read string
		{
			// To be continued...
			break;
		}
		case 9: // sbrk (allocate heap memory)
		{
			// To be continued...
			break;
		}
		case 10:
		{
			flagreg[HF] = '1';
			break;
		}
		case 11: // print character
		{
			// To be continued...
			break;
		}
		case 12: // read character
		{
			// To be continued...
			break;
		}
		/*
		.
		.
		.
		*/
		default:
		{
			printf("Something went wrong.\n");	
		}
	}		
}

/*
	Function implementing the 'lb' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int offset - integer representation of the offset
*/
void lb(int rt, int offset, int rs)
{
	for(int i = 0 ; i < 24 ; i++)
	{
		registers[rt][i] = '1';
	}
	
	memcpy(registers[rt]+24, memory[bin_to_int(registers[rs], 32, 0) + offset], 8);
	registers[rt][32] = '\0';
}

/*
	Function implementing the 'lbu' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int offset - integer  representation of the offset
*/
void lbu(int rt, int offset, int rs)
{
	for(int i = 0 ; i < 24 ; i++)
	{
		registers[rt][i] = '0';
	}
	
	memcpy(registers[rt]+24, memory[bin_to_int(registers[rs], 32, 0) + offset], 8);
	registers[rt][32] = '\0';
}

/*
	Function implementing the 'lh' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int offset - integer representation of the offset
*/
void lh(int rt, int offset, int rs)
{
	for(int i = 0 ; i < 16 ; i++)
	{
		registers[rt][i] = '1';
	}
	
	memcpy(registers[rt]+16, memory[bin_to_int(registers[rs], 32, 0) + offset], 8);
	memcpy(registers[rt]+24, memory[bin_to_int(registers[rs], 32, 0) + offset] + 9, 8);
	registers[rt][32] = '\0';
}

/*
	Function implementing the 'lhu' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int offset - integer representation of the offset
*/
void lhu(int rt, int offset, int rs)
{
	for(int i = 0 ; i < 16 ; i++)
	{
		registers[rt][i] = '0';
	}
	
	memcpy(registers[rt]+16, memory[bin_to_int(registers[rs], 32, 0) + offset], 8);
	memcpy(registers[rt]+24, memory[bin_to_int(registers[rs], 32, 0) + offset] + 9, 8);
	registers[rt][32] = '\0';
}

/*
	Function implementing the 'lw' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int offset - integer representation of the offset
	
*/
void lw(int rt, int offset, int rs)
{
	memcpy(registers[rt], memory[bin_to_int(registers[rs], 32, 0) + offset], 8);
	memcpy(registers[rt]+8, memory[bin_to_int(registers[rs], 32, 0) + offset] + 9, 8);
	memcpy(registers[rt]+16, memory[bin_to_int(registers[rs], 32, 0) + offset] + 18, 8);
	memcpy(registers[rt]+24, memory[bin_to_int(registers[rs], 32, 0) + offset] + 27, 8);
}

/*
	Function implementing the 'sb' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int offset - integer representation of the offset
*/
void sb(int rt, int offset, int rs)
{
	for(int i = 0 ; i < 3 ; i++)
	{
		for(int j = 0 ; j < 8 ; j++)
		{
			memory[i + bin_to_int(registers[rs], 32, 0) + offset][j] = '0';
		}
		
		memory[i + bin_to_int(registers[rs], 32, 0) + offset][8] = '\0';
	}
	
	memcpy(memory[bin_to_int(registers[rs], 32, 0) + offset] + 27, registers[rt]+24, 8);
}

/*
	Function implementing the 'sh' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int offset - integer representation of the offset
*/
void sh(int rt, int offset, int rs)
{
	for(int i = 0 ; i < 2 ; i++)
	{
		for(int j = 0 ; j < 8 ; j++)
		{
			memory[i + bin_to_int(registers[rs], 32, 0) + offset][j] = '0';
		}
		
		memory[i + bin_to_int(registers[rs], 32, 0) + offset][8] = '\0';
	}
	
	memcpy(memory[bin_to_int(registers[rs], 32, 0) + offset] + 18, registers[rt]+16, 8);
	memcpy(memory[bin_to_int(registers[rs], 32, 0) + offset] + 27, registers[rt]+24, 8);
}

/*
	Function implementing the 'sw' instruction.
	
	parameters:
	int rs - integer representation of the source register
	int rt - integer representation of the target register
	int offset - integer  representation of the offset
*/
void sw(int rt, int offset, int rs)
{
	memcpy(memory[bin_to_int(registers[rs], 32, 0) + offset], registers[rt], 8);
	memcpy(memory[bin_to_int(registers[rs], 32, 0) + offset] + 9, registers[rt]+8, 8);
	memcpy(memory[bin_to_int(registers[rs], 32, 0) + offset] + 18, registers[rt]+16, 8);
	memcpy(memory[bin_to_int(registers[rs], 32, 0) + offset] + 27, registers[rt]+24, 8);
}

