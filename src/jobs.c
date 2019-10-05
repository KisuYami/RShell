#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <setjmp.h>
#include "shell.h"
#include "jobs.h"

void child_add(struct child *list_head, struct TOKEN *head)
{

	struct child *child_ptr = NULL;
	size_t command_size = strlen(head->command[0]);

	child_ptr = list_head;

	while(1) {
		if(child_ptr->pid == 0) {

			if(child_ptr->name != NULL)
				free(child_ptr->name);

			child_ptr->name = malloc(sizeof(char) * command_size + 1);

			if(child_ptr->name == NULL) {
				printf("RShell: Failed to allocate memory\n");
				longjmp(prompt_jmp, 1);
			}

			memset(child_ptr->name, 0, command_size);
			strncpy(child_ptr->name, head->command[0], command_size + 1);

			child_ptr->pid = head->pid;
			child_ptr->next = NULL;

			printf("[ Stopped: %s - %d ]\n", child_ptr->name, child_ptr->pid);

			return;

		} else if(child_ptr->next == NULL) {

			child_ptr->next = malloc(sizeof(struct child));
			child_ptr = child_ptr->next;

			child_ptr->name = malloc(sizeof(char) * command_size + 1);

			if(child_ptr->name == NULL) {
				printf("RShell: Failed to allocate memory\n");
				longjmp(prompt_jmp, 1);
			}

			memset(child_ptr->name, 0, command_size);
			strncpy(child_ptr->name, head->command[0], command_size + 1);

			child_ptr->pid = head->pid;
			child_ptr->next = NULL;

			printf("[ Stopped: %s - %d ]\n", child_ptr->name, child_ptr->pid);

			return;

		}
		child_ptr = child_ptr->next;
	}
}

void child_chk()
{

	int status = 0;
	struct child *child_ptr = NULL;

	for(child_ptr = &list_child; child_ptr != NULL;
			child_ptr = child_ptr->next) {

		if(child_ptr->pid > 0) {

			waitpid(child_ptr->pid, &status, WCONTINUED | WNOHANG);
			if(WIFCONTINUED(status)) {

				printf("[ Continued: %s - %d ]\n", child_ptr->name,
						child_ptr->pid);

				tcsetpgrp(STDIN_FILENO, child_ptr->pid);

				running_child = *child_ptr;
				waitpid(child_ptr->pid, &status, WUNTRACED);

				while(!WIFSIGNALED(status)) {

					if(WIFSTOPPED(status)) {
						printf("[ Stopped: %s - %d ]\n", child_ptr->name,
								child_ptr->pid);
						break;
					}

					if(WIFEXITED(status)) {
						printf("[ Exited: %s - %d ]\n", child_ptr->name,
								child_ptr->pid);
						child_ptr->pid = 0;
						break;
					}
				}

				running_child = (struct child) {
					.pid = 0,
					.name = NULL,
					.next = NULL
				};

				tcsetpgrp(STDIN_FILENO, getpgrp());
				break;

			}

		}
	}
}

void signal_sigint(int sig)
{
	char *prompt = NULL;

	if(running_child.pid > 0)
		kill(running_child.pid, 9);

	prompt = print_prompt();
	printf("\n%s", prompt);

	free(prompt);
}

void signal_sigtstp(int sig)
{
	if(running_child.pid > 0)
		kill(running_child.pid, SIGSTOP);
}
