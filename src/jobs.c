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
	for (job_t *child_ptr = list_head; child_ptr != NULL;
		 child_ptr = child_ptr->next)
    {
        if(child_ptr->pid == 0)
        {
            *child_ptr = (job_t) {
                .pid = head->pid,
                .state = head->flags,
                .next = NULL,
            };

            strcpy(child_ptr->name, head->command[0]);

            if(child_ptr->state & JOB_STOPPED)
                printf("[ Stopped: %s - %d ]\n", child_ptr->name, child_ptr->pid);

            break;
        }

        else if(child_ptr->next == NULL)
        {

            child_ptr->next = malloc(sizeof(job_t));
            child_ptr = child_ptr->next;

            *child_ptr = (job_t) {
                .pid = head->pid,
                .state = head->flags,
                .next = NULL,
            };

            strcpy(child_ptr->name, head->command[0]);

            if(child_ptr->state & JOB_STOPPED)
                printf("[ Stopped: %s - %d ]\n", child_ptr->name, child_ptr->pid);

            return;
        }
    }
}

void
child_chk()
{
    for(job_t *child_ptr = &list_child; child_ptr != NULL;
        child_ptr = child_ptr->next)
    {
        if(child_ptr->pid > 0)
        {
			int status = 0;
            int ret = waitpid(child_ptr->pid, &status, WCONTINUED | WNOHANG);

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

				memset(&running_child, 0, sizeof(job_t));
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
        switch(sig)
		{

        case SIGINT:

            /* if the running_child was set in child_chk() */
            if(running_child.state & JOB_STOPPED)
            {
                for(job_t *cptr = &list_child; cptr != NULL; cptr = cptr->next)
                {
                    if(running_child.pid == cptr->pid)
                    {
						kill(cptr->pid, SIGCONT);
                        cptr->pid = 0;
                        break;
                    }
                }
            }

            kill(running_child.pid, SIGINT);
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
