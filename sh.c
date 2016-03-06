#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "parseCommandLine.h"
#include "sh.h"


int sh(int argc, char **argv, char **envp) {
  char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandLine = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandPath, *p, *owd, *cwd, *pwd;
//  char **args = calloc(MAXARGS, sizeof(char*));
  char **args;
  int uid, i, status, argsct, go = 1;
  struct passwd *password_entry;
  char *homedir;
  struct pathelement *pathList;

  int numTokens;
  char **commandAndArgs;

  uid = getuid();
  password_entry = getpwuid(uid);               /* get passwd info */
  homedir = password_entry->pw_dir;		/* Home directory to start
						  out with*/
  
  pwd = NULL;
  if ( (cwd = getcwd(NULL, PATH_MAX+1)) == NULL ) {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(cwd) + 1, sizeof(char));
  memcpy(owd, cwd, strlen(cwd));
//  free(pwd);
  //prompt[0] = ' '; prompt[1] = '\0';
  prompt[0] = '\0';

  /* Put PATH into a linked list */
  pathList = get_path();

//printf("PrintWoringDirectory='%s', OriginalWorkingDirectory='%s'\n", pwd, owd);

  while ( go ) {
    /* print your prompt */
    printf("%s [%s]> ", prompt, cwd);

    /* get command line and process */
    //gets(commandLine); // Look into http://stackoverflow.com/a/21198059 for timeout option
    fgets(commandLine, MAX_CANON, stdin);
    commandLine[strlen(commandLine)-1] = '\0';

    // parseCommandLine.h functions
    numTokens = getNumTokens(commandLine);
    commandAndArgs = buildStringArray(commandLine, numTokens); // must be freed properly
    command = getCommand(commandAndArgs, numTokens);
    args = getArgs(commandAndArgs, numTokens);

//printf("commandLine='%s',numTokens='%i',commandAndArgs='%p',command='%s'\n", commandLine, numTokens, commandAndArgs, command);

    /* check for each built in command and implement */
    if (command == NULL) continue;
    else if (strcmp(command, "alias") == 0) {					/* alias */
    } else if (strcmp(command, "cd") == 0) {					/* cd */
      char *twd = calloc(strlen(cwd) + 1, sizeof(char));
      strcpy(twd, cwd);
      if (args == NULL || strcmp(args[0], "~") == 0) {
        if (pwd == NULL) pwd = calloc(PATH_MAX+1, sizeof(char));
        char *home = getenv("HOME");
        if (chdir(home) == 0) {
          strcpy(cwd, home);
          strcpy(pwd, twd);
        }
      } else if (strcmp(args[0], "-") == 0) {
        if (pwd == NULL) printf("No previous directory\n");
        else {
          strcpy(cwd, pwd);
          strcpy(pwd, twd);
        }
      } else {
        if (pwd == NULL) pwd = calloc(PATH_MAX+1, sizeof(char));
        if (chdir(args[0]) == 0) {
          cwd = getcwd(cwd, PATH_MAX+1);
          strcpy(pwd, twd);
        }
      }
      free(twd);
    } else if (strcmp(command, "exit") == 0) {					/* exit */
      printf("exit\n");
      if (numTokens == 1 || numTokens == 2) {
        int status = numTokens == 1 ? 0 : atoi(args[0]);
        freeStringArray(commandAndArgs, numTokens); // freed
        freePathList(pathList);
        free(pwd);
        free(cwd);
        free(owd);
        free(commandLine);
        free(prompt);
        exit(status);
      } else printf("mysh: exit: too many arguments\n");
    } else if (strcmp(command, "history") == 0) {				/* history */
    } else if (strcmp(command, "kill") == 0) {					/* kill */
    } else if (strcmp(command, "list") == 0) {					/* list */
      if (args == NULL) list(cwd);
      else {
        if (numTokens == 2) list(args[0]);
        else {
          for (i = 0; i < numTokens-1; i++) {
	    printf("%s:\n", args[i]);
            list(args[i]);
            if (i != numTokens-2) printf("\n"); // blank space between each dir
          }
        }
      }
    } else if (strcmp(command, "pid") == 0) {					/* pid */
    } else if (strcmp(command, "printenv") == 0) {				/* printenv */
      if (args == NULL) {
        char **env;
        for (env = envp; *env != 0; env++) printf("%s\n", *env);
      } else if (numTokens == 2) {
        char *env = getenv(args[0]);
        if (env != NULL) printf("%s\n", env);
      } else printf("printenv: Too many arguments.\n");
    } else if (strcmp(command, "prompt") == 0) {				/* prompt */
      if (args == NULL) {
        printf("  input prompt prefix: ");
        fgets(prompt, PROMPTMAX, stdin);
        prompt[strlen(prompt)-1] = '\0';
      } else {
        strcpy(prompt, args[0]);
      }
    } else if (strcmp(command, "pwd") == 0) {					/* pwd */
      printf("%s\n", cwd);
    } else if (strcmp(command, "setenv") == 0) {				/* setenv */
      if (args == NULL) { // no arguments, print all environment variables
        char **env;
        for (env = envp; *env != 0; env++) printf("%s\n", *env);
      } else if (numTokens == 2) { // one argument, set env variable to blank
        setenv(args[0], "", 1); // overwrite set
      } else if (numTokens == 3) { // two arguments, set env variable to value
        setenv(args[0], args[1], 1); // overwrite set
      } else printf("setenv: Too many arguments.\n"); // too many arguments (0-2)
    } else if (strcmp(command, "where") == 0) {					/* where */
      for (i = 0; i < numTokens-1; i++) where(args[i], pathList);
    } else if (strcmp(command, "which") == 0) {					/* which */
      char *path;
      for (i = 0; i < numTokens-1; i++) {
        path = which(args[i], pathList);
        if (path != NULL) {
          printf("%s\n", path);
          free(path);
        }
      }
    } else { /* else program to exec */
       /* find it */
       /* do fork(), execve() and waitpid() */
      //if (0) // not found
      //else fprintf(stderr, "%s: Command not found.\n", args[0]);
    }
    freeStringArray(commandAndArgs, numTokens); // freed
  }
  freePathList(pathList);
  free(pwd);
  free(cwd);
  free(owd);
  free(commandLine);
  free(prompt);
  return 0;
} /* sh() */

char *which(char *command, struct pathelement *pathList ) {
   /* loop through pathlist until finding command and return it.  Return
   NULL when not found. */
  if (pathList == NULL) return NULL;
  struct pathelement *node = pathList;
  char *commandPath;
  int commandLen = strlen(command);
  do {
    // GET length of path, /, command, and null terminator.
    commandPath = calloc(commandLen+strlen(node->element)+2, sizeof(char));
    strcpy(commandPath, node->element);
    strcat(commandPath, "/");
    strcat(commandPath, command);
    if (access(commandPath, F_OK | X_OK) == 0) return commandPath;
    node = node->next;
    free(commandPath);
  } while (node != NULL);
  return NULL;
} /* which() */

char *where(char *command, struct pathelement *pathList ) {
  /* similarly loop through finding all locations of command */
  if (pathList == NULL) return NULL;
  struct pathelement *node = pathList;
  char *commandPath;
  int commandLen = strlen(command);
  do {
    // GET length of path, /, command, and null terminator.
    commandPath = calloc(commandLen+strlen(node->element)+2, sizeof(char));
    strcpy(commandPath, node->element);
    strcat(commandPath, "/");
    strcat(commandPath, command);
    // PRINT commandPath if the file exists and is executable.
    if (access(commandPath, F_OK | X_OK) == 0) printf("%s\n", commandPath);
    node = node->next;
    free(commandPath);
  } while (node != NULL);
  return NULL;
} /* where() */

void list(char *dir) {
  /* see man page for opendir() and readdir() and print out filenames for
  the directory passed */
  DIR *dp;
  struct dirent *dirp;

  if ((dp = opendir(dir)) == NULL) {
    printf("list: cannot access %s: No such directory\n", dir);
    return;
  }

  while ((dirp = readdir(dp)) != NULL) printf("%s\n", dirp->d_name);

  closedir(dp);
  return;
} /* list() */

