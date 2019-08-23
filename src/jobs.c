#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "jobs.h"
#include "shell.h"

void process_child_register(char *child_name)
{
	process_child_list.number = process_child_last;
	process_child_list.pid = getpgrp();
	strncpy(process_child_list.name, child_name, strlen(child_name));

#ifdef DEBUG
	printf("RShell Debug: child number %d\n", process_child_last);
	printf("RShell Debug: child name   %s\n", child_name);
	printf("RShell Debug: child pid    %d\n", getpid());
#endif
}

void signal_sigstop(pid_t child_pgid) {
	if(kill(process_child_list.pid, 0) != ESRCH)  {
		kill(child_pgid, SIGSTOP);
#ifdef DEBUG
		printf("RShell: signal_sigstop called\n");
		printf("RShell: stoppped pgid %d\n", child_pgid);
		printf("RShell: continued pgid %d\n", process_parent.pid);
#endif
	}
	tcsetpgrp(STDERR_FILENO, process_parent.pid);
}

void signal_sigintr() {
	if(kill(process_child_list.pid, 0) != ESRCH)
		kill(process_child_list.pid, SIGINT);
#ifdef DEBUG
	printf("RShell Debug: child killed by SIGINT\n");
#endif
}
