#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <error.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/file.h>
#include <setjmp.h>
#include "shell.h"
#include "builtin.h"
#include "jobs.h"
#include "mem.h"

#define RSHELL_VERSION "v1.2"
void get_user_opts(int argc, char *argv[]);

int main(int argc, char *argv[])
{

	get_user_opts(argc, argv);

	int fd[2];

	char *command_string;
	char *prompt;

	prompt			= NULL;
	list_head		= NULL;
	command_string	= NULL;

	list_child.pid		= 0;
	running_child.pid	= 0;

	using_history();
	stifle_history(1000);

	signal(SIGINT,	SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, signal_sigtstp);
	signal(SIGCHLD, child_chk);

	while(1) {

		child_chk();
		setjmp(prompt_jmp);

		prompt = print_prompt();
PROMPT: command_string = readline(prompt);

		if(*command_string)		add_history(command_string);
		if(!(*command_string))	goto PROMPT;	// Don't do anything
												// with empty imput

		list_head = parse_input(command_string);

		if(exec_builtin(list_head) == 0) { // if "q" was typed

			clean_TOKEN_list(list_head);
			clean_child_list(&list_child);

			free(command_string);
			free(prompt);

			break;
		}

		if(!(list_head->flags & BUILTIN))
			exec_command(list_head, 0, fd);

		if(list_head->flags	& CHILD_SIG_STOP)
			child_add(&list_child, list_head);

		clean_TOKEN_list(list_head);
		free(command_string);
		free(prompt);
	}

	return 0;
}

void get_user_opts(int argc, char *argv[])
{
	int choice, i, fd[2];
	int option_index = 0;

	char *tmp_string = NULL;

	struct TOKEN *list_head;

	while (1)
	{
		static struct option long_options[] =
		{
			{"version", no_argument,		0,	'v'},
			{"help",	no_argument,		0,	'h'},
			{"command",	required_argument,	0,	'c'},
			{"dir",	required_argument,		0,	'd'},
			{0,0,0,0}
		};

		choice = getopt_long( argc, argv, "vhc:d:",
					long_options, &option_index);

		if (choice == -1)
			break;

		switch(choice)
		{
			case 'v':
				printf("RShell %s\n", RSHELL_VERSION);
				exit(0);

			case 'h':
				printf("Read rshell(1) man page\n");
				exit(0);

			case 'c':

				tmp_string = malloc(sizeof(char) * 1024);
				memset(tmp_string, 0, sizeof(char) * 1024);

				for(i = optind - 1; i < argc; ++i) {
					strcat(tmp_string, argv[i]);
					strcat(tmp_string, " ");
				}

				list_head = parse_input(tmp_string);
				exec_builtin(list_head);

				if(!(list_head->flags & BUILTIN))
					exec_command(list_head, 0, fd);

				clean_TOKEN_list(list_head);
				free(tmp_string);
				exit(0);

			case 'd':
				chdir(optarg);
				break;

		}
	}

}
