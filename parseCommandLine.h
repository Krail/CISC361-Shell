// Defines functions for parsing a command line string
//   Got help from:
//     -> http://stackoverflow.com/a/11198645/5171749
//     -> http://stackoverflow.com/a/14667564/5171749



/* Alloc the string array of the comand and its arguments
 *		Parameters:
 *			string		- command_line
 *		Returns:
 *			string[]	- argv
 *										string array of the command_line's
 *										command and arguments
 */
char** allocArgv(char *command_line);


/* Get the command in argv
 *   Parameters:
 *     string[] - argv
 *                  string array of the commandLine's
 *                  command and arguments
 *   Returns:
 *     string   - command
 *                  the command in argv
 * Verified no memory leaks.
 */
char* getCommand(char **argv);


/* Get the arguments in argv
 *   Parameters:
 *     string[] - argv
 *                  string array of the commandLine's
 *                  command and arguments
 *   Returns:
 *     string   - args
 *                  the arguments in argv
 * Verified no memory leaks.
 */
char** getArgs(char **argv);

/* Free up argv of the command and its arguments
 *		Parameters:
 *    	string[] - argv
 *     								string array of the command_line's
 *                  	command and arguments
 *		Returns:
 *     	void
 * Verified no memory leaks.
 */
void freeArgv(char **argv);

