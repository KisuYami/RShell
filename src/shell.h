#ifndef SHELL_HEADER
#define SHELL_HEADER

/******************************************************************************
* File:             shell.h
* Author:           Reberti Carvalho Soares
* Created:          09/03/19
* Description:      User input handling.
*****************************************************************************/

#define MAX_INPUT_SIZE 50
#define MAX_node_t_NUMBER 10

#include <stddef.h>
#include <termios.h>

#include "parse.h"

void
set_history_file(void);

char *
print_prompt(void);


void
exec_command(node_t *command, int i, int fd[2]);

void
exec_command_redirection(node_t *);
#endif /* SHELL_HEADER */
