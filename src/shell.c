#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <wordexp.h>

#include "builtin.h"
#include "jobs.h"
#include "mem.h"
#include "parse.h"
#include "shell.h"

void set_history_file(void)
{
	if (getenv("RSHELL_HISTORY_FILE") == NULL)
		sprintf(rshell_hist_file, "%s/.local/share/rshell_history",
			getenv("HOME"));
	else
		strcpy(rshell_hist_file, getenv("RSHELL_HISTORY_FILE"));

	using_history();
	stifle_history(1000);
	read_history(rshell_hist_file);
}

char *print_prompt(void)
{
	char *ps1 = getenv("RSHELL_PROMPT");
	char *prompt = malloc(sizeof(char) * PATH_MAX * 2);
	char pwd[PATH_MAX + 1];

	prompt[0] = '\0';
	getcwd(pwd, PATH_MAX + 1);

	if (ps1 == NULL) {
		strcpy(prompt, " % ");
		return prompt;
	}

	for (; *ps1 != '\0'; ps1++) {
		if (*ps1 == '%') {
			switch (ps1[1]) {
			case 'h': {
				char tmp[1024];
				gethostname(tmp, 1024);
				strcat(prompt, tmp);
				break;
			}
			case 'u':
				strcat(prompt, getenv("USER"));
				break;

			case 'd': {
				char *home = getenv("HOME");
				size_t size = strlen(home);

				if (strncmp(pwd, home, size) == 0) {
					strcat(prompt, "~");
					strcat(prompt, pwd + size);
				} else
					strcat(prompt, pwd);

				break;
			}

			case 'D': {
				char *home = getenv("HOME");
				size_t size = strlen(home);

				if (strncmp(pwd, home, size) == 0 &&
				    strlen(pwd) == size)
					strcat(prompt, "~");
				else
					strcat(prompt, strrchr(pwd, '/') + 1);
				break;
			}
			default:
				strncat(prompt, ps1, 1);
				break;
			}
			ps1++;
		} else
			strncat(prompt, ps1, 1);
	}

	return prompt;
}

void exec_command(node_t *command)
{
	pid_t pid = 0;
	int t_fd[2], r_fd[2];

	for (node_t *cmd = command; cmd; cmd = cmd->next) {
		if (cmd->next)
			pipe(t_fd);

		switch (pid = fork()) {
		case -1:
			printf("RShell: Failed to create child process\n");
			break;

		case 0: /* Child */

			// Reset so the shell can point the problem
			errno = 0;

			// Readirection for simple commands
			if (command->flags & NODE_REDIRECTION_PIPE) {
				if (cmd != command) {
					close(r_fd[1]);
					dup2(r_fd[0], STDIN_FILENO);
					close(r_fd[0]);
				}

				if (command->next) {
					close(t_fd[0]);
					dup2(t_fd[1], STDOUT_FILENO);
					close(t_fd[1]);
				}
			}

			// Readirection for pipes
			if (cmd->flags & NODE_REDIRECTION ||
			    cmd->flags & NODE_EXEC_ASYNC)
				exec_command_redirection(cmd);

			// Prepare command for simple execution
			if (!command->next) {
				setpgid(getpid(), 0);
				tcsetpgrp(STDIN_FILENO, getpgrp());
			}

			execvp(cmd->command[0], cmd->command);

			if (errno != 0) {
				if (errno == ENOENT)
					printf("RShell: Program don't exist\n");
				else if (errno == EACCES)
					printf(
					    "RShell: Can't execute command\n");
				else
					printf("RShell: Can't hear you!\n");
			}

			exit(1);

		default: /* Father */

			if (cmd != command) {
				close(r_fd[0]);
				close(r_fd[1]);
			}

			if (command->next) {
				r_fd[0] = t_fd[0];
				r_fd[1] = t_fd[1];
			}

			// When a "normal" command is called
			if (!cmd->next && !(command->flags & NODE_EXEC_ASYNC)) {
				child_running(pid, command, NULL);
				return;
			}

			if (cmd->flags & NODE_REDIRECTION)
				return;
		}
	}

	// The only reason the execution gets here is because of a pipeline
	close(t_fd[0]);
	close(t_fd[1]);
	close(r_fd[0]);
	close(r_fd[1]);
}

void exec_command_redirection(node_t *head)
{
	int file_fd = 0;
	char *file_name = head->next->command[0];

	if (head->flags & NODE_REDIRECTION_READ) {
		file_fd = open(file_name, O_RDONLY);
		dup2(file_fd, STDIN_FILENO);
	} else {
		if (head->flags & NODE_REDIRECTION_APPEND)
			file_fd =
			    open(file_name, MODE_APPEND, MODE_PERMISSIONS);

		else if (head->flags & NODE_REDIRECTION_CREAT)
			file_fd =
			    open(file_name, MODE_CREATE, MODE_PERMISSIONS);

		else if (head->flags & NODE_EXEC_ASYNC ||
			 head->flags & NODE_REDIRECTION_DUP)
			file_fd = open("/dev/null", O_WRONLY);

		dup2(file_fd, STDOUT_FILENO);
	}

	close(file_fd);

	errno = 0;

	execvp(head->command[0], head->command);
	exit(1);
}
