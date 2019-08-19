#ifndef SHELL_HEADER

#define MAX_PATH 1024
#define MAX_COMMAND_LINE 1024
#define COMMAND_BUF_SIZE 1024
#define TOKEN_BUFSIZE 64
#define TOKEN_DELIM "\t\r\n\a"

void shell_input(char *command);
void shell_fork(char **command_argv);
void shell_parser_token(char *command_string, char **argv_buf);

#define SHELL_HEADER
#endif /* ifndef SHELL_HEADER */
