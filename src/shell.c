#include <sys/wait.h>
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

#define DEBUG 1

char *
print_prompt(void)
{
    size_t size;

    char *user		= NULL;
	char *home      = NULL;
    char *prompt	= NULL;
    char  pwd[PATH_MAX+1];

    getcwd(pwd, PATH_MAX+1);

    user   = getenv("USER");
	home   = getenv("HOME");
    size   = strlen(home);
    prompt = malloc(sizeof(char) * PATH_MAX * 2);

#if DEBUG
    if(strncmp(pwd, home, size) == 0)
		sprintf(prompt, "[ %s - ~%s ] $ ", user, pwd+size);

	else
		sprintf(prompt, "[ %s - %s ] $ ", user, pwd);

#else
    sprintf(prompt, "[ DEBUG - %s ] $ ", pwd);
#endif

    return prompt;
}

struct TOKEN *
parse_input(char *command_string)
{

    char string_literal;

    char *token      = NULL;
	char *tmp_string = NULL;

    size_t count          = 0;
    size_t position       = 0;
    size_t tmp_size_token = 0;

    wordexp_t parsed_expression;

    struct TOKEN *list_head = NULL;
	struct TOKEN *list_ptr  = NULL;

    list_head	= init_TOKEN_list();
    list_ptr	= list_head;

    token = strtok(command_string, INPUT_TOKEN_DELIMITER);

    while(token)
    {

        switch(*token)
        {
        case '|':

            list_ptr->flags |= PIPE;

            list_ptr->next	= init_TOKEN_list();
            list_ptr		= list_ptr->next;

            position = 0;

            token = strtok(NULL, INPUT_TOKEN_DELIMITER);

            break;

        case '>':

            if(strncmp(token, ">>", 2) == 0)
                list_ptr->flags |= APPEND | REDIRECTION;
            else
                list_ptr->flags |= CREAT | REDIRECTION;

            list_ptr->next	= init_TOKEN_list();
            list_ptr	    = list_ptr->next;

            position = 0;

            token = strtok(NULL, INPUT_TOKEN_DELIMITER);

            break;

        case '<':

            list_ptr->flags |= REDIRECTION;
            list_ptr->flags |= READ;

            list_ptr->next	= init_TOKEN_list();
            list_ptr		= list_ptr->next;

            position = 0;

            token = strtok(NULL, INPUT_TOKEN_DELIMITER);
            break;

        case '&':

            list_ptr->flags |= CHILD_BACKGROUND;
            token = strtok(NULL, INPUT_TOKEN_DELIMITER);
            break;

        case '\'':
        case '\"':
            string_literal = *token;

            tmp_size_token = strlen(token);
            tmp_size_token = tmp_size_token * tmp_size_token + 10; // Elevate by 2

            tmp_string = malloc(sizeof(char) * tmp_size_token);
            memset(tmp_string, 0, tmp_size_token);

            if(tmp_string == NULL)
            {
                printf("RShell: Failed to allocate memory\n");
                longjmp(prompt_jmp, 1);
            }

			// Remove the " or ' in the start of the first token
			token++;

            while(1)
            {

                strcat(tmp_string, token);
                token = strtok(NULL, INPUT_TOKEN_DELIMITER);

                if(token == NULL)
                {
					char *dot = strrchr(tmp_string, string_literal);

					if(dot != NULL)
						dot[0] = '\0'; // Remove trailing string_literal
					else
						printf("RShell: missing ending %c\n", string_literal);

					break;
                }

				strcat(tmp_string, " ");

				// Reallocation of memory
				count += strlen(token);

                if(count >= tmp_size_token - 10)
                {
                    tmp_size_token = count * count;
                    tmp_string = realloc_string(tmp_string,
                                                tmp_size_token * sizeof(char));
                }

            }

            list_ptr->command[position++] = tmp_string;
            token = strtok(NULL, INPUT_TOKEN_DELIMITER);
            break;

        default:

			if(1); // Work around for the declaration below.
			char *dot = NULL;

			dot = strrchr(token, '\'');
			if(dot != NULL) dot[0] = '\0'; // Remove trailing '

			dot = strrchr(token, '\"');
			if(dot != NULL) dot[0] = '\0'; // Remove trailing "

            wordexp(token, &parsed_expression, 0);

            for (size_t i = 0; i < parsed_expression.we_wordc; ++i)
            {

                tmp_string = malloc(sizeof(char) * 512);


                if(tmp_string == NULL)
                {
                    printf("RShell: Failed to allocate memory\n");
                    longjmp(prompt_jmp, 1);
                }

                strcpy(tmp_string, parsed_expression.we_wordv[i]);
                list_ptr->command[position++] = tmp_string;
            }

            wordfree(&parsed_expression);
            token = strtok(NULL, INPUT_TOKEN_DELIMITER);
            break;
        }

    }

    list_ptr->size = position;

    return list_head;
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

#if 0
    if(i > 1)
    {
        wait(NULL);
        i--;
    }
#endif

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
