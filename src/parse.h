#ifndef PARSE_H
#define PARSE_H

#include <stddef.h>

#define INPUT_NODE_T_DELIMITER " \t\r\t\b\0"

enum
{
	NODE_NORMAL = 0,

	NODE_EXEC_LIST    = 1,
	NODE_EXEC_ASYNC   = 2,
	NODE_EXEC_BUILTIN = 4,

	NODE_REDIRECTION        = 8,
	NODE_REDIRECTION_IN     = 16,
	NODE_REDIRECTION_OUT    = 32,
	NODE_REDIRECTION_ERR    = 64,
	NODE_REDIRECTION_DUP    = 128,
	NODE_REDIRECTION_PIPE   = 256,
	NODE_REDIRECTION_READ   = 512,
	NODE_REDIRECTION_CREAT  = 1024,
	NODE_REDIRECTION_APPEND = 2048,
};

struct node
{
	pid_t pid;
	int flags;

	size_t size;
	char *command[50];
	struct node *next;
};

typedef struct node node_t;

node_t *
parse_input(char *command_string);

void
escape_char(char *string, char escape);

int
get_type(char *token);

#endif /* PARSE_H */
