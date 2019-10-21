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

	return;
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

	if(ps1 != NULL) {

		for(; *ps1 != '\0'; ps1++) {

			if(*ps1 == '%') {
				
				ps1++;

				if(*ps1 == '\0') strcat(prompt, "%");

				else if(*ps1 == ' ') {

				    strcat(prompt, "%");
					strcat(prompt, " ");
					
				}

				else if(*ps1 == 'D') {

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
exec_command(struct TOKEN *command, int i, int fd[2])
{
    int status = 0, output_fd = 0;
    pid_t pid = 0;

    if(!command)   // Cleaning stuff
    {
        close(fd[0]);
        close(fd[1]);

        for(; i >= 0; --i)
            wait(NULL);

        return;
    }

    if(0 == i && command->next)
        pipe(fd);

    switch(pid = fork())
    {
    case -1:
        printf("RShell: Failed to create child process\n");
        break;

    case 0: /* Child */

        // When a "normal" command is called
        if(0 == i && !command->next)
        {

            if(command->flags & CHILD_BACKGROUND)
            {
                output_fd = open("/dev/null", O_WRONLY);
                dup2(output_fd, STDOUT_FILENO);

                close(output_fd);
            }

            setpgid(getpid(), 0);
            tcsetpgrp(STDIN_FILENO, getpgrp());

            if(execvp(command->command[0], command->command) != 0)
                printf("RShell: Can't hear you!\n");

            exit(1);

        }

        // When command uses redirect to/from file
        else if(command->flags & REDIRECTION)
        {

            if(command->flags & APPEND || command->flags & CREAT)
            {

                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);

            }

            exec_command_redirection(command);
            exit(0);
        }

        // First command when using PIPEs
        else if(0 == i)
        {

            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);

        }

        // Rest of the command
        else
        {

            close(fd[1]);

            dup2(fd[0], STDIN_FILENO);

            if(command->next != NULL)
                dup2(fd[1], STDOUT_FILENO);

            close(fd[1]);
            close(fd[0]);

        }

        if(execvp(command->command[0], command->command) != 0)
            printf("RShell: Can't hear you!\n");

        exit(1);

    default: /* Father */

        // When a "normal" command is called
        if(0 == i && !command->next)
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

    if(!(command->flags & CHILD_BACKGROUND))
        i++;

    exec_command(command->next, i, fd);

}

void exec_command_redirection(struct TOKEN *head)
{
    int file_fd = 0;
    struct TOKEN *file_name;
    mode_t mode;

    file_name = head->next;
    mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    if(head->flags & APPEND) file_fd = open(file_name->command[0], O_WRONLY | O_CREAT | O_APPEND, mode);
    if(head->flags & CREAT ) file_fd = open(file_name->command[0], O_WRONLY | O_CREAT | O_TRUNC,  mode);
    if(head->flags & READ  ) file_fd = open(file_name->command[0], O_RDONLY);

    if((head->flags & APPEND) || (head->flags & CREAT)) // Outputting
        dup2(file_fd, STDOUT_FILENO);					// to file

    else if(head->flags & READ)							// Reading
        dup2(file_fd, STDIN_FILENO);					// from file

    if(execvp(head->command[0], head->command) != 0)
        printf("RShell: Can't hear you!\n");

    close(file_fd);
}
