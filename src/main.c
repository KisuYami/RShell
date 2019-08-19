#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

int main(int argc, char *argv[])
{
	char *command = NULL;
	char **command_argv = NULL;

	command = malloc(sizeof(char) * MAX_COMMAND_LINE);
	command_argv = malloc(sizeof(char *) * MAX_COMMAND_LINE);

	if(command_argv == NULL || command == NULL) 
		perror("RShell: Memory Allocation Failed");

	while(1 == 1) { // Main loop

		shell_input(command);

		if(strncmp(command, "q", 1) == 0) break;

		shell_parser_token(command, command_argv);
		shell_fork(command_argv);

	}

	free(command_argv);
	free(command);

	return 0;
}
