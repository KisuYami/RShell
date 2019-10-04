#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <wordexp.h>
#include <sys/wait.h>
#include <setjmp.h>
#include "shell.h"
#include "builtin.h"
#include "jobs.h"
#include "mem.h"

#define DEBUG 1

char *print_prompt(void)
{
	size_t size;

	char *user		= NULL;
	char *pwd		= NULL;
	char *prompt	= NULL;

	size = strlen(getenv("HOME"));
	pwd = malloc(sizeof(char) * MAX_PATH_SIZE);

	if(pwd == NULL) {
		printf("RShell: Failed to allocate memory\n");
		longjmp(prompt_jmp, 1);
	}

	getcwd(pwd, MAX_PATH_SIZE);
	if(strncmp(pwd, getenv("HOME"), size) == 0) {
		pwd += size - 1;
		pwd[0] = '~';
	}

	user   = getenv("USER");
	prompt = malloc(sizeof(char) * (14 + strlen(user) + MAX_PATH_SIZE));

#if DEBUG
	sprintf(prompt, "[ %s - %s ] $ ", user, pwd);
#else
	sprintf(prompt, "[ DEBUG - %s ] $ ", pwd);
#endif

	pwd -= size - 1;
	free(pwd);

	return prompt;
}

struct TOKEN *parse_input(char *command_string)
{

	unsigned int i = 0, position = 0;
	char string_literal;
	size_t size_count = 0;
	size_t tmp_size_token;

	char *token = NULL, *tmp_string = NULL;
	struct TOKEN *list_head, *list_ptr;

	wordexp_t parsed_expression;

	list_head = init_TOKEN_list();

	size_count = MAX_TOKEN_NUMBER;
	list_ptr   = list_head;

	token = strtok(command_string, INPUT_TOKEN_DELIMITER);

	while(token) {

		if(position >= size_count) { // Allocating more memory for command

			size_count += MAX_TOKEN_NUMBER;
			list_ptr->command = (char **)realloc_string(*list_ptr->command,
					size_count * sizeof(char));

		}

		switch(*token) {
			case '|':

				position = 0;

				list_ptr->flags |= PIPE;
				list_ptr->next	= init_TOKEN_list();
				list_ptr		= list_ptr->next;

				token = strtok(NULL, INPUT_TOKEN_DELIMITER);

				break;

			case '>':

				if(strncmp(token, ">>", 2) == 0)
					list_ptr->flags |= APPEND | REDIRECTION;
				else
					list_ptr->flags |= CREAT | REDIRECTION;

				position = 0;

				list_ptr->next	= init_TOKEN_list();
				list_ptr	    = list_ptr->next;

				token = strtok(NULL, INPUT_TOKEN_DELIMITER);

				break;

			case '<':

				position = 0;

				list_ptr->flags |= REDIRECTION;
				list_ptr->flags |= READ;

				list_ptr->next	= init_TOKEN_list();
				list_ptr		= list_ptr->next;

				token = strtok(NULL, INPUT_TOKEN_DELIMITER);
				break;

			case '\'':
			case '\"':
				string_literal = *token;

				tmp_size_token = sizeof(char) * 50;
				tmp_string = malloc(tmp_size_token);
				memset(tmp_string, 0, tmp_size_token);

				if(tmp_string == NULL) {
					printf("RShell: Failed to allocate memory\n");
					longjmp(prompt_jmp, 1);
				}

				while(1) {

					if(strlen(tmp_string) >= tmp_size_token) {
						tmp_size_token += 50;
						tmp_string = realloc_string(tmp_string,
								tmp_size_token * sizeof(char));
					}

					if(token == NULL) {
						printf("RShell: missing ending %c\n", string_literal);
						break;
					}

					else if(token[strlen(token) - 1] == string_literal) {

						if(*token == string_literal) token++;

						token[strlen(token) - 1] = '\0';
						strcat(tmp_string, token);

						break;
					}

					if(*token == string_literal) token++;

					strcat(tmp_string, token);
					strcat(tmp_string, " ");

					token = strtok(NULL, INPUT_TOKEN_DELIMITER);

				}

				list_ptr->command[position++] = tmp_string;
				token = strtok(NULL, INPUT_TOKEN_DELIMITER);
				break;

			default:

				wordexp(token, &parsed_expression, 0);

				for (i = 0; i < parsed_expression.we_wordc; ++i) {

					tmp_size_token = strlen(parsed_expression.we_wordv[i]) + 1;
					tmp_string = malloc(tmp_size_token);

					if(tmp_string == NULL) {
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

void exec_command(struct TOKEN *command, int i, int fd[2])
{
	int status = 0;
	pid_t pid = 0;

	if(!command) { // Cleaning stuff

		close(fd[1]);
		close(fd[0]);

		for(; i >= 0; --i)
			wait(NULL);

		return;
	}

	if(0 == i && command->next) {
		pipe(fd);
	}

	switch(pid = fork()) {
		case -1:
			printf("RShell: Failed to create child process\n");
			break;

		case 0: /* Child */

			// When a "normal" command is called
			if(0 == i && !command->next) {
				if(execvp(command->command[0], command->command) != 0)
					printf("RShell: Can't hear you!\n");
				exit(1);

			}

			// When commands uses redirect to/from file
			else if(command->flags & REDIRECTION) {
				if(command->flags & APPEND || command->flags & CREAT) {
					close(fd[1]);
					dup2(fd[0], STDIN_FILENO);
					close(fd[0]);
				}

				exec_command_redirection(command);
				exit(0);
			}

			// When commands uses PIPE and it's the last element
			else if(!command->next) {
				close(fd[1]);
				dup2(fd[0], STDIN_FILENO);
				close(fd[0]);
			}

			// When commands uses PIPE
			else {
				close(fd[0]);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
			}

			if(execvp(command->command[0], command->command) != 0)
				printf("RShell: Can't hear you!\n");

			exit(1);
		default: /* Father */

			// When a "normal" command is called
			if(0 == i && !command->next) {

				command->pid = pid;
				running_child.pid = pid;

				while(!WIFSIGNALED(status)) {

					waitpid(pid, &status, WUNTRACED);

					if(WIFSTOPPED(status)) {
						command->flags = CHILD_SIG_STOP;
						return;
					}
					if(WIFEXITED(status)) {
						return;
					}
				}

			}
			break;
	}
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
