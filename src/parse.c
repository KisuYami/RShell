#include <wordexp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parse.h"
#include "shell.h"
#include "mem.h"

struct TOKEN *
parse_input(char *command_string)
{
    char *token      = NULL;
    char *tmp_string = NULL;

    size_t position       = 0;

    wordexp_t parsed_expression;

    struct TOKEN *list_head = NULL;
    struct TOKEN *list_ptr  = NULL;

    list_head   = init_TOKEN_list();
    list_ptr    = list_head;

    token = strtok(command_string, INPUT_TOKEN_DELIMITER);

    while(token)
    {

        switch(*token)
        {
        case '|':

            list_ptr->flags |= PIPE;
            break;

        case '>':

            if(strncmp(token, ">>", 2) == 0) list_ptr->flags |= APPEND | REDIRECTION;
            else                             list_ptr->flags |= CREAT  | REDIRECTION;
            break;

        case '<':

            list_ptr->flags |= REDIRECTION | READ;
            break;

        case '&':

            if(position == 0)
            {
                clean_TOKEN_list(list_head);
                return NULL;
            }

            list_ptr->flags |= CHILD_BACKGROUND;
            break;

        case '\'':
        case '\"':

            parse_string(list_ptr, token, &position);
            token = strtok(NULL, INPUT_TOKEN_DELIMITER);
            position++;
            break;

        default:
        {
            char *dot = NULL;

            /* There is no reason to have more than 2 of those(',") in one token */
            dot = strchr(token, '\'');

            // XXX
            if(dot != NULL && dot != strrchr(token, '\''))
                for(int i = 0; dot[i+1] != '\0'; dot[i] = dot[i+1], i++);

            else
            {
                dot = strchr(token, '\"');

                if(dot != NULL && dot != strrchr(token, '\"'))
                    for(int i = 0; dot[i+1] != '\0'; dot[i] = dot[i+1], i++);
            }

            wordexp(token, &parsed_expression, 0);

            for (size_t i = 0; i < parsed_expression.we_wordc; ++i)
            {
                tmp_string = malloc(sizeof(char) * strlen(parsed_expression.we_wordv[i]) + 1);

                if(tmp_string == NULL)
                {
                    printf("RShell: Failed to allocate memory\n");
                    longjmp(prompt_jmp, 1);
                }

                strcpy(tmp_string, parsed_expression.we_wordv[i]);
                list_ptr->command[position++] = tmp_string;
            }

            wordfree(&parsed_expression);
            token = strtok(NULL, INPUT_TOKEN_DELIMITER);

            break;
        } /* default */

        } /* switch(*token) */

        if(list_ptr->flags != 0)
        {
            list_ptr->size = position;
            list_ptr->next = init_TOKEN_list();
            list_ptr       = list_ptr->next;

            position = 0;

            token = strtok(NULL, INPUT_TOKEN_DELIMITER);
        }

    } /* while(token) */

    list_ptr->size = position;

    return list_head;
}

void
parse_string(struct TOKEN *list_ptr, char *token, size_t *position)
{

    size_t  count           =  0;
    size_t  tmp_size_token  =  64 * 64; // Elevate by 2
    char   *tmp_string      =  malloc(sizeof(char) * tmp_size_token);
    char    string_literal  = *token;

    if(tmp_string == NULL)
    {
        printf("RShell: Failed to allocate memory\n");
        longjmp(prompt_jmp, 1);
    }

    // Remove the " or ' in the start of the first token
    token++;
    strcpy(tmp_string, token);

    while(1)
    {
        if(*token == string_literal || token[strlen(token) - 1] == string_literal) {

            char *dot = strchr(tmp_string, string_literal);
            dot[0] = '\0'; // Remove trailing string_literal

            break;
        }

        token = strtok(NULL, INPUT_TOKEN_DELIMITER);

        if(token == NULL)
        {
            printf("RShell: missing ending %c\n", string_literal);
            break;
        }

        // Reallocation of memory
        count += strlen(token);

        if(count >= tmp_size_token - 10)
        {
            tmp_size_token = count * count;
            tmp_string = realloc_string(tmp_string,
                                        tmp_size_token * sizeof(char));
        }

        strcat(tmp_string, " ");
        strcat(tmp_string, token);

    } // While loop

    list_ptr->command[*position] = tmp_string;

    return;
}
