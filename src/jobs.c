#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "jobs.h"
#include "shell.h"

void process_child_register(char *child_name)
{
	process_child_list[process_child_numb].number = process_child_last;
	process_child_list[process_child_numb].pid = getpid();
	//strncpy(process_child_list[process_child_numb].name, child_name, sizeof(*child_name)); /* XXX */

	process_child_last = process_child_numb;
	process_child_numb++;
#ifdef DEBUG
	printf("RShell Debug: child number %d\n", process_child_last);
	printf("RShell Debug: child name   %s\n", child_name);
	printf("RShell Debug: child pid    %d\n", getpid());
#endif
}

void process_child_set_fg()
{
	//pid_t foreground_pgid;
}

void signal_sigstop() {}
void signal_sigcont() {}
void signal_sigintr() {}
void signal_sigkill() {}
