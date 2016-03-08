#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "./parseCommandLine.h"


// http://stackoverflow.com/a/11198645/5171749
// http://stackoverflow.com/a/14667564/5171749


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
char * getCommand(char **commandAndArgs, int numTokens) {
  if (numTokens == 0) return NULL;
  return commandAndArgs[0];
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
char ** getArgs(char **commandAndArgs, int numTokens) {
  if (numTokens == 0 || numTokens == 1) return NULL;
  return (&commandAndArgs[1]); // return pointer to second element
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
