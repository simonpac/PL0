#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_CODE_LENGTH 500

typedef struct symbol
{
	int kind;
	char name[12];
	int val;
	int level;
	int addr;
}symbol;

typedef struct instruction
{
	int op;
	int l;
	int m;
} instruction;

// token type for parser
typedef enum
{
	nulsym = 1, identsym, numbersym, plussym, minussym,
	multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
	gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
	periodsym, becomessym, beginsym, endsym, ifsym, thensym,
	whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
	readsym , elsesym
} token_type;

// Opcodes for virtual machine
typedef enum
{
	lit = 1, opr, lod, sto, cal, inc, jmp, jpc, print_top_stack, read_input, halt
} opcode;

//Operator codes
typedef enum
{
	RET = 0, NEG, ADD, SUB, MUL,
	DIV, ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ
} operator;

/*
typedef enum
{
	nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
	multsym = 6,  slashsym = 7, oddsym = 8, eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
	gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
	periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
	whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
	readsym = 32, elsesym = 33
} token_type;
*/

// Global Variables
token_type token;
int token_num = 0, program_counter = 0, var_counter = 0, m1 = 0;
instruction mcode[MAX_CODE_LENGTH];

// Function prototypes
symbol* read_file();
void get_token(symbol *symbols);
void program(symbol *symbols);
void block(symbol *symbols);
void statement(symbol *symbols);
void expression(symbol *symbols);
void condition(symbol *symbols);
void term(symbol *symbols);
void factor(symbol *symbols);
void create_symbol(symbol *symbols);
void generate_code(int op, int l, int m);
int find_constant(char* variable_name);
int find_variable(char* variable_name);
symbol symbols_table[1000];

void create_symbol(symbol * symbols)
{
	int counter = 0;
	int counter_symbol = 0;
	bzero(symbols_table,sizeof(symbol)*1000);

	while(1)
	{
		if(symbols[counter].kind == 29 )	
		{
			counter ++;
			while(symbols[counter].kind != semicolonsym){
					if(symbols[counter].kind == 17)
						counter ++;
					symbols_table[counter_symbol].kind = 2;
					strcpy(symbols_table[counter_symbol].name,symbols[counter].name);
					symbols_table[counter_symbol].level = 0;
					symbols_table[counter_symbol].addr = var_counter;
					var_counter ++;
					counter_symbol ++;
					counter ++;

			}
		}
		else if(symbols[counter].kind == 28){
			counter ++;
			while(symbols[counter].kind != semicolonsym){
				if(symbols[counter].kind == 17)
					counter ++;
				symbols_table[counter_symbol].kind = 1;
				strcpy(symbols_table[counter_symbol].name, symbols[counter].name);
				counter ++;
				counter ++;
				symbols_table[counter_symbol].val = symbols[counter].val;
				counter_symbol ++;
				counter ++;
				printf("%d\n",symbols[counter].kind);
			}
		
		}

		else if(symbols_table[counter].kind == 0)
			break;

		counter ++;
	}
}

void get_token(symbol *symbols)
{
	token = symbols[token_num].kind;
	token_num++;
}

int main()
{
	symbol *symbols = read_file();
	create_symbol(symbols);
	program(symbols);

	return 0;
}

void program(symbol *symbols)
{
	//printf("\t\tline 71\n");
	get_token(symbols);
	block(symbols);

	if (token != periodsym)
	{
		printf("Period Expected!\n");
		exit(1);
	}
	generate_code(halt, 0, 3);
}

void generate_code(int op, int l, int m)
{
	mcode[program_counter].op = op;
	mcode[program_counter].l = l;
	mcode[program_counter].m = m;
	printf("%d %d %d\n", mcode[program_counter].op,mcode[program_counter].l,mcode[program_counter].m);
	program_counter++;
}

//void emit(opcodes op, int l, )

void block(symbol *symbols)
{
	if (token == constsym)
	{
		do
		{
			//printf("\t\tline 89\n");
			get_token(symbols);
			if (token != identsym)
			{
				printf("Expected an Identity Symbol!\n");
				exit(1);
			}
			//printf("\t\tline 96\n");
			get_token(symbols);
			if (token != eqsym)
			{
				printf("Expected an Equal Symbol\n");
				exit(1);
			}
			//printf("\t\tline 103\n");
			get_token(symbols);
			if (token != numbersym)
			{
				printf("Expected a number\n");
				exit(1);
			}
			//printf("\t\tline 110\n");
			get_token(symbols);
			//get_token(symbols);
		} while (token == commasym);
		if (token != semicolonsym)
		{
			printf("Expected a Semicolon\n");
			exit(1);
		}
		//printf("\t\tline 119\n");
		get_token(symbols);
	}
	if (token == varsym)
	{
		do
		{
			//printf("\t\tline 126\n");
			get_token(symbols);
			if (token != identsym)
			{
				printf("Expected an Identity Symbol!\n");
				exit(1);
			}
			//printf("\t\tline 132\n");
			get_token(symbols);
		} while (token == commasym);

		if (token != semicolonsym)
		{
			printf("Expected a Semicolon\n");
			exit(1);
		}
		//printf("\t\tline 142\n");
		get_token(symbols);
	}
	/*
	do
	{
		get_token(symbols);
		if (token != identsym)
		{
			printf("Expected an Identity Symbol!\n");
			exit(1);
		}
		get_token(symbols);
		if (token != semicolonsym)
		{
			printf("Expected a Semicolon\n");
			exit(1);
		}
		get_token(symbols);
		block(symbols);
		if (token != semicolonsym)
		{
			printf("Expected a Semicolon\n");
			exit(1);
		}
		get_token(symbols);

	} while (token == procsym);
	*/
	generate_code(inc, 0, var_counter);
	statement(symbols);
	
	//generate_code(opr, 0, RET);
}

int find_variable(char* variable_name){

	int x = 0;
	while(symbols_table[x].kind != 0)
	{
		if (strcmp(symbols_table[x].name, variable_name) == 0 && symbols_table[x].kind == 2)
		{
			m1 = symbols_table[x].addr;
			return 1;
		}
		x++;
	}
	return 0;
}

int find_constant(char* variable_name){

	int x = 0;
	while(symbols_table[x].kind != 0)
	{
		if (strcmp(symbols_table[x].name, variable_name) == 0 && symbols_table[x].kind == 1)
		{
			m1 = symbols_table[x].val;
			return 1;
		}
		x++;
	}
	return 0;
}

//void find_function(char)

void statement(symbol *symbols)
{
	if (token == identsym)
	{
		if(!find_variable(symbols[token_num-1].name))
		{
			if(find_constant(symbols[token_num-1].name))
			printf("Error. Variable not found.\n");
			exit(1);
		}
		//printf("\t\tline 178\n");
		get_token(symbols);
		//printf("token = %d\n", token);
		if (token != becomessym)
		{
			printf("Expected a Becomes Symbol\n");
			exit(1);
		}
		//printf("\t\tline 185\n");
		get_token(symbols);
		expression(symbols);
		printf("\n%s\n",symbols[token_num].name);
		generate_code(sto, 0, m1);
	}
	else if (token == callsym)
	{
		//printf("\t\tline 191\n");
		get_token(symbols);
		if (token != identsym)
		{
			printf("Expected an Identity Symbol\n");
			exit(1);
		}
		generate_code(cal, 0, m1);
		//printf("\t\tline 198\n");
		get_token(symbols);
	}
	else if (token == beginsym)
	{
		//printf("\t\tline 203\n");
		get_token(symbols);
		statement(symbols);
		while (token == semicolonsym)
		{
			//printf("\t\tline 208\n");
			get_token(symbols);
			statement(symbols);
		}
		if (token != endsym)
		{
			printf("Expected an End Symbol\n");
			exit(1);
		}
		//printf("\t\tline 220\n");
		get_token(symbols);
	}
	else if (token == ifsym)
	{
		//printf("\t\tline 225\n");
		get_token(symbols);
		condition(symbols);
		if (token != thensym)
		{
			printf("Expected a Then Symbol!\n");
			exit(1);
		}
		//printf("\t\tline 233\n");
		get_token(symbols);
		statement(symbols);
	}
	else if (token == whilesym)
	{
		//printf("\t\tline 238\n");
		get_token(symbols);
		condition(symbols);
		if (token != dosym)
		{
			printf("Expected a Do Symbol\n");
			exit(1);
		}
		//printf("\t\tline 247\n");
		get_token(symbols);
		statement(symbols);

		generate_code(jmp, 0, program_counter);
	}
	else if (token == readsym)
	{
		generate_code(readsym, 0, 2);
		get_token(symbols);

		if (token == identsym)
		{
			if (find_variable(symbols[token_num - 1].name))
			{
				generate_code(sto, 0, m1);
			}
		}
		else
		{
			printf("Expected an Identity Symbol!\n");
			exit(1);
		}
		get_token(symbols);
	}
	else if (token == writesym)
	{
		get_token(symbols);
		if (token == identsym)
		{
			if (find_variable(symbols[token_num - 1].name))
			{
				generate_code(lod, 0, m1);
			}
			else if (find_constant(symbols[token_num-1].name))
			{
				generate_code(lit, 0, m1);
			}
		}
		else
		{
			printf("Expected an Identity Symbol!\n");
			exit(1);
		}
		get_token(symbols);
		generate_code(print_top_stack, 0, 1);
	}
}

void condition(symbol *symbols)
{
	if (token == oddsym)
	{
		//printf("\t\tline 257\n");
		get_token(symbols);
		expression(symbols);
		generate_code(opr, 0, ODD);
	}
	else
	{
		int temp;
		expression(symbols);
		if (token != eqsym && token != neqsym && token != leqsym && token != gtrsym && token != geqsym)
		{
			printf("Unexpected Expression\n");
			exit(1);
		}
		else
		{
			temp = token;
		}
		//printf("\t\tline 269\n");
		get_token(symbols);
		expression(symbols);

		if (temp == eqsym)
			generate_code(opr, 0, EQL);
		else if (temp == neqsym)
			generate_code(opr, 0, NEQ);
		else if (temp == lessym)
			generate_code(opr, 0, LSS);
		else if (temp == leqsym)
			generate_code(opr, 0, LEQ);
		else if (temp == gtrsym)
			generate_code(opr, 0, GTR);
		else if (temp == geqsym)
			generate_code(opr, 0, GEQ);
	}
}

void expression(symbol *symbols)
{
	int temp;
	if (token == plussym || token == minussym)
	{
		//temp = token;
		//printf("\t\tline 279\n");
		get_token(symbols);
	}
	term(symbols);

	if (token == minussym)
		generate_code(opr, 0, NEG);

	while (token == plussym || token == minussym)
	{
		//printf("\t\tline 285\n");
		temp = token;
		get_token(symbols);
		term(symbols);

		if (temp == plussym)
			generate_code(opr, 0, ADD);
		if (temp == minussym)
			generate_code(opr, 0, SUB);
	}
}

void term(symbol *symbols)
{
	int temp;
	factor(symbols);
	while (token == multsym || token == slashsym)
	{
		temp = token;
		//printf("\t\tline 296\n");
		get_token(symbols);
		factor(symbols);

		if (temp == multsym)
			generate_code(opr, 0, MUL);
		if (temp == slashsym)
			generate_code(opr, 0, DIV);
	}
}

void factor(symbol *symbols)
{
	if (token == identsym)
	{
		if (find_constant(symbols[token_num-1].name)){
			generate_code(lit, 0, m1);
		}
		if (find_variable(symbols[token_num-1].name))
			generate_code(lod, 0, m1);
		//printf("\t\tline 306\n");
		get_token(symbols);
	}
	else if (token == numbersym)
	{
		generate_code(lit, 0, symbols[token_num-1].val);
		//printf("\t\tline 311\n");
		get_token(symbols);
	}
	else if (token == lparentsym)
	{
		//printf("\t\tline 316\n");
		get_token(symbols);
		expression(symbols);
		if (token != rparentsym)
		{
			printf("Expected a Right Parenthesis Symbol\n");
			exit(1);
		}
		//printf("\t\tline 324\n");
		get_token(symbols);
	}
	else
	{
		printf("Error. Unexpected Symbol\n");
	}
}

symbol* read_file()
{
    int inc = 0;
	int x = 1;
	FILE *ifp = fopen("lexemelist.txt","r");
	symbol *symbols = malloc(sizeof(symbol) * 1000);
	bzero(symbols,sizeof(symbol)*1000);
	int c = 0;
	int counter = -1;
	int continued = 0;
	int string_position = 0;
	while(1)
	{
		c = fgetc(ifp);//getting the character, if the character is the end of file then we break out of it
		if(c == EOF)
			break;

		if(c== ' ' || c == '\n')
		{
		    if(symbols[counter].kind == 3)
		    {

                c = fgetc(ifp);
                symbols[counter].val = (c - '0');
                c = fgetc(ifp);
                while(!inc)
                {
                	if(c == ' ' || c == '\n')
            	        inc =1;
                    else
                    {
            	        symbols[counter].val = symbols[counter].val *10 + (c- '0');
                	    c = fgetc(ifp);
                    }
				}
            }
            inc = 0;
            continued = 0;

		}//check if space


		else
		{//if not space its a digit or word

			if(isdigit(c))
			{// if digit
				string_position = 0;
				if(continued == 0)
				{
				    //check if its first number
					counter++;
					continued = 1;
					symbols[counter].kind = c - '0';

				}

				else
				{//if not first number add it
					symbols[counter].kind = symbols[counter].kind * 10 + (c - '0');
				}


			}
			else
			{	//if not a number its a string
				symbols[counter].name[string_position] = c ;
				string_position ++;
				while(x)
				{
					c  = fgetc(ifp);
					if(c == EOF || c == ' ')
						x = 0;
					else
					{
						symbols[counter].name[string_position] = c ;
						string_position ++;
					}

					//printf("%s %d\n\n",symbols[counter].name, counter);
				}
				continued = 0;
			}
		}
		//printf("%d %s\n",symbols[counter].kind,symbols[counter].name);
	}


	return symbols;

}