#include <wordexp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parse.h"
#include "mem.h"

node_t *
parse_input(char *command_string)
{
	if(*command_string == '&')
			return NULL;

	node_t *list_head = init_node_list();
	node_t *list_ptr  = list_head;
	char   *token     = strtok(command_string, INPUT_node_t_DELIMITER);

	while(token)
	{
		list_ptr->flags = get_type(token);

		char *s = strchr(token, '\'');
		if(s && s != token && s == strrchr(token, '\''))
			strcat(token, "\'");

		s = strchr(token, '\"');
		if(s && s != token && s == strrchr(token, '\"'))
			strcat(token, "\"");

		if(list_ptr->flags != 0)
		{
			list_ptr->next = init_node_list();

			list_ptr = list_ptr->next;
			token = strtok(NULL, INPUT_node_t_DELIMITER);
		}

		else // the real parser code
		{
			wordexp_t parsed_expression;

			if((token[0] == '\'' || token[0] == '\"') &&
			   (strlen(token) == 1 || token[strlen(token)-1] != token[0]))
			{
				char parse[] = {*token , '\0'}; // delimiter: "\'\0" or "\"\0"
				char tmp_string[1024];

				strcpy(tmp_string, token);
				strcat(tmp_string, " ");

				token = strtok(NULL, parse);

				if(token)
					strcat(tmp_string, token);

				strcat(tmp_string, &parse[0]);
				tmp_string[strlen(tmp_string)] = '\0';

				wordexp(tmp_string, &parsed_expression, 0);
			}

			else
				wordexp(token, &parsed_expression, 0);

			for(size_t i = 0; i < parsed_expression.we_wordc; ++i)
			{
				list_ptr->command[list_ptr->size] = malloc(1024);
				strcpy(list_ptr->command[list_ptr->size++],
				       parsed_expression.we_wordv[i]);
			}

			wordfree(&parsed_expression);
			token = strtok(NULL, INPUT_node_t_DELIMITER);
		}

	} /* while(token) */

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
