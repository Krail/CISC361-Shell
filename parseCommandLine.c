#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <glob.h>

#include "./parseCommandLine.h"


// http://stackoverflow.com/a/11198645/5171749
// http://stackoverflow.com/a/14667564/5171749


char** getArgv(char *command_line) {
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
//printf("firsttoken='%s'\n", token);

	int glob_result, i = 1;
	char **glob_args,
			 **argv = malloc(2*sizeof(char*));
	glob_t *arg_paths = malloc(sizeof(glob_t));

	argv[0] = malloc((strlen(token)+1)*sizeof(char)); // command is special
	strcpy(argv[0], token);
//printf("command=argv[0]='%s'\n", argv[0]);
	token = strtok(NULL, deliminator);
	while (token != NULL) {
//printf("token='%s'\n", token);
		glob_result = glob(token, 0, NULL, arg_paths);
		if (glob_result == 0) {
			for (glob_args = arg_paths->gl_pathv; *glob_args != NULL; glob_args++) {
				// Args
				argv = realloc(argv, (i+2)*sizeof(char*));
				argv[i] = malloc((strlen(*glob_args)+1)*sizeof(char));
				strcpy(argv[i], *glob_args);
//printf("args[%i]=argv[%i]='%s'\n", i-1, i, argv[i]);
				i++;
			}
			globfree(arg_paths);
		} else if (glob_result == GLOB_NOMATCH) {
			argv = realloc(argv, (i+2)*sizeof(char*));
			argv[i] = malloc((strlen(token)+1)*sizeof(char));
			strcpy(argv[i], token);
//printf("args[%i]=argv[%i]='%s'\n", i-1, i, argv[i]);
			i++;
		}
		token = strtok(NULL, deliminator);		
	}
//printf("length='%i'\n", i);
	argv[i] = NULL;
//if (argv[i] == NULL) printf("argv[%i]='%s'\n", i, argv[i]);
//else printf("NOT NULL");
	free(copy_command_line); free(arg_paths);
	return argv;
}


/* Get the number of tokens in the command line string.
 *   Parameters:
 *     string - commandLine
 *   Returns:
 *     int    - numTokens
 *                the number of tokens in commandLine
 *                including the command and its arguments
 * Verified no memory leaks.
 */
int getNumTokens(char *commandLine) {

  // COPY commandLine parameter
  char *commandLineCopy = calloc(strlen(commandLine)+1, sizeof(char));
  strcpy(commandLineCopy, commandLine);

  // ASSIGN deliminator to separate tokens
  char *delim = " ";

  // GET first token (don't count this token... implementation specific)
  char *token = strtok(commandLineCopy, delim);

  // LOOP through tokens
  int numTokens;
  for(numTokens = 0; token != NULL; numTokens++) token = strtok(NULL, delim);

  // FREE copy of commandLine parameter
  free(commandLineCopy);

  // RETURN number of tokens
  return numTokens;
}


/* Get the string array of the command and its arguments.
 *   Parameters:
 *     string   - commandLine
 *     int      - numTokens
 *                  the number of tokens in commandLine
 *   Returns:
 *     string[] - commandAndArgs
 *                  string array of the commandLine's
 *                  command and arguments
 * Verified no memory leaks.
 *   - Must free commandAndArgs's elements and itself when finished
 */
char** buildStringArray(char *commandLine, int numTokens) {

  // CHECK if there are tokens
  if (numTokens == 0) return NULL;

  // COPY commandLine parameter
  char *commandLineCopy = calloc(strlen(commandLine)+1, sizeof(char));
  strcpy(commandLineCopy, commandLine);

  // ALLOCATE the string array
  char **commandAndArgs = malloc((numTokens+1)*sizeof(char*));

  // ASSIGN deliminator to separate tokens
  char *delim = " ";

  // GET first token
  char *token = strtok(commandLineCopy, delim);

  // LOOP through tokens and build string array
  char *commandArg;
  int i;
  for(i = 0; token != NULL; i++) {

    // ALLOCATE each string in the array
    commandArg = malloc((strlen(token)+1)*sizeof(char));

    // COPY token into string array
    strcpy(commandArg, token);
    commandAndArgs[i] = commandArg;
//printf("'%s' vs '%s' vs '%s'\n", token, commandArg, commandAndArgs[i]);

    // GET next token
    token = strtok(NULL, delim);
  }
	commandAndArgs[numTokens] = 0;

  // FREE copy of commandLine parameter
  free(commandLineCopy);

  // RETURN string array of tokens (including command[0] and arguments[>1])
  return commandAndArgs;
}

/*
char** expandWildcards(char **commandAndArgs) {
	char **args_p = commandAndArgs+1;                     // points to first argument
	char **glob_args;                                     // Glob array pointer
	char **new_commandAndArgs = malloc(2*sizeof(char*));  // New args to be returne
  new_commandAndArgs[0] = malloc((strlen(commandAndArgs[0])+1)*sizeof(char));
  strcpy(new_commandAndArgs[0], commandAndArgs[0]);
  glob_t *arg_paths = malloc(sizeof(glob_t));           // Returned paths by glob
  int i = 1;
  while (*args_p) {
    if (glob(*args_p, 0, NULL, arg_paths) == 0) { // 0?, NULL?
      for (glob_args = arg_paths->gl_pathv; *glob_args != NULL; glob_args++, i++) {
        //printf("NewArg='%s'\n", *glob_args);
        // Args
        new_commandAndArgs = realloc(new_commandAndArgs, (i+2)*sizeof(char*));
        new_commandAndArgs[i] = malloc((strlen(*glob_args)+1)*sizeof(char));
        strcpy(new_commandAndArgs[i], *glob_args);
      }
      globfree(arg_paths);
    }
    args_p++;
  }
  new_commandAndArgs[i] = 0;
  return new_commandAndArgs;
}*/


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


/* Free up the string array of the command and its arguments
 *   Parameters:
 *     string[] - commandAndArgs
 *                  string array of the commandLine's
 *                  command and arguments
 *     int      - numTokens
 *                  the number of tokens in commandLine
 *   Returns:
 *     void
 * Verified no memory leaks.
 */
void freeStringArray(char ** commandAndArgs, int numTokens) {
  if (commandAndArgs == NULL) return;
  int i;
  for(i = 0; i < numTokens+1; i++) free(commandAndArgs[i]);
  free(commandAndArgs);
}

void freeArgv(char **argv) {
	if (argv == NULL) return;
	char **argv_p = argv;
//printf("first: '%s'\n", *argv_p);
	//while (*(argv_p++)) free(*argv_p);
	while (*argv_p) {
//printf("free: '%s'\n", *argv_p);
		free(*argv_p);
		argv_p++;
	}
	free(argv);
}

