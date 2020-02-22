#include <wordexp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parse.h"
#include "mem.h"
#include "err.h"

node_t *
parse_input(char *command_string)
{
	if(*command_string == '&')
		return NULL;

	node_t	 *list_head = init_node_list();
	node_t	 *list_ptr  = list_head;
	wordexp_t parsed_expression;

	char *string_buffer = malloc(strlen(command_string) * 2);
	strcpy(string_buffer, command_string);

	escape_char(string_buffer, '|');
	escape_char(string_buffer, '>');
	escape_char(string_buffer, '<');
	escape_char(string_buffer, '&');

	switch(wordexp(string_buffer, &parsed_expression, 0))
	{
	case WRDE_BADCHAR:
		printf("RShell: Illegal occurrence of <, >, (, ), {, }\n");
		clean_node_list(list_head);
		return NULL;

	case WRDE_SYNTAX:
		printf("RShell: Unbalanced parentheses or unmatched quotes\n");
		clean_node_list(list_head);
		return NULL;

	default:

		list_ptr->command = calloc(parsed_expression.we_wordc+1,
					   sizeof(char **));

		for(size_t i = 0; i < parsed_expression.we_wordc; ++i)
		{
			list_ptr->flags = get_type(parsed_expression.we_wordv[i]);

			if(list_ptr->flags != 0)
			{
				list_ptr->next = init_node_list();
				list_ptr = list_ptr->next;
				list_ptr->command =
					calloc(parsed_expression.we_wordc,
					       sizeof(char **));
			}

			else
			{
				list_ptr->command[list_ptr->size] =
					calloc(1024, sizeof(char));

				strcpy(list_ptr->command[list_ptr->size++],
				       parsed_expression.we_wordv[i]);
			}
		}

		wordfree(&parsed_expression);
	}

	free(string_buffer);

	return list_head;
}

int
get_type(char *token)
{

	switch(*token)
	{
	case '|': return NODE_REDIRECTION_PIPE;
	case '&': return NODE_EXEC_ASYNC;
	case '<': return (NODE_REDIRECTION | NODE_REDIRECTION_READ);

	case '>':
		if(strncmp(token, ">>", 2) == 0)
			return (NODE_REDIRECTION_APPEND | NODE_REDIRECTION);

		else if(strncmp(token, ">&", 2) == 0)
			return (NODE_REDIRECTION_DUP | NODE_REDIRECTION);

		else
			return (NODE_REDIRECTION_CREAT	| NODE_REDIRECTION);
	}
	return NODE_NORMAL;
}

// TODO: don't escape chars that are insido of quotes
void
escape_char(char *string, char escape)
{
	char *string_ptr = strchr(string, escape);
	size_t string_length = strlen(string);

	while(string_ptr)
	{
		for(int i = string_length; &string[i] != string_ptr; --i)
			string[i] = string[i-1];

		string_ptr[0] = '\\';
		string[string_length+1] = '\0';

		string_ptr = strchr(string_ptr+2, escape);
		string_length = strlen(string);
	}
}
