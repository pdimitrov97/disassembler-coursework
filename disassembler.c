/**************************************************************************
 * Assessment Title: ACE 2
 *
 * Number of Submitted C Files: 1
 *
 * Date: 26.10.2017
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

#define MEMORY_SIZE 4096
#define INSTRUCTION_FILE "disassembler-test-program2.txt"
#define HF 0 // Halt Flag
#define OF 1 // Overflow Flag
#define ZF 2 // Zero Flag

char memory[MEMORY_SIZE][17];
char ac[17];
char ir[17];
char mbr[17];
char pc[13];
char mar[13];
char inreg[17];
char outreg[17];
char flagreg[8] = {"0000000"};
char what_is_what[13][9] = {"Halt", "Skipcond", "Jump", "Load", "Store", "Subt", "Add", "Input", "Output", "JZE", "JNZ", "SubtI", "AddI"};

void int_to_bin(int number, int bits, char* destination);
int bin_to_int(char* binary);
void display_memory(void);
void display_assembly(void);
void load_default(void);
void load_user(void);
void load_file(void);
void check_flagreg(void);
void execute_program(void);

void halt_instruction(void);
void skipcond_instruction(void);
void jump_instruction(void);
void load_instruction(void);
void store_instruction(void);
void subt_instruction(void);
void add_instruction(void);
void input_instruction(void);
void output_instruction(void);
void jze_instruction(void);
void jnz_instruction(void);
void subti_instruction(void);
void addi_instruction(void);

/*
	This program takes in one argument at the start and then executes an assembly program
	with instructions determined by the argument itself.
	If the given argument is -d it loads a default assembly program already hard-coded;
	if the given argument is -c it allows the user to enter each 16-bit field one-by-one; 
	if the given argument is -f it loads the assembly program from a provided file named "ace3_program.txt".
	If the user enters zero or more than one argument, the program terminates with a useful message to the user.
*/
int main (int argc, char *argv[])
{
	if(argc != 2)
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
	
	printf("\nMemory before execution of program:\n");
	display_memory();
	
	printf("\nMemory converted to assembly:\n");
	display_assembly();
	
	execute_program();
	
	printf("\nMemory after execution of program:\n");
	display_memory();
	printf("\nValue of AC is: %d\n", bin_to_int(ac));
	
	return 0;
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
*/
int bin_to_int(char* binary)
{
	int number = 0;
	char temp[16];
	int i = 0;
	
	for (i = 0 ; i < (16 - strlen(binary)) ; i++)
	{
		temp[i] = '0';
	}
	
	temp[i] = '\0';
	strcat(temp, binary);
	
	number += (temp[0] - '0') * -1;
	
	for (int j = 1 ; j < 16 ; j++)
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
	printf("MLoc | Memory contents\n");
	
	for (int counter = 0 ; counter < MEMORY_SIZE ; counter++)
	{
		if (!strcmp(memory[counter] , "") == 0)
		{
			printf("%4d - %s\n", counter, memory[counter]);
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
	char temp_opcode[5];
	char temp_operand[13];
	int temp_int_opcode;
	
	printf("MLoc | Translated Memory\n");
	
	for (int counter = 0 ; counter < MEMORY_SIZE ; counter++)
	{
		if(strcmp(memory[counter], "") == 0)
			continue;
		
		memcpy(temp_opcode, memory[counter], 4);
		memcpy(temp_operand, memory[counter] + 4,  12);
		temp_opcode[4] = '\0';
		temp_operand[12] = '\0';
		temp_int_opcode = bin_to_int(temp_opcode);
		
		if(temp_int_opcode == 0 || temp_int_opcode == 7 || temp_int_opcode == 8)
			printf("%4d - %-8s\n", counter, what_is_what[temp_int_opcode]);
		else	
			printf("%4d - %-8s %d\n", counter, what_is_what[temp_int_opcode], bin_to_int(temp_operand));
	}
}

/*
	Loads a default program in memory.
*/
void load_default(void)
{
	strcpy(memory[0], "0111000000000000"); // Input
	strcpy(memory[1], "0100111111111111"); // Store 4095
	strcpy(memory[2], "0111000000000000"); // Input
	strcpy(memory[3], "0100111111111110"); // Store 4094
	strcpy(memory[4], "0100111111111101"); // Store 4093
	strcpy(memory[5], "0101111111111101"); // Subt 4093
	strcpy(memory[6], "0100111111111100"); // Store 4092
	
	strcpy(memory[7], "0011111111111101"); // Load 4093
	strcpy(memory[8], "1001000000011010"); // JZE 26 (END)

	strcpy(memory[9], "0011111111111110"); // Load 4094	
	strcpy(memory[10], "0001100000000000"); // Skipcond 800 (AC > 0)
	strcpy(memory[11], "0010000000010011"); // Jump 19 (NEGATIVE)

	strcpy(memory[12], "0011111111111100"); // Load 4092
	strcpy(memory[13], "0110111111111111"); // Add 4095
	strcpy(memory[14], "0100111111111100"); // Store 4092
	
	strcpy(memory[15], "0011111111111101"); // Load 4093
	strcpy(memory[16], "1011000000000001"); // SubtI 1
	strcpy(memory[17], "0100111111111101"); // Store 4093
	
	strcpy(memory[18], "0010000000000111"); // Jump 7 (LOOP)
	
	strcpy(memory[19], "0011111111111100"); // Load 4092
	strcpy(memory[20], "0101111111111111"); // Sub 4095
	strcpy(memory[21], "0100111111111100"); // Store 4092
	
	strcpy(memory[22], "0011111111111101"); // Load 4093
	strcpy(memory[23], "1100000000000001"); // AddI 1
	strcpy(memory[24], "0100111111111101"); // Store 4093	
	
	strcpy(memory[25], "0010000000000111"); // Jump 7 (LOOP)
	
	strcpy(memory[26], "0011111111111100"); // Load 4092
	strcpy(memory[27], "1000000000000000"); // Output
	strcpy(memory[28], "0000000000000000"); // Halt
	
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
	char input[18];
	int flag = 0;
	
	while(1)
	{
		printf("Enter (\'stop\' to end) memory content (16 bit) at memory location %d: ", ROW_POINTER);
		scanf("%s", input);
		
		if (!strcmp(input, "stop"))
			break;
			
		if(ROW_POINTER >= MEMORY_SIZE)
		{
			printf("Not enough memory !\n");
			continue;
		}
			
		if (strlen(input) < 16)
		{
			printf("Instruction too short !\n");
			continue;
		}
			
		if (strlen(input) > 16)
		{
			printf("Instruction too long !\n");
			continue;
		}
		
		flag = 0;
			
		for (COLUMN_POINTER = 0 ; COLUMN_POINTER < 16 ; COLUMN_POINTER++)
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
        	if(COLUMN_POINTER != 16)
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
			if(COLUMN_POINTER < 16)
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
	if (bin_to_int(ac) == 0)
		flagreg[ZF] = '1';
	else
		flagreg[ZF] = '0';
		
	if (flagreg[OF] == '1')
	{
		printf("Overflow detected at memory location: %d\n", (bin_to_int(pc) - 1));
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
	char temp_opc[5];
	int temp_int_opc;
	
	int_to_bin(0, 12, pc); // Set PC = 0
	
	while(flagreg[HF] == '0') // While HF is not 1
	{
		check_flagreg();
		
		if(strcmp(memory[bin_to_int(pc)], "") == 0) // If empty instruction
		{
			int_to_bin((bin_to_int(pc) + 1), 12, pc); // PC = PC + 1
			continue;
		}
			
		if(bin_to_int(pc) == MEMORY_SIZE) // Check if we have reached end of memory
			break;
		
		strcpy(mar, pc); // Copy PC to MAR
		strcpy(ir, memory[bin_to_int(mar)]); // IR = memory[MAR] 
		int_to_bin((bin_to_int(pc) + 1), 12, pc); // PC = PC + 1
		memcpy(temp_opc, ir, 4); // Decode OPC
		temp_opc[4] = '\0';
		temp_int_opc = bin_to_int(temp_opc);
		strcpy(mar, ir+4); // MAR = IR[11-0]
		
		switch(temp_int_opc)
		{
			case 0: // Halt
			{
				halt_instruction();
				break;
			}
			case 1: // Skipcond X
			{
				skipcond_instruction();
				break;
			}
			case 2: // Jump X
			{
				jump_instruction();
				break;
			}
			case 3: // Load X
			{
				load_instruction();
				break;
			}
			case 4: // Store X
			{
				store_instruction();
				break;
			}
			case 5: // Subt X
			{
				subt_instruction();
				break;
			}
			case 6: // Add X
			{
				add_instruction();
				break;
			}
			case 7: // Input
			{
				input_instruction();
				break;
			}
			case 8: // Output
			{
				output_instruction();
				break;
			}
			case 9: // JZE X
			{
				jze_instruction();
				break;
			}
			case 10: // JNZ X
			{
				jnz_instruction();
				break;
			}
			case 11: // SubtI N
			{
				subti_instruction();
				break;
			}
			case 12: // AddI N
			{
				addi_instruction();
				break;
			}
			default:
			{
				printf("Something went wrong.\n");
			}
		}
	}
}

/*
	This function is representation of the Halt instruction.
*/
void halt_instruction(void)
{
	flagreg[HF] = '1';
}

/*
	This function is representation of the Skipcond instruction.
	If the operand is 0 and AC < 0, skips next instruction;
	If the operand is 1024 and AC == 0, skips next instruction;
	If the operand is 2048 and AC > 0, skips next instruction;
*/
void skipcond_instruction(void)
{
	int temp_value = bin_to_int(mar);
	int temp_ac = bin_to_int(ac);
	
	if (temp_value == 0 && temp_ac < 0) // operand ?= 000000000000
		int_to_bin((bin_to_int(pc) + 1), 12, pc);
	else if (temp_value == 1024 && temp_ac == 0) // operand ?= 010000000000
		int_to_bin((bin_to_int(pc) + 1), 12, pc);
	else if (temp_value == 2048 && temp_ac > 0) // operand ?= 100000000000
		int_to_bin((bin_to_int(pc) + 1), 12, pc);
}

/*
	This function is representation of the Jump instruction.
	It sets PC to the operand of the current instruction.
*/ 
void jump_instruction(void)
{
	strcpy(pc, mar);
}

/*
	This function is representation of the Load instruction.
	It loads the value at memory location the operand of the current instruction into AC.
*/
void load_instruction(void)
{
	strcpy(mbr, memory[bin_to_int(mar)]);
	strcpy(ac, mbr);		
}

/*
	This function is representation of the Store instruction.
	It puts the value of AC at memory location the operand of the current instruction.
*/
void store_instruction(void)
{
	strcpy(mbr, ac);
	strcpy(memory[bin_to_int(mar)], mbr);	
}

/*
	This function is representation of the Subt instruction.
	It calculates the result of (AC - (the value at memory location the operand of the current instruction)).
	The result is put into AC.
*/
void subt_instruction(void)
{
	strcpy(mbr, memory[bin_to_int(mar)]);
	
	int temp_result = bin_to_int(ac) - bin_to_int(mbr);
	
	if (temp_result > 32767 || temp_result < -32768)
	{
		flagreg[OF] = '1';
	}
	
	int_to_bin(temp_result, 16, ac);	
}

/*
	This function is representation of the Add instruction.
	It calculates the result of (AC + (the value at memory location the operand of the current instruction)).
	The result is put into AC.
*/
void add_instruction(void)
{
	strcpy(mbr, memory[bin_to_int(mar)]);
	
	int temp_result = bin_to_int(ac) + bin_to_int(mbr);

	if (temp_result > 32767 || temp_result < -32768)
	{
		flagreg[OF] = '1';
	}
	
	int_to_bin(temp_result, 16, ac);
}

/*
	This function is representation of the Input instruction.	
	It prompts the user to enter a number from the keyboard and puts that number
	into AC if the number is a valid 16-bit number in two's complement.
*/
void input_instruction(void)
{
	int temp_inp = 0;
	
	printf("Please enter a number to be further used: ");
	scanf("%d", &temp_inp);
	
	if (temp_inp > 32767 || temp_inp < -32768)
	{
		flagreg[OF] = '1';
	}
	
	int_to_bin(temp_inp, 16, inreg);
	strcpy(ac, inreg);
}

/*
	This function is representation of the Output instruction.	
	It displays the value of AC to the user's screen.
*/
void output_instruction(void)
{
	strcpy(outreg, ac);
	printf("The value of AC is: %d\n", bin_to_int(outreg));
}

/*
	This function is representation of the JZE instruction.	
	It goes to memory location the operand of the current instruction
	if the value of AC is 0 (ZERO flag is set).
*/
void jze_instruction(void)
{
	if (flagreg[ZF] == '1')
		strcpy(pc, mar);
}

/*
	This function is representation of the JNZ instruction.	
	It goes to memory location the operand of the current instruction
	if the value of AC is NOT 0 (ZERO flag is NOT set).
*/
void jnz_instruction(void)
{
	if (flagreg[ZF] == '0')
		strcpy(pc, mar);
}

/*
	This function is representation of the SubtI instruction.
	It calculates the result of (AC - (the integer representation of the operand)).
	The result is put into AC.
*/
void subti_instruction(void)
{
	int temp_result = bin_to_int(ac) - bin_to_int(mar);
	
	if (temp_result > 32767 || temp_result < -32768)
	{
		flagreg[OF] = '1';
	}
	
	int_to_bin(temp_result, 16, ac);
}

/*
	This function is representation of the AddI instruction.
	It calculates the result of (AC + (the integer representation of the operand)).
	The result is put into AC.
*/
void addi_instruction(void)
{
	int temp_result = bin_to_int(ac) + bin_to_int(mar);
	
	if (temp_result > 32767 || temp_result < -32768)
	{
		flagreg[OF] = '1';
	}
	
	int_to_bin(temp_result, 16, ac);
}

