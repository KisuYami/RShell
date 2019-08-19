#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "shell.h"

void shell_input(char *command)
{
	int count = 0;

	printf("> ");
	while((command[count++] = getchar()) != '\n') {
		if(count > MAX_COMMAND_LINE) break;
	}
	command[count - 1] = '\0';
}

void shell_fork(char **command_argv)
{
	pid_t pid;
	int status;

	pid = fork();
	if(pid == 0) {
    // Child process
	if(execvp(command_argv[0], command_argv) == -1) {
		perror("RShell: child failed execution");
	}
	exit(EXIT_FAILURE);
    } else if (pid < 0) { // Error forking
		perror("RShell: child failed execution");
	} else { // Parent process
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}

void shell_parser_token(char *command_string, char **argv_buf)
{
	if(command_string[0] == '\0') return; // Don't process empty string

	int position = 0;
	char *token = NULL;

	memset(argv_buf, 0, COMMAND_BUF_SIZE);

	token = strtok(command_string, TOKEN_DELIM);
	while(token != NULL) {
		argv_buf[position] = token;
		position++;
		token = strtok(NULL, TOKEN_DELIM);
	}
}

void shell_pwd(char *dst) {} // TODO
void shell_cd(char *dst) {} // TODO
void shell_bg(char *dst) {} // TODO
void shell_fg(char *dst) {} // TODO
void shell_kill(char *dst) {} // TODO
void shell_printf(char *dst) {} // TODO
void shell_user() {} //TODO
