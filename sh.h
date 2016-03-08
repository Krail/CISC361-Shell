
#include "get_path.h"

int pid;
int sh( int argc, char **argv, char **envp);
int isAbsolutePath(char *command);
char *which(char *command, struct pathelement *pathlist);
void where(char *command, struct pathelement *pathlist);
void list ( char *dir );
void printenv(char **envp);

#define PROMPTMAX 32
#define MAXARGS 10
