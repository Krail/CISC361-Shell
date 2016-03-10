// Defines functions for parsing a command line string
//   Got help from:
//     -> http://stackoverflow.com/a/11198645/5171749
//     -> http://stackoverflow.com/a/14667564/5171749


char** getArgv(char *command_line);
void freeArgv(char **argv);

/* Get the number of tokens in the command line string.
 *   Parameters:
 *     string - commandLine
 *   Returns:
 *     int    - numTokens
 *                the number of tokens in commandLine
 *                including the command and its arguments
 * Verified no memory leaks.
 */
int getNumTokens(char *commandLine);

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
char **buildStringArray(char *commandLine, int numTokens);

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
 *   -> Must free commandAndArgs's elements and itself when finished!
 */
char** getCommandAndArgs(char *commandLine, int numTokens);


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
char* getCommand(char **argv);


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
char** getArgs(char **argv);


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
void freeStringArray(char ** commandAndArgs, int numTokens);

