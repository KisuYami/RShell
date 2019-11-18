#ifndef PARSE_H
#define PARSE_H

#include <stddef.h>

#define INPUT_node_t_DELIMITER " \t\r\t\b\0"

enum
{
	NODE_NORMAL = 0,
    NODE_PIPE = 1,
    NODE_READ = 2,
    NODE_CREAT = 4,
    NODE_ASYNC = 8,
    NODE_APPEND = 16,
    NODE_BUILTIN = 32,
    NODE_REDIRECTION = 64,
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

int
get_type(char *token);

int
check_string(char *string);

#endif /* PARSE_H */
