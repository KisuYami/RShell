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

#define MODE_PERMISSIONS S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define MODE_APPEND O_WRONLY | O_CREAT | O_APPEND
#define MODE_CREATE O_WRONLY | O_CREAT | O_TRUNC

#include <stddef.h>
#include <termios.h>

#include "parse.h"

void
set_history_file(void);

char *
print_prompt(void);

void
exec_command(node_t *command);

void
exec_command_redirection(node_t *);
#endif /* SHELL_HEADER */
