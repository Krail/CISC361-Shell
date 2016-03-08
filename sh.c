#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
//#include <sys/select.h>			//
//#include <sys/time.h>				//
//#include <poll.h>						//
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <errno.h>

#include "parseCommandLine.h"
#include "builtin/alias.h"
#include "sh.h"


extern char **environ;
extern pid_t c_pid;				// defined in my "./main.c"
extern int timeout;				// defined in my "./main.c"

int sh(int argc, char **argv, char **envp) {
	char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandLine = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandPath, *p, *owd, *cwd, *pwd;

  char **command_history = malloc(sizeof(*command_history));
  char **commandAndArgs, **args;
  int go = 1;
  int num_history = 0;
  int uid, i, status, argsct, numTokens;

//	struct pollfd poll = {STDIN_FILENO, POLLIN | POLLPRI};
//	sigset_t mask;
//	sigset_t orig_mask;
//	struct timespec timeout;
//	timeout.tv_sec = 2;
//	timeout.tv_nsec = 0;

  struct alias *alias_head = NULL;
  struct passwd *password_entry;
  char *homedir;
  struct pathelement *pathList;


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

  prompt[0] = '\0';

  /* Put PATH into a linked list */
  pathList = get_path();

//printf("PrintWoringDirectory='%s', OriginalWorkingDirectory='%s'\n", pwd, owd);

  while ( go ) {
    /* print your prompt */
		printf("%s [%s]> ", prompt, cwd);

		/* get command line and process */
    if (fgets(commandLine, MAX_CANON, stdin) == NULL) {
			printf("\nUse \"exit\" to leave mysh.\n");
			continue;
		}
		commandLine[strlen(commandLine)-1] = '\0'; // Replace newline with null terminator

    // parseCommandLine.h functions
    numTokens = getNumTokens(commandLine);
    commandAndArgs = buildStringArray(commandLine, numTokens); // must be freed properly
    command = getCommand(commandAndArgs, numTokens);
    args = getArgs(commandAndArgs, numTokens);

		args = expandWildcards(args);

//printf("commandLine='%s',numTokens='%i',commandAndArgs='%p',command='%s'\n", commandLine, numTokens, commandAndArgs, command);

    if (command != NULL) {
      num_history++;
      command_history = (char**) realloc(command_history, (num_history+1)*sizeof(*command_history));
      command_history[num_history-1] = malloc((strlen(commandLine)+1)*sizeof(char));
      strcpy(command_history[num_history-1], commandLine);
    }

    /* check for each built in command and implement */
    if (command == NULL) continue;
    else if (strcmp(command, "alias") == 0) {					/* alias */
			printf("Executing built-in %s\n", command);
      if (args == NULL) printAliasTable(alias_head);
      else if (numTokens == 2) {
        char *alias_command = getAlias(alias_head, args[0]);
        if (alias_command != NULL) printf("%s\n", alias_command);
      } else {
        char *alias_command = calloc(MAX_CANON, sizeof(char));
        strcpy(alias_command, args[1]);
int len = strlen(args[1]);
				for (i = 2; i < numTokens-1; i++) {
          strcat(alias_command, " ");
          strcat(alias_command, args[i]);
len += 1 + strlen(args[i]);
				}
printf("Length='%i', MAX_CANON='%i'\n", len, MAX_CANON);
				alias_head = setAlias(alias_head, args[0], alias_command);
        free(alias_command);
      }
    } else if (strcmp(command, "cd") == 0) {					/* cd */
			printf("Executing built-in %s\n", command);
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
					if (chdir(pwd) == 0) {
						strcpy(cwd, pwd);
						strcpy(pwd, twd);
					}
        }
			} else if (numTokens > 2) {
				printf("%s: Too many arguments.\n", command);
      } else {
				if (pwd == NULL) pwd = calloc(PATH_MAX+1, sizeof(char));
        if (chdir(args[0]) == 0) {
          cwd = getcwd(cwd, PATH_MAX+1);
          strcpy(pwd, twd);
        }
      }
      free(twd);
    } else if (strcmp(command, "exit") == 0) {					/* exit */
			printf("Executing built-in %s\n", command);
      printf("exit\n");
      if (numTokens == 1 || numTokens == 2) {
        int status = numTokens == 1 ? 0 : atoi(args[0]);
        freeStringArray(commandAndArgs, numTokens); // freed
        freePathList(pathList);
        free(pwd);
        free(cwd);
        free(owd);
        free(commandLine);
        for (i = 0; i < num_history; i++) free(command_history[i]);
        free(command_history);
        free(prompt);
        freeAliasTable(alias_head);
        exit(status);
      } else printf("mysh: exit: too many arguments\n");
    } else if (strcmp(command, "history") == 0) {				/* history */
			printf("Executing built-in %s\n", command);
      int n = 10;
      if (args != NULL) n = atoi(args[0]);
      if (n > num_history) n = num_history;
          for (i = num_history - n; i < num_history; i++)
              printf("\t%i\t%s\n", i+1, command_history[i]);
    } else if (strcmp(command, "kill") == 0) {					/* kill */
			printf("Executing built-in %s\n", command);
      if (args == NULL) printf("kill: usage: kill [-signum] pid\n");
      else if (numTokens == 2) {
        const int pid = atoi(args[0]);
        if (kill(pid, SIGTERM) == -1)
            printf("kill: (%i) - No such process\n", pid);
      } else {
				int signum = atoi(args[0]+1);
//printf("SigNum='%i'\n", signum);
				if (signum > 31) signum = 0;
				for (i = 1; i < numTokens-1; i++) {
          const int pid = atoi(args[i]);
          if (kill(pid, signum) == -1)
              printf("kill: (%i) - No such process\n", pid);
        }
      }
    } else if (strcmp(command, "list") == 0) {					/* list */
			printf("Executing built-in %s\n", command);
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
			printf("Executing built-in %s\n", command);
      printf("%i\n", getpid());
    } else if (strcmp(command, "printenv") == 0) {				/* printenv */
			printf("Executing built-in %s\n", command);
      if (args == NULL) {
        char **env;
        for (env = environ; *env != 0; env++) printf("%s\n", *env);
      } else if (numTokens == 2) {
        char *env = getenv(args[0]);
        if (env != NULL) printf("%s\n", env);
      } else printf("printenv: Too many arguments.\n");
    } else if (strcmp(command, "prompt") == 0) {				/* prompt */
			printf("Executing built-in %s\n", command);
      if (args == NULL) {
        printf("  input prompt prefix: ");
        fgets(prompt, PROMPTMAX, stdin);
        prompt[strlen(prompt)-1] = '\0';
      } else {
        strcpy(prompt, args[0]);
      }
    } else if (strcmp(command, "pwd") == 0) {					/* pwd */
			printf("Executing built-in %s\n", command);
      printf("%s\n", cwd);
    } else if (strcmp(command, "setenv") == 0) {				/* setenv */
			printf("Executing built-in %s\n", command);
      if (args == NULL) { // no arguments, print all environment variables
        char **env;
        for (env = environ; *env != 0; env++) printf("%s\n", *env);
      } else if (numTokens == 2) { // one argument, set env variable to blank
        setenv(args[0], "", 1); // overwrite set
        if (strcmp(args[0], "PATH") == 0) {
					freePathList(pathList);
					pathList = get_path();
				}
			} else if (numTokens == 3) { // two arguments, set env variable to value
        setenv(args[0], args[1], 1); // overwrite set
				if (strcmp(args[0], "PATH") == 0) {
					freePathList(pathList);
					pathList = get_path();
				}
			} else printf("setenv: Too many arguments.\n"); // too many arguments (0-2)
    } else if (strcmp(command, "where") == 0) {					/* where */
			printf("Executing built-in %s\n", command);
      for (i = 0; i < numTokens-1; i++) where(args[i], pathList);
    } else if (strcmp(command, "which") == 0) {					/* which */
			printf("Executing built-in %s\n", command);
      char *path;
      for (i = 0; i < numTokens-1; i++) {
        path = which(args[i], pathList);
        if (path != NULL) {
          printf("%s\n", path);
          free(path);
        }
      }
    } else { /* else program to exec */
			if (isAbsolutePath(command) == 0) {
				if (access(command, F_OK) == 0) {
					if (access(command, X_OK) == 0) {
						// Absolute/relative path - Execute command
						// do fork(), execve(), and waitpid()...
//sigemptyset(&mask);
//sigaddset(&mask, SIGCHLD);
//if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) return 1;
						c_pid = fork();
						if (c_pid == 0) { // Child process success
							printf("Executing %s\n", command);
							if (execve(command, commandAndArgs, environ) == -1) printf("%s: Command not found.\n", command);
						} else if (c_pid > 0) { // Parent process success
							int status;
//printf("timeout='%i'\n", timeout);
							alarm(timeout);
							waitpid(c_pid, &status, 0);
							if (WEXITSTATUS(status) != 0) printf("%s: Command exited with status: %d\n", command, WEXITSTATUS(status));

//int timeout = 2;
//int waittime = 0;

//do {
//	if (sigtimedwait(&mask, NULL, &timeout) < 0) {
//		printf("errno='%i'\n", errno);
//	}
//	break;
//} while (1);
//if (waitpid(c_pid, &status, 0) < 0) return 1;
						} else { // Error forking
						  printf("%s: Unable to fork child process.\n", command);
						}
						c_pid = 0;
					} else printf("%s: Permission denied.\n", command);
				} else printf("%s: Command not found.\n", command);
      } else {
				// "Global" executable file name, find it with my 'which' command
				char *commandPath = which(command, pathList);
        if (commandPath == NULL) printf("%s: Command not found.\n", command);
        else {
					// Command found
					// do fork(), execve(), and waitpid()...
					c_pid = fork();
					if (c_pid == 0) { // Child process success
						printf("Executing %s at %s\n", command, commandPath);
						if (execve(commandPath, commandAndArgs, environ) == -1) printf("%s: Command not found.\n", commandPath);
					} else if (c_pid > 0) { // Parent process success
						int status;
//printf("timeout='%i'\n", timeout);
						alarm(timeout);
						waitpid(c_pid, &status, 0);
					  if (WEXITSTATUS(status) != 0) printf("%s: Command exited with status: %d\n", commandPath, WEXITSTATUS(status));
					} else { // Error forking
						printf("%s: Unable to fork child process.\n", commandPath);
					}
					c_pid = 0;
				}
        free(commandPath);
      }
    }
    freeStringArray(commandAndArgs, numTokens); // freed
  }									/* End of control loop */
  return 0;
} /* sh() */


char** expandWildcards(char **args) {
	if (args == NULL) return NULL;
	return args;
}

int isAbsolutePath(char *command) {
  if (command[0] == '/') return 0;
  else if (command[0] == '.') {
    if (strlen(command) > 2 && command[1] == '/') return 0;
    else if (strlen(command) > 3 && command[1] == '.' && command[2] == '/') return 0;
  }
  return 1;
}

char* which(char *command, struct pathelement *pathList ) {
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

void where(char *command, struct pathelement *pathList ) {
  /* similarly loop through finding all locations of command */
  if (pathList == NULL) return;
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

