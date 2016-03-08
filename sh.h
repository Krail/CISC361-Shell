#include "get_path.h"


#define PROMPTMAX 32
#define MAXARGS 10

int sh( int argc, char **argv, char **envp);
char** expandWildcards(char **args);
int isAbsolutePath(char *command);
char* which(char *command, struct pathelement *pathlist);
void where(char *command, struct pathelement *pathlist);
void list ( char *dir );
//void printenv(char **envp);

