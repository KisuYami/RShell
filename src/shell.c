#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "shell.h"

struct shell_list shell_builtin_list[] = {
	{1, "q",      shell_builtin_exit    },
	{2, "cd",     shell_builtin_cd      },
	{3, "pwd",    shell_builtin_pwd     },
	{5, "clean",  shell_builtin_clean   },
	{6, "printf", shell_builtin_printf  }
};

void shell_command_input(char *command)
{
	int count = 0;
	char shell_prompt[MAX_PATH] = {0};
	char path[MAX_PATH] = {0};

	getcwd(path, MAX_PATH);
	strncpy(shell_prompt, "[ rshell - ", 11);
	strncat(shell_prompt, path, strlen(path));
	strncat(shell_prompt, " ] $ ", 5);

	printf("%s", shell_prompt);
	while((command[count++] = getchar()) != '\n') {
		if(count > COMMAND_BUF_SIZE) break;
	}
	command[count - 1] = '\0';

}

void shell_command_parser(char *command_string, char **argv_buf)
{
	if(command_string[0] == '\0') return; // Don't process empty string

	int position = 0;
	char *token = NULL;

	memset(argv_buf, 0, COMMAND_BUF_SIZE);

	token = strtok(command_string, TOKEN_DELIM);
	while(token != NULL) {
		argv_buf[position] = token;
		token = strtok(NULL, TOKEN_DELIM);
		position++;
	}
}

int shell_command_builtin(char **command_argv) 
{
	for(int i = 0; i < BUILTIN_NUMBER; i++) {
		if(strncmp(command_argv[0], shell_builtin_list[i].option,
					shell_builtin_list[i].size) == 0) {
		shell_builtin_list[i].func(command_argv);
		return 1;
		}
	}
	return 0;
}

void shell_command_fork(char **command_argv)
{
	pid_t pid;
	int status;

	// U ugly
	pid = fork();
	if(pid == 0) {
    // Child process
	if(execvp(command_argv[0], command_argv) == -1) {
		printf("RShell: Not Such Command\n");
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

static void shell_builtin_pwd(const char **argv) {
	char pwd[MAX_PATH] = {0};
	getcwd(pwd, MAX_PATH);
	pwd[strlen(pwd)] = '\n';

	write(1, pwd, strlen(pwd));
}

static void shell_builtin_cd(const char **argv) {
	chdir(argv[1]);
}

static void shell_builtin_exit(const char **argv) {
	exit(0);
}

static void shell_builtin_clean(const char **argv) {
	for(int i = 0; i < SHELL_CLEAN_VALUE; ++i) write(1, "\n", 1);
}

static void shell_builtin_printf(const char **argv) {

	int count = 1;
	while(argv[count] != NULL) printf("%s ", argv[count++]);

}

static void shell_builtin_bg(const char **argv) {} // TODO
static void shell_builtin_fg(const char **argv) {} // TODO
static void shell_builtin_kill(const char **argv) {} // TODO
static void shell_builtin_user(const char **argv) {} //TODO
