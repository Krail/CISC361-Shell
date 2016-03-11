#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <glob.h>

#include "./parseCommandLine.h"


// http://stackoverflow.com/a/11198645/5171749
// http://stackoverflow.com/a/14667564/5171749


/* Alloc the string array of the command and its arguments.
 *   Parameters:
 *     string   - command_line
 *   Returns:
 *     string[] - argv
 *                  string array of the command_line's
 *                  command and arguments
 * Verified no memory leaks.
 *   - Must free argv's elements with freeArgv(argv).
 */
char** allocArgv(char *command_line) {
	// Definitions
	const char *deliminator = " ";
	char *token,
			 *copy_command_line = malloc((strlen(command_line)+1)*sizeof(char));

	strcpy(copy_command_line, command_line);
	token = strtok(copy_command_line, deliminator);

	if (token == NULL) {	// Not a single token (i.e. no command)
		free(copy_command_line);
		return NULL;
	}
	
	// Define glob and argv
	int glob_result, i = 1;
	char **glob_args,
			 **argv = malloc(2*sizeof(char*));
	glob_t *arg_paths = malloc(sizeof(glob_t));

	// Get command
	argv[0] = malloc((strlen(token)+1)*sizeof(char)); // command is special
	strcpy(argv[0], token);

	// Get first argument
	token = strtok(NULL, deliminator);
	while (token != NULL) {
		// Glob token with possible wildcard chars
		glob_result = glob(token, 0, NULL, arg_paths);
		// Wildchar found and matched!
		if (glob_result == 0) {
			for (glob_args = arg_paths->gl_pathv; *glob_args != NULL; glob_args++) {
				// Args
				argv = realloc(argv, (i+2)*sizeof(char*));
				argv[i] = malloc((strlen(*glob_args)+1)*sizeof(char));
				strcpy(argv[i], *glob_args);
				i++;
			}
			globfree(arg_paths);
		// Wildcard not found OR wildchar found and no match
		} else if (glob_result == GLOB_NOMATCH) {
			argv = realloc(argv, (i+2)*sizeof(char*));
			argv[i] = malloc((strlen(token)+1)*sizeof(char));
			strcpy(argv[i], token);
			i++;
		}
		token = strtok(NULL, deliminator);		
	}
	// NULL terminate the string array
	argv[i] = NULL;
	// FREE local vars
	free(copy_command_line); free(arg_paths);
	return argv;
}


/* Get the command in commandAndArgs
 *   Parameters:
 *     string[] - commandAndArgs
 *                  string array of the commandLine's
 *                  command and arguments
 *     int      - numTokens
 *                  the number of tokens in commandLine
 *   Returns:
 *     string   - command
 *                  the command in commandAndArgs
 * Verified no memory leaks.
 */
char* getCommand(char **argv) {
  if (argv == NULL) return NULL;
  return argv[0];
}


/* Get the arguments in commandAndArgs
 *   Parameters:
 *     string[] - commandAndArgs
 *                  string array of the commandLine's
 *                  command and arguments
 *     int      - numTokens
 *                  the number of tokens in commandLine
 *   Returns:
 *     string   - args
 *                  the arguments in commandAndArgs
 * Verified no memory leaks.
 */
char ** getArgs(char **argv) {
  if (argv == NULL || *(argv+1) == NULL) return NULL;
  return (argv+1); // return pointer to second element
}                              // that's the first arg


/* Free up argv strings
 *		Parameters:
 *			string[] - argv
 *										string array of the command and arguments
 *		Returns:
 *			void
 * Verified no memory leaks.
 */
void freeArgv(char **argv) {
	if (argv == NULL) return;
	char **argv_p = argv;
	while (*argv_p) {
		free(*argv_p);
		argv_p++;
	}
	free(argv);
}

