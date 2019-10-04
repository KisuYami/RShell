#ifndef SHELL_HEADER
#define SHELL_HEADER

/******************************************************************************
* File:             shell.h
* Author:           Reberti Carvalho Soares
* Created:          09/03/19
* Description:      User input handling.
*****************************************************************************/

#define MAX_INPUT_SIZE 50
#define MAX_TOKEN_NUMBER 10
#define INPUT_TOKEN_DELIMITER " \t\r\t\b"

enum TOKEN_FLAGS {
	PIPE = 1,
	REDIRECTION = 2,
	CREAT = 4,
	APPEND = 8,
	READ = 16,
	BUILTIN = 32,
	CHILD_SIG_STOP = 64,
	CHILD_BACKGROUND = 128
};

struct TOKEN {
	pid_t pid;
	int flags;
	unsigned int size;

	char **command;
	struct TOKEN *next;
};

struct TOKEN *list_head;
struct TOKEN *parse_input(char *command_string);

char *print_prompt(void);

void exec_command(struct TOKEN *command, int i, int fd[2]);
void exec_command_redirection(struct TOKEN *head);
#endif /* SHELL_HEADER */
