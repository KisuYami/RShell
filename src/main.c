#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "shell.h"
#include "jobs.h"

int main(int argc, char *argv[])
{
	shell_command_loop();
	return 0;
}
