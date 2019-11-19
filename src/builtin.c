#include <readline/history.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "builtin.h"
#include "parse.h"
#include "jobs.h"
#include "mem.h"
#include "shell.h"

struct
{
    size_t size;
    char *command;

    int (*func)(node_t *head);

} builtin_list[] = {
    {1, "q",     builtin_exit		},
    {2, "cd",    builtin_cd			},
    {2, "bg",    builtin_bg			},
    {2, "fg",    builtin_fg			},
    {3, "pwd",   builtin_pwd		},
    {3, "set",   builtin_set_env	},
    {4, "show",  builtin_show_env	},
    {4, "calc",  builtin_calc		},
    {5, "clear", builtin_clean		},
    {0,  NULL,	 NULL}
};

int is_file(char *path)
{
    struct stat path_to_file;
    stat(path, &path_to_file);
    return S_ISREG(path_to_file.st_mode);
}

int exec_builtin(node_t *head)
{
    for(int i = 0; builtin_list[i].size != 0; ++i)
    {
        if(strcmp(head->command[0], builtin_list[i].command) == 0)
        {
            builtin_list[i].func(head);
            return 1;
        }
    }

    return 0;
}

int builtin_exit(node_t *head)
{
    job_t *child_ptr = NULL;

	/* Check for running/stopped/waiting childs */
    for(child_ptr = &list_child; child_ptr != NULL; child_ptr = child_ptr->next)
    {
        if(child_ptr->pid > 0)
        {
            printf("RShell: child process still running...\n");
            builtin_bg(head);
			break;
        }

		else
		{
			// Memory is freed with atexit(clean_everything);
			write_history(rshell_hist_file);
			printf(".·´¯`(>▂<)´¯`·.\n");
			exit(0);
		}

    }
	return 1;
}

int
builtin_fg(node_t *head)
{
    if(head->size < 2)
    {
		for(job_t *child_ptr = &list_child; child_ptr != NULL;
			child_ptr = child_ptr->next)
        {
            if(child_ptr->pid > 0)
                return kill(child_ptr->pid, SIGCONT);
        }
    }
    else
    {
        pid_t pid = atoi(head->command[1]);

		for(job_t *child_ptr = &list_child; child_ptr != NULL;
			child_ptr = child_ptr->next)
        {
            if(child_ptr->pid == pid)
                return kill(pid, SIGCONT);
        }
    }
	return 1;
}

int
builtin_bg(node_t *head)
{
	for(job_t *child_ptr = &list_child; child_ptr != NULL;
			child_ptr = child_ptr->next)
    {
        if(child_ptr->pid > 0)
            printf("[ %s - %d ]\n", child_ptr->name, child_ptr->pid);
    }
	return 1;
}

int builtin_cd(node_t *head)
{
	if (is_file(head->command[1]) != 1 && chdir(head->command[1]) != 0)
		printf("RShell: Directory does not exist\n");

	else if(is_file(head->command[1]) == 1)
		printf("RShell: %s isn't a folder or directory\n", head->command[1]);

	return 1;
}

int builtin_pwd(node_t *head)
{
    char buf[PATH_MAX];
    getcwd(buf, PATH_MAX);

    printf("%s\n", buf);
	return 1;
}

int builtin_calc(node_t *head)
{
    if(head->size < 2)
    {
        printf("RShell: Missing arguments\n");
        return -1;
    }

	char buf[256];

	sprintf(buf, "print(%s)", head->command[1]);

	if(fork() == 0)
	{
		execlp("lua", "lua", "-e", buf, (char *)NULL);
		exit(1);
	}

	wait(NULL);
	return 1;
}

int builtin_clean(node_t *head)
{
	printf("\033[1J");
	printf("\033[1H");
	return 1;
}

int builtin_set_env(node_t *head)
{
    if(head->size < 3)
    {
        printf("RShell: Missing arguments\n");
        return -1;
    }

	setenv(head->command[1], head->command[2], 1);
	return 1;
}

int builtin_show_env(node_t *head)
{
    if(head->size < 2)
    {
        printf("RShell: Missing arguments\n");
        return -1;
    }

    char *env = getenv(head->command[1]);

    if(env == NULL)
        printf("RShell: Enviroment \"%s\" isn't defined.\n", head->command[1]);

    else
		printf("%s: %s\n", head->command[1], env);

	return 1;
}
