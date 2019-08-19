#ifndef DECLARATION_PARSER_HEADER
#define DECLARATION_PARSER_HEADER

#define MAX_TOKEN_LENGHT 25
#define MAX_TOKEN 10

struct token {
	int type;
	char string[MAX_TOKEN_LENGHT];
};

void parse_token_string(struct token *this, char *string, int *pos);
char *parse_token_type(char *string);

#endif /* ifndef DECLARATION_PARSER_HEADER */
