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

struct child {
	char *name;
	pid_t pid;
	struct child *next;
} list_child, running_child;

void child_add(struct child *list_head, struct TOKEN *head);
void child_chk();

void signal_sigint(int sig);
void signal_sigtstp(int sig);
#endif /* ifndef JOBS_HEADER */
