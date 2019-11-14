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

#include <setjmp.h>

#include "shell.h"

#define MAX_CHILD_NAME 50

char rshell_hist_file[1024];

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

typedef struct child job_t;

void
child_add(job_t *, node_t *);

void
child_chk(void);

void
signal_handler(int sig);
#endif /* ifndef JOBS_HEADER */
