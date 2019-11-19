#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <termios.h>
#include <wordexp.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <fcntl.h>

#include "parse.h"
#include "shell.h"
#include "builtin.h"
#include "jobs.h"
#include "mem.h"

void
set_history_file(void)
{
	if(getenv("RSHELL_HISTORY_FILE") == NULL)
	{
		strcpy(rshell_hist_file, getenv("HOME"));
		strncat(rshell_hist_file, "/.local/share/rshell_history", 29);
	}
	else
		strcpy(rshell_hist_file, getenv("RSHELL_HISTORY_FILE"));

    using_history();
    stifle_history(1000);
    read_history(rshell_hist_file);
}

char *
print_prompt(void)
{
    size_t size;

	char *home      = NULL;
    char *prompt	= NULL;
	char *ps1       = NULL;
    char  pwd[PATH_MAX+1];

    getcwd(pwd, PATH_MAX+1);

	ps1    = getenv("RSHELL_PROMPT");
    prompt = malloc(sizeof(char) * PATH_MAX * 2);
	memset(prompt, 0, sizeof(char) * PATH_MAX * 2);

	/*************************************************************************

    D is debug mode

    u adds the user name
    d adds the current path

    ************************************************************************/

	if(ps1 != NULL)
	{

		for(; *ps1 != '\0'; ps1++)
		{
			if(*ps1 == '%')
			{
				ps1++;

				if(*ps1 == '\0') strcat(prompt, "%");

				else if(*ps1 == ' ')
				{
				    strcat(prompt, "%");
					strcat(prompt, " ");
				}

				else if(*ps1 == 'D')
				{
					sprintf(prompt, "[ DEBUG ] $ ");
					break;
				}

				else if(*ps1 == 'u') strcat(prompt, getenv("USER"));

				else if(*ps1 == 'd') {

					home   = getenv("HOME");
					size   = strlen(home);

					if(strncmp(pwd, home, size) == 0) {

						strcat(prompt, "~");
						strcat(prompt, pwd+size);

					} else
						strcat(prompt, pwd);

				}
			} else {
				strncat(prompt, ps1, 1);
			}

		}

	} else {

		strcpy(prompt, " % ");

	}

    return prompt;
}

void
exec_command(node_t *command)
{
	pid_t pid;
	int status = 0, output_fd = 0;
	int t_fd[2], r_fd[2];

	for(node_t *cmd = command; cmd; cmd = cmd->next)
	{
		if(cmd->next)
            pipe(t_fd);

		switch(pid = fork())
		{
		case -1:
			printf("RShell: Failed to create child process\n");
			break;

		case 0: /* Child */

			if(cmd->flags & NODE_ASYNC)
			{
				output_fd = open("/dev/null", O_WRONLY);
				dup2(output_fd, STDOUT_FILENO);

				close(output_fd);
			}

			// Readirection for simple commands
			if(!(command->flags & NODE_PIPE) &&
			   (cmd->flags & NODE_REDIRECTION))
			{

				if(cmd->flags & NODE_APPEND || cmd->flags & NODE_CREAT)
				{
					close(t_fd[1]);
					dup2(t_fd[0], STDIN_FILENO);
					close(t_fd[0]);
				}

				exec_command_redirection(cmd);
				exit(0);
			}

			if(command->flags & NODE_PIPE)
			{
				if(cmd != command)
				{
					close(r_fd[1]);
					dup2(r_fd[0], STDIN_FILENO);
					close(r_fd[0]);
				}

				if(command->next)
				{
					close(t_fd[0]);
					dup2(t_fd[1], STDOUT_FILENO);
					close(t_fd[1]);
				}
			}

			// Readirection for pipes
			if(cmd->flags & NODE_REDIRECTION)
				exec_command_redirection(cmd);

			// Prepare command for simple execution
			if(!command->next)
			{
				setpgid(getpid(), 0);
				tcsetpgrp(STDIN_FILENO, getpgrp());
			}

			if(execvp(cmd->command[0], cmd->command) != 0)
				printf("RShell: Can't hear you!\n");

			exit(1);

		default: /* Father */
			if(cmd != command)
			{
				close(r_fd[0]);
				close(r_fd[1]);
			}

			if(command->next)
			{
				r_fd[0] = t_fd[0];
				r_fd[1] = t_fd[1];
			}

			// When a "normal" command is called
			if(!cmd->next && !(command->flags & NODE_ASYNC))
			{
				running_child.pid = pid;
				running_child.state = JOB_RUNNING;

				while(!WIFSIGNALED(status))
				{
					waitpid(pid, &status, WUNTRACED);

					if(WIFSTOPPED(status))
					{
						running_child.state = JOB_STOPPED;

						command->flags = JOB_STOPPED;
						command->pid = pid;

						child_add(&list_child, command);
						break;
					}

					if(WIFEXITED(status))
					{
						running_child.state = JOB_EXITED;
						break;
					}
				}

				running_child.state = 0;
				running_child.pid = 0;
				return;
			}
			break;
		}
	}

	close(t_fd[0]);
	close(t_fd[1]);
	close(r_fd[0]);
	close(r_fd[1]);
}

void
exec_command_redirection(node_t *head)
{
    int file_fd = 0;
    node_t *file_name;
    mode_t mode;

    file_name = head->next;
    mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    if(head->flags & NODE_APPEND)
	{
		file_fd = open(file_name->command[0],
					   O_WRONLY | O_CREAT | O_APPEND, mode);
        dup2(file_fd, STDOUT_FILENO);
	}

    else if(head->flags & NODE_CREAT)
	{
		file_fd = open(file_name->command[0],
					   O_WRONLY | O_CREAT | O_TRUNC,  mode);
        dup2(file_fd, STDOUT_FILENO);
	}

    else if(head->flags & NODE_READ)
	{
		file_fd = open(file_name->command[0], O_RDONLY);
        dup2(file_fd, STDIN_FILENO);
	}

	if(execvp(head->command[0], head->command) != 0)
		printf("RShell: Can't hear you!\n");
}
