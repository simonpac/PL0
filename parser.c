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

// Declaration of Error Codes
typedef enum 
{
	equal_instead_of_becomes = 1,
	equal_must_follow_number = 2,
	identifier_must_follow_equal = 3,
	cvp_must_be_followed_by_identifier = 4,
	semi_or_comma_missing = 5,
	incorrect_sym_after_proc = 6,
	statement_expected = 7,
	incorrect_sym_after_block = 8,
	period_expected = 9,
	semi_missing_between_statements = 10,
	undeclared_identifier = 11,
	assign_to_cons_or_proc = 12,
	assignment_expected = 13,
	call_must_follow_ident = 14,
	meaningless_callvar = 15,
	then_expected = 16,
	missing_semi_or_rightbrac = 17,
	do_expected = 18,
	incorrect_sym_after_state = 19,
	relation_expected = 20,
	expression_contains_proc = 21,
	right_paren_missing = 22,
	prec_factor_wrong_beginning = 23,
	expression_wrong_beginning = 24,
	num_too_large = 25,
	right_brack_missing_at_end = 26,
	wrong_storing = 27
} errorCode;

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
void generate_error(errorCode code);
symbol symbols_table[1000];
int increment_program_counter();
int get_program_counter();
void get_prev_instruction(int index, int opcode, int m);

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
				//printf("%d\n",symbols[counter].kind);
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

int increment_program_counter(){

	return program_counter++;

}

int get_program_counter(){
	return program_counter;

}

void get_prev_instruction(int index, int opcode, int m){
	mcode[index].op = opcode;
	mcode[index].l = 0;
	mcode[index].m = m;
	//printf("%d %d %d\n\n", mcode[index].op,mcode[index].l,mcode[index].m);


}

int main()
{
	int ab =0;
	symbol *symbols = read_file();
	create_symbol(symbols);
	printf("Hello!\n");

	program(symbols);
	FILE *ifp = fopen("mcode.txt","w");
	for(ab= 0; ab< program_counter; ab++){
		fprintf(ifp,"%d %d %d\n\n", mcode[ab].op,mcode[ab].l,mcode[ab].m);


	}
	return 0;
}

void program(symbol *symbols)
{
	//printf("\t\tline 71\n");
	get_token(symbols);
	block(symbols);

	if (token != periodsym)
	{
		//if (token != nulsym)
			//generate_error(incorrect_sym_after_block);
		//else
			generate_error(period_expected);
	}
	generate_code(halt, 0, 3);
}

void generate_code(int op, int l, int m)
{
	mcode[program_counter].op = op;
	mcode[program_counter].l = l;
	mcode[program_counter].m = m;
	//printf("%d %d %d\n\n", mcode[program_counter].op,mcode[program_counter].l,mcode[program_counter].m);
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
				generate_error(cvp_must_be_followed_by_identifier);
			}
			//printf("\t\tline 96\n");
			get_token(symbols);
			if (token != eqsym)
			{
				if (token == becomessym) {
					generate_error(equal_instead_of_becomes);
				}
				else {
					generate_error(identifier_must_follow_equal);
				}
			}
			//printf("\t\tline 103\n");
			get_token(symbols);
			if (token != numbersym)
			{
				generate_error(equal_must_follow_number);
			}
			//printf("\t\tline 110\n");
			get_token(symbols);
			//get_token(symbols);
		} while (token == commasym);
		if (token != semicolonsym)
		{
			generate_error(semi_or_comma_missing);
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
				generate_error(cvp_must_be_followed_by_identifier);
			}
			//printf("\t\tline 132\n");
			get_token(symbols);
		} while (token == commasym);

		if (token != semicolonsym)
		{
			generate_error(semi_or_comma_missing);
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
	int temp1 = 0;
	int temp_program_counter[2];
	if (token == identsym)
	{
		if(!find_variable(symbols[token_num-1].name))
		{
			// CHECK THE LOGIC HERE MIGHT NOT BE NOT SIGN (!)
			if(!find_constant(symbols[token_num-1].name)) {
				generate_error(assign_to_cons_or_proc);
			}
			else {
				generate_error(undeclared_identifier);
			}
		}
		//printf("\t\tline 178\n");
		get_token(symbols);
		//printf("token = %d\n", token);
		if (token != becomessym)
		{
			generate_error(assignment_expected);
		}
		//printf("\n%d M1 \n",m1);
		temp1 = m1;
		//printf("\t\tline 185\n");
		get_token(symbols);
		expression(symbols);
		//printf("\n%d M1 \n",m1);
		generate_code(sto, 0, temp1);
	}
	else if (token == callsym)
	{
		//printf("\t\tline 191\n");
		get_token(symbols);
		if (token != identsym)
		{
			generate_error(call_must_follow_ident);
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
		if (token != endsym && (token != beginsym && token 
			!= whilesym && token != ifsym && token != identsym))
		{
			generate_error(missing_semi_or_rightbrac);
		}
		else if (token == identsym || token == ifsym ||
			token == whilesym || token == beginsym) {
			generate_error(semi_missing_between_statements);
		} 
		//printf("\t\tline 220\n");
		get_token(symbols);
	}
	else if (token == ifsym)
	{
		temp_program_counter[1] = 0;
		//printf("\t\tline 225\n");
		get_token(symbols);
		condition(symbols);
		temp_program_counter[0] = increment_program_counter();//temp label is equal to PC +1


		if (token != thensym)
		{
			generate_error(then_expected);
		}
		//printf("\t\tline 233\n");
		get_token(symbols);
		statement(symbols);

		if(token == elsesym){
			temp_program_counter[1] = increment_program_counter();
			program_counter--;
			get_token(symbols);

		}

		get_prev_instruction(temp_program_counter[0],jpc,get_program_counter());
		if(temp_program_counter[1]){
			statement(symbols);
			get_prev_instruction(temp_program_counter[1],jmp,get_program_counter());

		}

	}
	else if (token == whilesym)
	{

		//printf("\t\tline 238\n");
		get_token(symbols);

		temp_program_counter[0] = get_program_counter();

		condition(symbols);
		
		temp_program_counter[1] = increment_program_counter();
		
		if (token != dosym)
		{
			generate_error(do_expected);
		}
		//printf("\t\tline 247\n");
		get_token(symbols);
		statement(symbols);

		generate_code(jmp, 0, program_counter);
		get_prev_instruction(temp_program_counter[1], jpc, get_program_counter());
	}
	else if (token == readsym)
	{
		generate_code(read_input, 0, 2);
		get_token(symbols);

		if (token == identsym)
		{
			if (find_variable(symbols[token_num - 1].name))
			{
				generate_code(sto, 0, m1);
			}
			else if (find_constant(symbols[token_num - 1].name))
			{
				generate_error(wrong_storing);
			}
			else
				generate_error(undeclared_identifier);
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
			else
			{
				generate_error(undeclared_identifier);
			}
		}
		get_token(symbols);
		generate_code(print_top_stack, 0, 1);
	}
	else if (token == endsym)
		return;
	else {
		if (token == periodsym);
		else
			generate_error(statement_expected);
	}
}

void generate_error(errorCode code){
  printf("Error #%d at token %d: ", code, token_num);

  switch(code){
  case(equal_instead_of_becomes): // Used
    printf("Use = instead of :=.\n");
    break;
  case(equal_must_follow_number): // Used
    printf("= must be followed by a number.\n");
    break;
  case(identifier_must_follow_equal): // Used
    printf("Identifier must be followed by a number.\n");
    break;
  case(cvp_must_be_followed_by_identifier): // Used
    printf("const, var, procedure must be followed by identifier.\n");
    break;
  case(semi_or_comma_missing): // Used
    printf("Semicolon or comma missing.\n");
    break;
  case(incorrect_sym_after_proc): // Used
    printf("Incorrect symbol after procedure declaration.\n");
    break;
  case(statement_expected): // Used
    printf("Statement expected.\n");
    break;
  case(incorrect_sym_after_block): // Used
    printf("Incorrect symbol after statement part in block.\n");
    break;
  case(period_expected): // Used
    printf("Period expected.\n");
    break;
  case(semi_missing_between_statements): // Used
    printf("Semicolon between statements missing.\n");
    break;
  case(undeclared_identifier):
    printf("Undeclared identifier.\n");
    break;
  case(assign_to_cons_or_proc): // Used
    printf("Assignment to constant or procedure is not allowed.\n");
    break;
  case(assignment_expected): // Used
    printf("Assignment operator expected.\n");
    break;
  case(call_must_follow_ident): // Used
    printf("callsym must be followed by an identifier.\n");
    break;
  case(meaningless_callvar): // NO
    printf("Call of a constant or variable is meaningless.\n");
    break;
  case(then_expected): // Used
    printf("thensym expected.\n");
    break;
  case(missing_semi_or_rightbrac): // Used
    printf("Semicolon or } expected.\n");
    break;
  case(do_expected): // Used
    printf("dosym expected.\n");
    break;
  case(incorrect_sym_after_state): // Used parallel to missing_semi_or_rightbrac
    printf("Incorrect symbol following statement.\n");
    break;
  case(relation_expected): // Used
    printf("Relational operator expected.\n");
    break;
  case(expression_contains_proc): // Used
    printf("Expression must not contain a procedure identifier.\n");
    break;
  case(right_paren_missing): // Used
    printf("Right parenthesis missing.\n");
    break;
  case(prec_factor_wrong_beginning): // Used
    printf("The preceding factor cannot begin with this symbol.\n");
    break;
  case(expression_wrong_beginning):
    printf("An expression cannot begin with this symbol.\n");
    break;
  case(num_too_large): // NO?
    printf("This number is too large.\n");
    break;
  case(right_brack_missing_at_end):
    printf("No right bracket at the end of the program\n");
    break;
  case(wrong_storing):
    printf("Cannot store a value in a constant or procedure.\n");
    break;
  default:
    printf("Improper error code.\n");
    break;
  }
  exit(1);
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
			if (token == becomessym) {
				generate_error(equal_instead_of_becomes);
			}
			generate_error(relation_expected);
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

	if (token != plussym && token != minussym && token != numbersym && token != lparentsym 
		&& token != identsym) {
		generate_error(expression_wrong_beginning);
	}

	if (token == plussym || token == minussym)
	{
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
		else if (find_variable(symbols[token_num-1].name))
			generate_code(lod, 0, m1);
		else
			generate_error(undeclared_identifier);
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
			generate_error(right_paren_missing);
		}
		//printf("\t\tline 324\n");
		get_token(symbols);
	}
	else
	{
		generate_error(prec_factor_wrong_beginning);
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