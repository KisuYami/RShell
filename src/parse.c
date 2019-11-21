#include <wordexp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parse.h"
#include "mem.h"

node_t *
parse_input(char *command_string)
{

    node_t *list_head   = init_node_list();
    node_t *list_ptr    = list_head;

	if(!list_head)
		return NULL;

    char *token = strtok(command_string, INPUT_node_t_DELIMITER);

    if(*token == ':')
    {
        char *tmp_token = getenv(token+1);

		if(token[1] == '\0')
		{
			fprintf(stderr, "RShell: missing variable after \':\'\n");
			clean_node_list(list_head);
			return NULL;
		}

        if(!tmp_token)
        {
            fprintf(stderr, "RShell: the \'%s\' alias isn't set, trying command in PATH\n", token+1);
			token++;
        }
        else
            token = tmp_token;
    }

    while(token)
    {
        list_ptr->flags = get_type(token);

		char *s = strchr(token, '\'');
		if(s && s != token && s == strrchr(token, '\''))
			strcat(token, "\'");

		s = strchr(token, '\"');
		if(s && s != token && s == strrchr(token, '\"'))
			strcat(token, "\"");

        if((list_ptr->flags & NODE_ASYNC) && list_ptr->size == 0)
        {
            clean_node_list(list_head);
            return NULL;
        }

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
                char parse[] = {*token , '\0'};
                char *tmp_string = calloc(1024, sizeof(char));

                strcpy(tmp_string, token);
                strcat(tmp_string, " ");

                token = strtok(NULL, parse);

                if(!token)
                {
                    strcat(tmp_string, &parse[0]);
                    tmp_string[strlen(tmp_string)] = '\0';
                }

                else
                {
                    strcat(tmp_string, token);
                    strcat(tmp_string, &parse[0]);
                    tmp_string[strlen(tmp_string)] = '\0';
                }

                wordexp(tmp_string, &parsed_expression, 0);
                free(tmp_string);
            }

            else
                wordexp(token, &parsed_expression, 0);

            for (size_t i = 0; i < parsed_expression.we_wordc; ++i)
            {
                list_ptr->command[list_ptr->size] = malloc(1024);

                if(list_ptr->command[list_ptr->size] == NULL)
                {
                    printf("RShell: Failed to allocate memory\n");
					clean_node_list(list_head);
					return NULL;
                }

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
    case '|': return NODE_PIPE;
    case '&': return NODE_ASYNC;
    case '<': return (NODE_REDIRECTION | NODE_READ);

    case '>':
        if(strncmp(token, ">>", 2) == 0)
			return (NODE_APPEND | NODE_REDIRECTION);

        else if(strncmp(token, ">&", 2) == 0)
			return (NODE_DUP | NODE_REDIRECTION);

        else
			return (NODE_CREAT  | NODE_REDIRECTION);
    }
    return NODE_NORMAL;
}
