#include <readline/readline.h>
#include <readline/history.h>
#include <termios.h>
#include <setjmp.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include "builtin.h"
#include "parse.h"
#include "shell.h"
#include "jobs.h"
#include "mem.h"

#define RSHELL_VERSION "v2.0-3"

void
get_user_opts(int argc, char *argv[]);

int
main(int argc, char *argv[])
{
    get_user_opts(argc, argv);

    set_history_file();
    atexit(clean_everything);

    memset(&running_child, 0, sizeof(job_t));
    memset(&list_child, 0, sizeof(job_t));

    signal(SIGINT,  signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, signal_handler);
    signal(SIGTTOU, SIG_IGN);

    setpgid(getpid(), tcgetpgrp(STDIN_FILENO));

    while(1)
    {
        child_chk();

		int   result = -1;
        char *prompt = print_prompt();
        char *command_str = readline(prompt);

        if(*command_str)     add_history(command_str);
        if(!(*command_str))  goto ERROR; // Don't do anything with empty imput

        list_head = parse_input(command_str);

        if(!list_head)
            goto ERROR;

        result = exec_builtin(list_head);

        if(!(list_head->flags & NODE_BUILTIN))
            exec_command(list_head, 0, (int[2]){0, 0});

        tcsetpgrp(STDIN_FILENO, getpgrp());
        setpgid(getpid(), tcgetpgrp(STDIN_FILENO));

        clean_node_list(list_head);
ERROR:  free(command_str);
        free(prompt);

        if(0 == result) // User input was "q"
            return 0;
    }

    return 0;
}

void get_user_opts(int argc, char *argv[])
{
    int option_index = 0;

    while (1)
    {

        static struct option long_options[] =
            {
                {"version", no_argument,        0,  'v'},
                {"help",    no_argument,        0,  'h'},
                {"command", required_argument,  0,  'c'},
                {"dir", required_argument,      0,  'd'},
                {0,0,0,0},
            };

        int choice = getopt_long(argc, argv, "vhc:d:",
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
        {
            char *tmp_string = calloc(1024, sizeof(char));

            for(int i = optind - 1; i < argc; ++i)
            {
                strcat(tmp_string, argv[i]);
                strcat(tmp_string, " ");
            }

            node_t *list_head = parse_input(tmp_string);
            exec_builtin(list_head);

            if(!(list_head->flags & NODE_BUILTIN))
                exec_command(list_head, 0, (int[2]){0, 0});

            clean_node_list(list_head);
            free(tmp_string);
            exit(0);
        }

        case 'd':
            chdir(optarg);
            break;

        }
    }

}
