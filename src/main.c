#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

int main(int argc, char *argv[])
{
	char *command = NULL;
	char **command_argv = NULL;

	command = malloc(sizeof(char) * MAX_COMMAND_SIZE);
	command_argv = malloc(sizeof(char *) * COMMAND_BUF_SIZE);

	if(command_argv == NULL || command == NULL) 
		perror("RShell: Memory Allocation Failed");

	while(1 == 1) { // Main loop

		shell_command_input(command);
		shell_command_parser(command, command_argv);
		if(shell_command_builtin(command_argv) == 0)
			shell_command_fork(command_argv);

	}

	free(command_argv);
	free(command);

	return 0;
}
