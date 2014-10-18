/***
Simon Pacheco
COP 3402 (Systems Software)
Homework #2 Lexical Analyzer
***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Struct to represent the input source code as a linked list
typedef struct linked_list
{
	int token_type;
	char line[150];
	struct linked_list *next;
} linked_list;

void print_clean_source(FILE* input, FILE *output);
int print_output(FILE *input, FILE *lex_table, FILE *lex_list);

typedef enum { 
nulsym = 1, identsym, numbersym, plussym, minussym,
multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym, 
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym } token_type;


int main()
{
	
	// Creating the necessary files for the lexical analyzer
	FILE *input = fopen("input.txt", "r");
	FILE *clean_source_output = fopen("cleaninput.txt", "w");
	FILE *lexeme_table = fopen("lexemetable.txt", "w");
	FILE *lexeme_list = fopen("lexemelist.txt", "w");

	print_clean_source(input, clean_source_output);
	fclose(clean_source_output);

	FILE *clean_source_input = fopen("cleaninput.txt", "r");

	int halt_code = print_output(clean_source_input, lexeme_table, lexeme_list);

	return 0;
}

void print_lexemes_list(linked_list *list, FILE *output)
{
	while (list->next != NULL)
	{
		printf("%s", list->line);
		list = list->next;
	}
		
}

int print_output(FILE *input, FILE *lex_table, FILE *lex_list)
{
	char temp[500][150], array[500][150], line[150], *delimiters;
	int i = 0, j, temp_size, length;
	

	while (1)
	{		
		// Break out of loop if the source ends
		if (fgets(line,150, input) == NULL) break;
		// Read the line and copy it to the node
		strcpy(temp[i], line);
		i++;
	}
	temp_size = i;

	j = 0;
	for (i = 0; i < temp_size; i++)
	{
		delimiters = strtok(temp[i], "\t \n");
		while (delimiters != NULL)
		{
			//printf("delimiters.len() = %lu\n", strlen(delimiters));
			strcpy(array[j], delimiters);	
			//printf("temp->line = %s\n", temp->line);
			j++;
			delimiters = strtok(NULL, "\t \n");
		}
	}

	int k;
	for (i = 0; i < j; i++)
	{
		//printf("array = %s\n", array[i]);
		for (k = 0; k < strlen(array[i]); k++)
		{
			//printf("temp[%d][%d] = %c\n", i, k, temp[i][k]);
			if (!isalpha(array[i][k]) && !isdigit(array[i][k]) && !ispunct(array[i][k]) && strlen(array[i]) == 1)
				array[i][0] = '\0';
		}
	}
	int array_len = j;


	linked_list *list = malloc(sizeof(linked_list));
	linked_list *head = list;
	
	for (i = 0; i < array_len; i++)
	{
		//printf("line = %s\n", array[i]);
		char string[150];
		if (strstr(array[i], ";") != NULL || strstr(array[i], ",") != NULL || strstr(array[i], ".") != NULL)
		{
			int x;
			k = 0; j = 0;
			while (array[i][k] != '\0')
			{
				//printf("i:%d\tj:%d\tk:%d\tchar:%c\t", i, j, k, array[i][k]);
				if (isalpha(array[i][k]))
				{
					string[j] = array[i][k];
					j++;
					k++;
				}
				else if (ispunct(array[i][k]))
				{
					list->next = malloc(sizeof(linked_list));
					list = list->next;
					//printf("string:%s\n", string);
					strcpy(list->line, string);
					for (x = 0; x < strlen(string); x++)
						string[x] = ' ';

					if (ispunct(array[i][k+1]))
					{
						list->next = malloc(sizeof(linked_list));
						list = list->next;
						string[0] = array[i][k];
						string[1] = array[i][k+1];
						//printf("string:%s\n", string);
						strcpy(list->line, string);
						for (x = 0; x < strlen(string); x++)
							string[x] = ' ';
						j = 0;
						k+=2;
					}
					else
					{
						list->next = malloc(sizeof(linked_list));
						list = list->next;
						string[0] = array[i][k];
						//printf("String:%s\n", string);
						strcpy(list->line, string);
						for (x = 0; x < strlen(string); x++)
							string[x] = ' ';
						j = 0;
						list->next = malloc(sizeof(linked_list));
						list = list->next;
						k++;
					}
				}
				else if (isdigit(array[i][k]))
				{
					string[j] = array[i][k];
					j++;
					k++;
				}
				else if (isspace(array[i][k]))
					k++;
			}
		}	
		else
		{
			strcpy(list->line, array[i]);
			list->next = malloc(sizeof(linked_list));
			list = list->next;
		}
			
	}
	linked_list *lexemes = head;

	//printf("\n");
	fprintf(lex_table, "lexeme\t\ttoken type\n");
	while (head->next != NULL)
	{
		if (!isalpha(head->line[0]) && !isdigit(head->line[0]) && !ispunct(head->line[0]) && strlen(head->line) == 1)
			head->line[0] = '\0';
		if (head->line[0] != '\0')
		{
			if (strstr(head->line, "const") != NULL)
				head->token_type = constsym;
			else if (strstr(head->line, "var") != NULL)
				head->token_type = varsym;
			else if (strstr(head->line, "procedure") != NULL)
				head->token_type = procsym;
			else if (strstr(head->line, "call") != NULL)
				head->token_type = callsym;
			else if (strstr(head->line, "begin") != NULL)
				head->token_type = beginsym;
			else if (strstr(head->line, "end") != NULL)
				head->token_type = endsym;
			else if (strstr(head->line, "if") != NULL)
				head->token_type = ifsym;
			else if (strstr(head->line, "then") != NULL)
				head->token_type = thensym;
			else if (strstr(head->line, "else") != NULL)
				head->token_type = elsesym;
			else if (strstr(head->line, "while") != NULL)
				head->token_type = whilesym;
			else if (strstr(head->line, "do") != NULL)
				head->token_type = dosym;
			else if (strstr(head->line, "read") != NULL)
				head->token_type = readsym;
			else if (strstr(head->line, "write") != NULL)
				head->token_type = writesym;
			else if (strstr(head->line, ":=") != NULL)
				head->token_type = becomessym;
			else if (strstr(head->line, ".") != NULL)
				head->token_type = periodsym;
			else if (strstr(head->line, ";") != NULL)
				head->token_type = semicolonsym;
			else if (strstr(head->line, ",") != NULL)
				head->token_type = commasym;
			else if (strstr(head->line, ")") != NULL)
				head->token_type = rparentsym;
			else if (strstr(head->line, "(") != NULL)
				head->token_type = lparentsym;				
			else if (strstr(head->line, ">=") != NULL)
				head->token_type = geqsym;
			else if (strstr(head->line, ">") != NULL)
			{
				int ne_found = 0;
				for (i = 0; i < strlen(head->line); i++)
				{
					if (head->line[i] == '<' && head->line[i+1] == '>')
					{
						ne_found = 1;
					}
				}
				if (ne_found == 1)
					head->token_type = neqsym;
				else
					head->token_type = gtrsym;	
			}
				
			else if (strstr(head->line, "<=") != NULL)
				head->token_type = leqsym;
			else if (strstr(head->line, "<") != NULL)
				head->token_type = lessym;
			else if (strstr(head->line, "=") != NULL)
				head->token_type = eqsym;
			else if (strstr(head->line, "odd") != NULL)
				head->token_type = oddsym;
			else if (strstr(head->line, "/") != NULL)
				head->token_type = slashsym;
			else if (strstr(head->line, "*") != NULL)
				head->token_type = multsym;
			else if (strstr(head->line, "-") != NULL)
				head->token_type = minussym;
			else if (strstr(head->line, "+") != NULL)
				head->token_type = plussym;
			else if (isdigit(head->line[0]))
			{
				int is_number = 1;
				for (i = 0; i < strlen(head->line); i++)
					if (isalpha(head->line[i]))
						is_number = 0;
				if (is_number == 1)
					head->token_type = numbersym;
				else
					head->token_type = identsym;
			}
			else
				head->token_type = identsym;

			if (head->token_type == identsym) // Checks if the identifier does not start with a letter or if the name is too long
			{
				if (!isalpha(head->line[0]) )
				{
					if (ispunct(head->line[0]))
					{
						printf("Error: invalid symbol\n");
						return -1;
					}
					else
					{
						printf("Error: variable needs to start with a letter\n");
						return -1;
					}
				}
				if (strlen(head->line) > 11)
				{
					printf("Error: identifier is too long\n");
					return -1;
				}
			}
			else if (head->token_type == numbersym) // Checks a number to see if it is within the range of the allowed length
			{
				if (strlen(head->line) > 5)
				{
					printf("Error: a number cannot be more than 5 digits in length\n");
					return -1;
				}
			}	
			
			fprintf(lex_table, "%s\t\t\t\t\t\t\t%d\n", head->line, head->token_type);
		}
			
		head = head->next;
	}
	linked_list *ll = lexemes;
	while (lexemes->next != NULL)
	{
		if (lexemes->line[0] != '\0')
		{
			if (lexemes->token_type == 2)
			{
				fprintf(lex_list, "%d ", lexemes->token_type);
				fprintf(lex_list, "%s ", lexemes->line);
			}
				
			else if (lexemes->token_type == 3)
			{
				fprintf(lex_list, "%d ", lexemes->token_type);
				int ascii = atoi(lexemes->line);
				fprintf(lex_list, "%d ", ascii);
			}
			else
				fprintf(lex_list, "%d ", lexemes->token_type);
		}
			
		lexemes = lexemes->next;
	}

	return 0; // Return a 0 if there is no errors

	/*
		FOR THIS PROJECT START WITH 2!
	nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,  slashsym = 7, 
	oddsym = 8,  eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, 
	rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, 
	ifsym = 23, thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31, 
	readsym = 32, elsesym = 33.
	*/
}

void print_clean_source(FILE* input, FILE *output)
{
	char line_array[500][150], line[150], *begin_comment = "/*", *end_comment = "*/";
	int i = 0, array_size, j, x, y, z, length, begin_index, end_index;
	while (1)
	{		
		// Break out of loop if the source ends
		if (fgets(line,150, input) == NULL) break;
		// Read the line and copy it to the node
		strcpy(line_array[i], line);
		i++;
	}
	array_size = i;
	
	for (j = 0; j < array_size; j++)
	{
		//printf("%d = %s", j, line_array[j]);
		// Checks the line if there is a comment
		if (strstr(line_array[j], begin_comment) != NULL) // if next_node-> line == "/*"
		{
			if (strstr(line_array[j], end_comment) != NULL) // Checking if the comment is in the same line
			{
				//printf("In the same line!\n");
				length = strlen(line_array[j]);
				for (x = 1; x < length - 1; x++)
				{
					if (line_array[j][x] == '*' && line_array[j][x-1] == '/') // Get the beginning of the comment
					{
						begin_index = x - 1;
						for (y = begin_index + 1; y < length - 1; y++)
							if (line_array[j][y-1] == '*' && line_array[j][y] == '/')
								end_index = y; // Get the end of the comment index
					}	
				}
				for (z = begin_index; z <= end_index; z++)
					line_array[j][z] = ' ';
			}
			else // The comment is not in the same line
			{
				line_array[j][0] = '\0';
				int index = j+1; // Setting i to the next line
				while (1)
				{
					int token_present = 0; // token_present is to check if there is anything after the end comment 
										  // (e.g. */ x token_present would be 0 because there is still stuff present)
					//printf("next_line = %s\n", line_array[index]);
					if (strstr(line_array[index], end_comment) != NULL) // If the end comment is in the line then get the end index
					{
						length = strlen(line_array[index]);
						char character;
						for (x = 1; x < length - 1; x++)
						{
							if (line_array[index][x] == '/' && line_array[index][x-1] == '*')
								end_index = x;
						}
						for (x = end_index+1; x <= length; x++)
						{
							character = line_array[index][x];

							if (isdigit(character) || isalpha(character) || ispunct(character))
							{
								token_present = 1;
								break;
							}
						}
						if (token_present)
						{
							for (x = end_index+1; x < strlen(line_array[index]); x++)
								line_array[index][x] = ' ';
						}
						else
							line_array[index][0] = '\0';
						break;
					}
					else
					{
						//printf("Making it null character\n");
						line_array[index][0] = '\0';
						//printf("line after null character = %s\n", line_array[j]);
					}
					index++;
				}
			}				
		}
	}

	for (j = 0; j < i; j++)
	{
		//int length = strlen(line_array[j]);
		//printf("length[%d] = %d\n\n", j, length);
			fprintf(output, "%s", line_array[j]);
	}
}