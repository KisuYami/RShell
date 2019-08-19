#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "declaraion-parser.h"

char *parse_function_type(char *string) {return 0;} // TODO
char *parse_cast_type(char *string) {return 0;} // TODO
char *parse_array_type(char *string) {return 0;} // TODO

// NULL means not a pointer type
char *parse_token_type(char *string)
{
	static char p_int[] =  "pointer to int";
	static char p_char[] = "pointer to char";
	static char p_void[] = "pointer to void";

	if(strncmp(string, "int*",   4) == 0) return p_int;
	if(strncmp(string, "int *",  5) == 0) return p_int;
	if(strncmp(string, "int",    3) == 0) return NULL;
	if(strncmp(string, "void*",  4) == 0) return p_void;
	if(strncmp(string, "void *", 4) == 0) return p_void;
	if(strncmp(string, "void",   4) == 0) return NULL;
	if(strncmp(string, "char*",  5) == 0) return p_char;
	if(strncmp(string, "char *", 6) == 0) return p_char;
	if(strncmp(string, "char",   4) == 0) return NULL;

	return NULL;
}

void parse_token_string(struct token *this, char *string, int *pos)
{
	int i = 0;
	char tmp[MAX_TOKEN_LENGHT] = {0}, *tmp_type_return = NULL;

	while(*pos < strlen(string)) {

		if((string[*pos] > 'a' && string[*pos] < 'z') ||
		   (string[*pos] > 'A' && string[*pos] < 'Z')) {
			tmp_type_return = parse_token_type(tmp);
			break;
		}

		tmp[i] = string[*pos];

		i++;
		(*pos)++;
	}

	if(tmp_type_return == NULL) strncpy(this->string, tmp, strlen(tmp) - 1);
	else strncpy(this->string, tmp_type_return, strlen(tmp_type_return) + 1);
}

int main(int argc, char *argv[])
{
	if(argc <= 2) return 1; /* TODO: Handle Usage */

	int i, pos = 0;
	char string[MAX_TOKEN_LENGHT];
	struct token stack[MAX_TOKEN] = {{0, {0}}};

	for(i = 1; i < argc; i++) {
		strncat(string, argv[i], strlen(argv[i]));
		strncat(string, " ", 2);
	}
	string[strlen(string)] = '\0';

	parse_token_string(&stack[0], string, &pos);
	parse_token_string(&stack[1], string, &pos);

	printf("%s is %s\n", stack[1].string, stack[0].string);

	return 0;
}
