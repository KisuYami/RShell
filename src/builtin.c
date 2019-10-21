#include <readline/history.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

#include "builtin.h"
#include "shell.h"
#include "jobs.h"
#include "mem.h"

struct
{
    size_t size;
    char *command;

    void (*func)(struct TOKEN *head);

} builtin_list[] = {
    {1, "q",     builtin_exit		},
    {2, "cd",    builtin_cd			},
    {2, "bg",    builtin_bg			},
    {2, "fg",    builtin_fg			},
    {3, "pwd",   builtin_pwd		},
    {3, "set",   builtin_set_env	},
    {4, "show",  builtin_show_env	},
    {4, "calc",  builtin_calc		},
    {4, "rand",  builtin_rand		},
    {5, "clear", builtin_clean		},
    {0,  NULL,	 NULL}
};

int is_file(char *path)
{
    struct stat path_to_file;
    stat(path, &path_to_file);
    return S_ISREG(path_to_file.st_mode);
}

int exec_builtin(struct TOKEN *head)
{
    for(int i = 0; builtin_list[i].size != 0; ++i)
    {

        if(strcmp(head->command[0], builtin_list[i].command) == 0)
        {
            builtin_list[i].func(head);
            head->flags = BUILTIN;
            return i;
        }
    }

    return -1;
}

void builtin_exit(struct TOKEN *head)
{
    struct child *child_ptr = NULL;

	/* Check for running childs */
    for(child_ptr = &list_child; child_ptr != NULL;
            child_ptr = child_ptr->next)
    {

        if(child_ptr->pid > 0)
        {
            printf("RShell: child process still running...\n");
            builtin_bg(head);
            longjmp(prompt_jmp, 0);
        }

    }
    printf(".·´¯`(>▂<)´¯`·.\n");
}

void builtin_fg(struct TOKEN *head)
{

    pid_t pid;
    struct child *child_ptr;

    child_ptr = &list_child;

    if(head->size <= 1)
    {
        while(child_ptr != NULL)
        {
            if(child_ptr->pid > 0)
            {

                kill(child_ptr->pid, SIGCONT);
                return;
            }
            child_ptr = child_ptr->next;
        }
    }
    else
    {
        pid = atoi(head->command[1]);
        while(child_ptr != NULL)
        {
            if(child_ptr->pid == pid)
            {

                kill(pid, SIGCONT);
                return;
            }
            child_ptr = child_ptr->next;
        }
    }
}

void builtin_bg(struct TOKEN *head)
{

    struct child *child_ptr = NULL;
    child_ptr = &list_child;

    while(child_ptr != NULL)
    {
        if(child_ptr->pid > 0)
            printf("[ %s - %d ]\n", child_ptr->name, child_ptr->pid);

        child_ptr = child_ptr->next;
    }
}

void builtin_cd(struct TOKEN *head)
{
    if(is_file(head->command[1]) != 1)
    {
        if(chdir(head->command[1]) != 0)
            printf("RShell: Directory does not exist\n");
    } else
		printf("RShell: %s isn't a folder or directory\n", head->command[1]);
		
}

void builtin_pwd(struct TOKEN *head)
{
    char buf[PATH_MAX];
    getcwd(buf, PATH_MAX);

    printf("%s\n", buf);
}

void builtin_calc(struct TOKEN *head)
{
    if(head->size < 2)
    {
        printf("RShell: Missing arguments\n");
        return;
    }

	char buf[256];

	sprintf(buf, "print(%s)", head->command[1]);

	if(fork() == 0)
	{
		execlp("lua", "lua", "-e", buf, (char *)NULL);
	}

	wait(NULL);
}

void builtin_rand(struct TOKEN *head)
{

    if(head->size < 3)
    {
        printf("RShell: Missing arguments\n");
        return;
    }

    int i, p;
	int x = 0;

    srand(time(NULL));

    i = atoi(head->command[1]);
    p = atoi(head->command[2]);

	if (i > p) {
		printf("RShell: Wrong Range: %d is greater than %d\n", i, p);
		return;
	}

    while(1)
    {
        x = rand();
        if(x >= i)
        {
            if(x <= p)
                break;
        }
    }

    printf("%d <--> %d: %d\n", i, p, x);
}

void builtin_clean(struct TOKEN *head)
{
	printf("\033[1J");
	printf("\033[1H");
}

void builtin_set_env(struct TOKEN *head)
{

    if(head->size < 3)
    {
        printf("RShell: Missing arguments\n");
        return;
    }

    char c, *env;

    c = getopt(head->size, head->command, "a:");

    switch(c)
    {
    case 'a':

        if(head->size != 4)
        {
            printf("usage: set -a <ENV> <VALUE>\n");
            return;
        }

        env = getenv(head->command[2]);
        strncat(env, ":", 2);
        strcat(env, head->command[3]);

        setenv(head->command[1], env, 1);
        break;

    default:
        setenv(head->command[1], head->command[2], 1);
    }

}

void builtin_show_env(struct TOKEN *head)
{
    if(head->size < 2)
    {
        printf("RShell: Missing arguments\n");
        return;
    }

    char *env = getenv(head->command[1]);

    if(env == NULL)
        printf("RShell: Enviroment \"%s\" isn't defined.\n", head->command[1]);
    else
        printf("%s=%s\n", head->command[1], env);

}
