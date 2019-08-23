#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "shell.h"
#include "jobs.h"

struct shell_list shell_builtin_list[] = {
	{5, "clean",  shell_builtin_clean   },
	{2, "cd",     shell_builtin_cd      },
	{1, "q",      shell_builtin_exit    },
	{2, "fg",     shell_builtin_fg      },
	{2, "bg",     shell_builtin_bg      },
	{3, "pwd",    shell_builtin_pwd     },
	{6, "printf", shell_builtin_printf  }
};

void shell_command_loop() 
{
	char *command = NULL;
	char **command_argv = NULL;
	int built_flag = 0;

	command = malloc(sizeof(char) * START_COMMAND_SIZE);
	command_argv = malloc(sizeof(char *) * COMMAND_BUF_SIZE);

	setpgrp(); // Set group ID

	strncpy(process_parent.name, "RShell", 6);
	process_parent.number = 0;
	process_parent.pid = getpgrp();

	signal(SIGINT, SIG_IGN);

	if(command_argv == NULL || command == NULL) {
		perror("RShell: Memory Allocation Failed\n");
		exit(1);
	}
#ifdef DEBUG
	printf("RShell Debug: Entering main loop.\n");
	printf("RShell Debug: father gpid is %d\n", process_parent.pid);
#endif


	while(1) { // Main loop

		shell_command_input(command);
		shell_command_parser(command, command_argv);
		shell_command_builtin(command_argv, &built_flag);
		shell_command_fork(command_argv, &built_flag);
	}

	free(command_argv);
	free(command);
}

void shell_command_prompt()
{
	char path[MAX_PATH] = {0};

	getcwd(path, MAX_PATH);

	write(STDIN_FILENO, "[ rshell - ", 11);
	write(STDIN_FILENO, path, strlen(path));
	write(STDIN_FILENO, " ] $ ", 5);
}

void shell_command_input(char *command)
{
	int count = 0;
	int command_size;

	char c;
	char *command_ptr = NULL;

	shell_command_prompt();
	command_size = START_COMMAND_SIZE;

	while(1) {
		switch(read(STDIN_FILENO, &c, 1)) { /* XXX */
			case 0:
				command[count] = '\0';
				break;

			default:
				if(c == '\n') {
					command[count] = '\0';
					if(count == 0) {
						shell_command_prompt();
						break;
					} else if(count > 0) return;
				}
				if (count >= command_size) {
					command_size += START_COMMAND_SIZE;
					command_ptr = realloc(command, command_size);
					if(command_ptr != NULL) command = command_ptr;
#ifdef DEBUG
					printf("RShell DEBUG: Allocatin more %dbytes for command_argv, using \
							%dbytes\n", COMMAND_BUF_SIZE, command_size);
#endif
				}
				command[count++] = c;
				break;
				
		}
	}

}

void shell_command_parser(char *command_string, char **argv_buf)
{

	int position = 0;
	int command_size;
	char *token = NULL;
	char **command_ptr = NULL;

	memset(argv_buf, 0, COMMAND_BUF_SIZE);
	command_size = START_COMMAND_SIZE;

	token = strtok(command_string, TOKEN_DELIM);
	while(token != NULL) {

		if(position > command_size) { // Handle Resizing
			command_size += COMMAND_BUF_SIZE;
			command_ptr = realloc(argv_buf, sizeof(char) * command_size);
#ifdef DEBUG
			printf("RShell DEBUG: Allocatin more %dbytes for command_argv, using \
					%dbytes\n", COMMAND_BUF_SIZE, command_size);
#endif

			if(command_ptr != NULL) argv_buf = command_ptr;
			else if(command_ptr == NULL) printf("RShell: failed to allocate memory for token\n");
		};

		argv_buf[position] = token;
		token = strtok(NULL, TOKEN_DELIM);
		position++;
	}
}

void shell_command_builtin(char **command_argv, int *flag)
{
	int i;

	for(i = 0; i < BUILTIN_NUMBER; i++) {
		if(strncmp(command_argv[0], shell_builtin_list[i].option,
					shell_builtin_list[i].size) == 0) {
			shell_builtin_list[i].func(command_argv);
			*flag = 1;
			return;
		}
	}
	flag = 0;
}

void shell_command_fork(char **command_argv, int *flag)
{

	if(*flag == 1) {
		*flag = 0;
		return;
	}

	pid_t pid;
	pid_t foreground;
	int status;

	// U ugly
	pid = fork();

	if(pid == 0) { // Child process

		foreground = tcgetpgrp(STDIN_FILENO);
		setpgid(pid, foreground);
		tcsetpgrp(STDIN_FILENO, foreground);

		process_child_register(command_argv[0]);

		if(execvp(command_argv[0], command_argv) == -1) {
			printf("RShell: I can't hear you!\n");
		}

		exit(EXIT_FAILURE);
    } 

	else if (pid < 0) perror("RShell: child failed execution");

	else {
		do {
			waitpid(pid, &status, WUNTRACED);

			if(WIFEXITED(status)) {
#ifdef DEBUG
				printf("RShell Debug: normal exit\n");
#endif
				break;
			}
			else if(WIFSTOPPED(status)) {
#ifdef DEBUG
				printf("RShell Debug: child received sigstop\n");
#endif
				signal_sigstop(pid);
				break;
			}

		} while (1);
	}
}

void shell_builtin_printf(const char **argv) {

	int count = 1;
	while(argv[count] != NULL) printf("%s ", argv[count++]);

}

void shell_builtin_cd(const char **argv) {
	chdir(argv[1]);
}

void shell_builtin_pwd(const char **argv) {

	char pwd[MAX_PATH] = {0};

	getcwd(pwd, MAX_PATH);
	pwd[strlen(pwd)] = '\n';

	write(1, pwd, strlen(pwd));
}

void shell_builtin_clean(const char **argv) {
#ifdef DEBUG
	printf("RShell Debug: shell_builtin_clean called\n");
#endif
	write(STDIN_FILENO, "\033[1J", 5); // Got to go fast
	write(STDIN_FILENO, "\033[1H", 5);
}

void shell_builtin_bg(const char **argv) {
	printf("%d - %s - %d\n", process_child_list.number,
			process_child_list.name, process_child_list.pid);
}

void shell_builtin_fg() {

	int status;
	if(kill(process_child_list.pid, 0) != ESRCH) {
		kill(process_child_list.pid, SIGCONT);

		tcsetpgrp(STDIN_FILENO, process_child_list.pid);

		do {
			waitpid(process_child_list.pid, &status, WUNTRACED);
#ifdef DEBUG
			if(WIFEXITED(status)) {
				printf("RShell Debug: normal exit\n");
				break;
			}
#endif
			if(WIFSTOPPED(status)) {
#ifdef DEBUG
				printf("RShell Debug: child received sigstop\n");
#endif
				signal_sigstop(process_child_list.pid);
				break;
			}

		} while (!WIFSIGNALED(status));
#ifdef DEBUG
		printf("RShell Debug: shell_builtin_fg called on pid %d\n", 
			process_child_list.pid);
#endif
	}
}

void shell_builtin_exit(const char **argv) {
#ifdef DEBUG
	printf("RShell Debug: shell_builtin_exit called\n");
#endif
	exit(0);
}
