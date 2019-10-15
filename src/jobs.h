#ifndef JOBS_HEADER
#define JOBS_HEADER

/******************************************************************************
* File:             jobs.h
*
* Author:           Reberti Carvalho Soares
*
* Created:          09/03/19
*
* Description:      Signal and process handling.
*****************************************************************************/

#include "shell.h"
#include <setjmp.h>

#define MAX_CHILD_NAME 50

jmp_buf prompt_jmp;

enum JOBS_FLAGS {
	JOB_RUNNING = 0,
	JOB_STOPPED = 1,
	JOB_EXITED  = 2,
};

struct child
{
	int state;
    pid_t pid;
    char *name;

    struct child *next;

} list_child, running_child;

void child_add(struct child *list_head, struct TOKEN *head);
void child_chk();

void signal_handler(int sig);
#endif /* ifndef JOBS_HEADER */
