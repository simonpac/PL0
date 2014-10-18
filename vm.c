#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

typedef struct 
{
	int op; // opcode
	int l; // L
	int m; // M
} instruction;

void print_assembly(FILE *ofp, int line, int op, int l, int m, char *opcode_string);
void execute_instructions(FILE *ofp, instruction *code, int program_counter, int base_pointer, int stack_pointer, int num_instructions);
void print_execution_state(FILE *ofp, instruction *code, int old_pc, int new_pc, int base_pointer, int stack_pointer, int *stack, int *records_start_indexes);
void print_stack(FILE *ofp, int *stack, int sp, int *bracket_index);
int base(int l, int base, int *stack);
int get_halt_flag(int pc, int instruction_count);

char *opcodes[] = {"", "LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SIO", "SIO", "SIO"};

int main()
{
	// Creating a stack and initializing registers as well as the first three indexes of the stack
	int sp = 0, bp = 1, pc = 0, ir = 0;

	instruction *code = malloc(sizeof(instruction) * MAX_CODE_LENGTH);

	// Open file and test to see if the file can open
	FILE *ifp = fopen("mcode.txt", "r");
	FILE *ofp = fopen("stacktrace.txt", "w");
	if (!ifp)
	{
		printf("File is unable to open\n");
		return -1;
	}

	int i = 0, j = 0, instruction_count;

	fprintf(ofp, "Line\tOP\t\tL\t\tM\n");
	// Looping until the end of the file
	while (!feof(ifp))
	{
		// Mallocing space for the mcode and reading the file from the values into the instruction variable
		fscanf(ifp, "%d %d %d", &code[i].op, &code[i].l, &code[i].m);

		if (opcodes[code[i].op] != '\0')
			print_assembly(ofp, i, code[i].op, code[i].l, code[i].m, opcodes[code[i].op]);
		i++;
	}

	instruction_count = i;

	fprintf(ofp, "\n\t\t\t\t\t\t\t\tPC\t\tBP\t\tSP\t\tstack\n");
	fprintf(ofp, "Initial Values\t\t\t\t\t\t\t%d\t\t%d\t\t%d\n", pc, bp, sp);
	
	execute_instructions(ofp, code, pc, bp, sp, instruction_count);

	fclose(ifp);
	return 0;
}

void print_assembly(FILE *ofp, int line, int op, int l, int m, char *opcode_string)
{
	fprintf(ofp, "%d\t\t%s\t\t%d\t\t%d\n", line, opcode_string, l, m);
}

void execute_instructions(FILE *ofp, instruction *code, int program_counter, int base_pointer, int stack_pointer, int num_instructions)
{
	
	// Initializing a stack and setting all of the values to 0
	int stack[MAX_CODE_LENGTH], i, halt = 0, temp, user_value;
	for (i = 1; i < MAX_CODE_LENGTH; i++)
		stack[i] = 0;

	int activation_records_starting_index[3], j = 0;
	activation_records_starting_index[j] = 0;

	while (!halt)
	{
		switch (code[program_counter].op)
		{
			case 1: // LIT
				temp = program_counter;
				program_counter++;
				stack_pointer++;
				stack[stack_pointer] = code[temp].m;
				print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
				halt = get_halt_flag(temp, num_instructions-1);
				break;
			case 2: // OPR
				switch (code[program_counter].m)
				{
					case 0: // RET
						temp = program_counter;
						stack_pointer = base_pointer - 1;
						program_counter = stack[stack_pointer + 4];
						base_pointer = stack[stack_pointer + 3];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 1: // NEG
						temp = program_counter;
						program_counter++;
						stack[stack_pointer] = stack[stack_pointer] * -1;
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 2: // ADD
						temp = program_counter;
						program_counter++;
						stack_pointer--;
						stack[stack_pointer] = stack[stack_pointer] + stack[stack_pointer + 1];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 3: // SUB
						temp = program_counter;
						program_counter++;
						stack_pointer--;
						stack[stack_pointer] = stack[stack_pointer] - stack[stack_pointer + 1];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 4: // MUL
						temp = program_counter;
						program_counter++;
						stack_pointer--;
						stack[stack_pointer] = stack[stack_pointer] * stack[stack_pointer + 1];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 5: // DIV
						temp = program_counter;
						program_counter++;
						stack_pointer--;
						stack[stack_pointer] = stack[stack_pointer] / stack[stack_pointer + 1];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 6: // ODD
						temp = program_counter;
						program_counter++;
						stack[stack_pointer] %= 2;
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 7: // MOD
						temp = program_counter;
						program_counter++;
						stack_pointer--;
						stack[stack_pointer] = stack[stack_pointer] % stack[stack_pointer + 1];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 8: // EQL
						temp = program_counter;
						program_counter++;
						stack_pointer--;
						stack[stack_pointer] = stack[stack_pointer] == stack[stack_pointer + 1];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 9: // NEQ
						temp = program_counter;
						program_counter++;
						stack_pointer--;
						stack[stack_pointer] = stack[stack_pointer] != stack[stack_pointer + 1];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 10: // LSS
						temp = program_counter;
						program_counter++;
						stack_pointer--;
						stack[stack_pointer] = stack[stack_pointer] < stack[stack_pointer + 1];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 11: // LEQ
						temp = program_counter;
						program_counter++;
						stack_pointer--;
						stack[stack_pointer] = stack[stack_pointer] <= stack[stack_pointer + 1];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 12: // GTR
						temp = program_counter;
						program_counter++;
						stack_pointer--;
						stack[stack_pointer] = stack[stack_pointer] > stack[stack_pointer + 1];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
					case 13: // GEQ
						temp = program_counter;
						program_counter++;
						stack_pointer--;
						stack[stack_pointer] = stack[stack_pointer] >= stack[stack_pointer + 1];
						print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
						halt = get_halt_flag(temp, num_instructions-1);
						break;
				}
				break;
			case 3: // LOD
				temp = program_counter;
				program_counter++;
				stack_pointer++;
				stack[stack_pointer] = stack[base(code[temp].l, base_pointer, stack) + code[temp].m];
				print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
				halt = get_halt_flag(temp, num_instructions-1);
				break;
			case 4: // STO
				temp = program_counter;
				program_counter++;
				stack[base(code[temp].l, base_pointer, stack) + code[temp].m] = stack[stack_pointer];
				stack_pointer--;
				print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
				halt = get_halt_flag(temp, num_instructions-1);
				break;
			case 5: // CAL
				activation_records_starting_index[j] = stack_pointer;
				temp = program_counter;
				stack[stack_pointer + 1] = 0;
				stack[stack_pointer + 2] = base(code[temp].l, base_pointer, stack);
				stack[stack_pointer + 3] = base_pointer;
				program_counter++;
				stack[stack_pointer + 4] = program_counter;
				base_pointer = stack_pointer + 1;
				program_counter = code[temp].m;
				print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
				j++;
				halt = get_halt_flag(temp, num_instructions-1);
				break;
			case 6: // INC
				temp = program_counter;
				program_counter++;
				stack_pointer = stack_pointer + code[temp].m;
				print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
				halt = get_halt_flag(temp, num_instructions-1);
				break;
			case 7: // JMP
				temp = program_counter;
				program_counter = code[program_counter].m;
				print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
				halt = get_halt_flag(temp, num_instructions-1);
				break;
			case 8: //JPC
				temp = program_counter;
				if (stack[stack_pointer] == 0)
					program_counter = code[program_counter].m;
				else
					program_counter++;
				stack_pointer--;
				print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
				halt = get_halt_flag(temp, num_instructions-1);
				break;
			case 9: // PRINT TOP OF STACK
				temp = program_counter;
				program_counter++;
				printf("%d\n", stack[stack_pointer]);
				print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
				halt = get_halt_flag(temp, num_instructions-1);
				break;
			case 10: // READ USER INPUT AND STORE TO TOP OF STACk
				temp = program_counter;
				program_counter++;
				stack_pointer++;
				printf("Please enter a number: ");
				scanf("%d", &user_value);
				stack[stack_pointer] = user_value;
				print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
				halt = get_halt_flag(temp, num_instructions-1);
				break;
			case 11: // HALT THE MACHINE
				temp = program_counter;
				program_counter++;
				print_execution_state(ofp, &code[temp], temp, program_counter, base_pointer, stack_pointer, stack, activation_records_starting_index);
				halt = 1;
				break;
		}
	}
}

int get_halt_flag(int pc, int instruction_count)
{
	if (pc == instruction_count)
		return 1;
	else
		return 0;
}

void print_execution_state(FILE *ofp, instruction *code, int old_pc, int new_pc, int base_pointer, int stack_pointer, int *stack, int *records_start_indexes)
{
	if (stack_pointer == 0)
	{
		fprintf(ofp, "%d\t\t%s\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", old_pc, opcodes[code->op], code->l, code->m, new_pc, base_pointer, stack_pointer);
	}
	else
	{
		fprintf(ofp, "%d\t\t%s\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t", old_pc, opcodes[code->op], code->l, code->m, new_pc, base_pointer, stack_pointer);
		print_stack(ofp, stack, stack_pointer, records_start_indexes);
	}
}

void print_stack(FILE *ofp, int *stack, int sp, int *bracket_index)
{
	int i, j = 0;
	for (i = 1; i <= sp; i++)
	{
		fprintf(ofp, "%d ", stack[i]);
		if (bracket_index[j] != 0 && i == bracket_index[j] && sp > bracket_index[j])
		{
			fprintf(ofp, "| ");
			j++;
		}
	}
		
	fprintf(ofp, "\n");
}

int base(int l, int base, int *stack) // l stand for L in the instruction format
{  
  int b1; //find base L levels down
  b1 = base; 
  while (l > 0)
  {
    b1 = stack[b1 + 1];
    l--;
  }
  return b1;
}