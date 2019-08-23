#ifndef SHELL_HEADER
#define SHELL_HEADER

/******************************************************************************
* define: MAX_PATH
* Description: Posix std of max path chars.
*****************************************************************************/
#define MAX_PATH 1024

/******************************************************************************
* define: MAX_COMMAND_SIZE
* Decripion: Max command size, this will be used in malloc.
*****************************************************************************/
#define START_COMMAND_SIZE 64

/******************************************************************************
* define: COMMAND_BUF_SIZE
* Decripion: Starting buf size, this will be used in malloc and the buf should 
* be resized on demand.
*****************************************************************************/
#define COMMAND_BUF_SIZE 10

/******************************************************************************
* define: TOKEN_BUFSIZE
* Decripion: Max ammount of parsed arguments.
*
* Se Also:
* <shell_command_parser>
*****************************************************************************/
#define TOKEN_BUFSIZE 64

/******************************************************************************
* define: TOKEN_DELIM
* Decripion: What is considered a delimiter between arguments.
*
* Se Also:
* <shell_command_parser>
*****************************************************************************/
#define TOKEN_DELIM "\t\r\n\a\b "

/******************************************************************************
* define: SHELL_CLEAN_VALUE
* Decripion: The ammount of newlines when using clean builtin.
*
* See Also:
* <shell_builtin_clean>
*****************************************************************************/
#define SHELL_CLEAN_VALUE 50

/******************************************************************************
* Group: Shell Routines
*****************************************************************************/

void shell_command_loop();
/******************************************************************************
* Function: shell_command_input
* Decripion: Print prompt and get input from user.
*
* Parameters:
*
*   command - Return the user input value via pointer
*
* User Input is Read:
*
*   === C ===
*	while((command[count++] = getchar()) != '\n') {
*		if(count > MAX_COMMAND_LINE) break;
*	}
*   =========
*****************************************************************************/
void shell_command_input(char *command);

void shell_command_prompt();
/******************************************************************************
* Function: shell_command_fork
* Decripion: Fork shell and wait return from child.
*
* Parameters:
* 
* command_argv - Parsed user input
* flag - Check if command was builtin user input
*
* See Also:
* 
*	<shell_command_parser>
*****************************************************************************/
void shell_command_fork(char **command_argv, int *flag);

/******************************************************************************
* Function: shell_command_parser
* Decripion: Parser user input in to null termined array of pointers.
*
* Parameters:
*
*	command_string - Unparsed string
*	argv_buf - Parsed array
*
* See Also:
*
*	<shell_command_input>
*****************************************************************************/
void shell_command_parser(char *command_string, char **argv_buf);

/******************************************************************************
* Function: shell_command_builtin
* Decripion: Check for built-in commands in user input.
* 
* Parameters:
* 
*	command_argv - Parsed user input
*****************************************************************************/
void shell_command_builtin(char **command_argv, int *flag);

/******************************************************************************
* Group: Shell Built-in Commands
*****************************************************************************/

/******************************************************************************
* define: BUILTIN_NUMBER
* Decripion: Size of shell_builtin_list
*****************************************************************************/
#define BUILTIN_NUMBER 6
#define BUILTIN_NUMBER_SCAPE 2

/******************************************************************************
* Function: shell_builtin_cd
* Decripion: Change working dir.
*****************************************************************************/
void shell_builtin_cd(const char **argv);

/******************************************************************************
* Function: shell_builtin_bg
* Decripion: Show background process.
*****************************************************************************/
void shell_builtin_bg(const char **argv);

/******************************************************************************
* Function: shell_builtin_fg
* Decripion: Return to last process.
*****************************************************************************/
void shell_builtin_fg(const char **argv);

/******************************************************************************
* Function: shell_builtin_pwd
* Decripion: Print the working path.
*****************************************************************************/
void shell_builtin_pwd(const char **argv);

/******************************************************************************
* Function: shell_builtin_exit
* Decripion: Exit shell.
*****************************************************************************/
void shell_builtin_exit(const char **argv);

/******************************************************************************
* Function: shell_builtin_clean
* Decripion: Clean terminal screen.
*****************************************************************************/
void shell_builtin_clean(const char **argv);

/******************************************************************************
* Function: shell_builtin_printf
* Decripion: Print organized string.
*****************************************************************************/
void shell_builtin_printf(const char **argv);

/******************************************************************************
* struct: shell_list
* Decripion: Containg the built in funcions.
*
* Members:
*	size - Size of the option
*	option - String cotaining compared text
*	func - Function executed when option matches
*****************************************************************************/
struct shell_list {
	int size;
	char *option;
	void (*func)();
};

struct shell_list_ctrl {
	int option;
	void (*func)();
};

#endif /* ifndef SHELL_HEADER */
