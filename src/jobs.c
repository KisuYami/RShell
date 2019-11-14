#include <sys/wait.h>
#include <termios.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdio.h>
#include <errno.h>

#include "shell.h"
#include "jobs.h"

void
child_add(job_t *list_head, node_t *head)
{

    job_t *child_ptr = NULL;
    size_t command_size = strlen(head->command[0]);

    child_ptr = list_head;

    while(1)
    {
        if(child_ptr->pid == 0)
        {

            if(child_ptr->name != NULL)
                free(child_ptr->name);

            *child_ptr = (job_t) {
                .name = malloc(sizeof(char) * command_size + 1),
                .pid = head->pid,
                .state = head->flags,
                .next = NULL,
            };

            if(child_ptr->name == NULL)
            {
                printf("RShell: Failed to allocate memory\n");
                free(child_ptr->next);
				return;
            }

            strncpy(child_ptr->name, head->command[0], command_size + 1);

            if(child_ptr->state & JOB_STOPPED)
                printf("[ Stopped: %s - %d ]\n", child_ptr->name, child_ptr->pid);

            return;

        }

        else if(child_ptr->next == NULL)
        {

            child_ptr->next = malloc(sizeof(job_t));
            child_ptr = child_ptr->next;

            *child_ptr = (job_t) {
                .name = malloc(sizeof(char) * command_size + 1),
                .pid = head->pid,
                .state = head->flags,
                .next = NULL,
            };

            if(child_ptr->name == NULL)
            {
                printf("RShell: Failed to allocate memory\n");
                free(child_ptr->next);
				return;
            }

            strncpy(child_ptr->name, head->command[0], command_size + 1);

            if(child_ptr->state & JOB_STOPPED)
                printf("[ Stopped: %s - %d ]\n", child_ptr->name, child_ptr->pid);

            return;

        }
        child_ptr = child_ptr->next;
    }
}

void
child_chk()
{

    int status = 0;
	int ret = -1;

    job_t *child_ptr = NULL;

    for(child_ptr = &list_child;
		child_ptr != NULL;
        child_ptr = child_ptr->next)
    {

        if(child_ptr->pid > 0)
        {

            ret = waitpid(child_ptr->pid, &status, WCONTINUED | WNOHANG);

            if(WIFCONTINUED(status))
            {

                printf("[ Continued: %s - %d ]\n", child_ptr->name,
                       child_ptr->pid);

                /* Setting running_child for signal_handler */
                running_child = *child_ptr;
                running_child.state = JOB_STOPPED;

                tcsetpgrp(STDIN_FILENO, child_ptr->pid);
                waitpid(child_ptr->pid, &status, WUNTRACED);

                while(!WIFSIGNALED(status))
                {

                    if(WIFSTOPPED(status))
                    {
                        printf("[ Stopped: %s - %d ]\n", child_ptr->name,
                               child_ptr->pid);
                        break;
                    }

                    if(WIFEXITED(status))
                    {
                        printf("[ Exited: %s - %d ]\n", child_ptr->name,
                               child_ptr->pid);
                        child_ptr->pid = 0;
                        break;
                    }
                }

                running_child = (job_t)
                    {
                        .pid = 0,
                        .state = 0,
                        .name = NULL,
                        .next = NULL,
                    };

                tcsetpgrp(STDIN_FILENO, getpgrp());
                break;

            } /* if: WIFCONTINUED(STATUS) */

            else if(WIFEXITED(status) && ret != 0)
            {
                printf("[ Exited: %s - %d ]\n", child_ptr->name,
                       child_ptr->pid);
                child_ptr->pid = 0;
                break;
            }

        } /* if: child->pid > 0 */

    } /* for loop */

}

void
signal_handler(int sig)
{

    if(running_child.state & JOB_RUNNING)
    {
        switch (sig) {

        case SIGINT:

            kill(running_child.pid, SIGINT);

            /* if the running_child was set in child_chk() */
            if(running_child.state & JOB_STOPPED)
            {
                for(job_t *cptr = &list_child; cptr != NULL; cptr = cptr->next)
                {
                    if(running_child.pid == cptr->pid)
                    {
                        cptr->pid = 0;
                        break;
                    }
                }
            }

            running_child.pid = 0;

            break;

        case SIGTSTP:

            kill(running_child.pid, SIGSTOP);

            break;

        case SIGCHLD:

            child_chk();
            break;
        }
    }
}
