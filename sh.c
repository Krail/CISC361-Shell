#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>
#include <glob.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
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
  char **commandAndArgs, **args, **my_argv;
	int go = 1;
  int num_history = 0;
  int uid, i, status, argsct, numTokens;


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


	// MAIN CONTROL LOOP
  while ( go ) {
    /* print your prompt */
		printf("%s [%s]> ", prompt, cwd);

		/* get command line and process */
    if (fgets(commandLine, MAX_CANON, stdin) == NULL) {
			printf("\nUse \"exit\" to leave mysh.\n");
			continue;
		}
		commandLine[strlen(commandLine)-1] = '\0'; // Replace newline with null terminator
 

		my_argv = allocArgv(commandLine);
		command = getCommand(my_argv);
		args = getArgs(my_argv);


    if (command != NULL) { // Save command line history
      num_history++;
      command_history = (char**) realloc(command_history, (num_history+1)*sizeof(*command_history));
      command_history[num_history-1] = malloc((strlen(commandLine)+1)*sizeof(char));
      strcpy(command_history[num_history-1], commandLine);
    }

    /* check for each built in command and implement */
    if (command == NULL) continue;
    else if (strcmp(command, "alias") == 0) {					/* alias */
			printf("Executing built-in %s\n", command);
      if (args == NULL) printAliasTable(alias_head);		// No args, print alias table.
			else if (args[1] == NULL) {												// only one arg, print this alias's command
				char *alias_command = getAlias(alias_head, args[0]);
        if (alias_command != NULL) printf("%s\n", alias_command);
      } else {																					// multiple args, set alias's command
        char *alias_command = calloc(MAX_CANON, sizeof(char));
        strcpy(alias_command, args[1]);
				if (args[2] != NULL) {													// append args to this alias's command
					char **args_p = args+2;
					for (i = 2; *args_p; i++, args_p++) {
						strcat(alias_command, " ");
						strcat(alias_command, args[i]);
					}
				}
				alias_head = setAlias(alias_head, args[0], alias_command);
        free(alias_command);
      }
    } else if (strcmp(command, "cd") == 0) {					/* cd */
			printf("Executing built-in %s\n", command);
			char *twd = calloc(strlen(cwd) + 1, sizeof(char));
      strcpy(twd, cwd);
			if (args != NULL && args[1] != NULL) printf("%s: Too many arguments.\n", command);	// too many args
			else if (args == NULL || strcmp(args[0], "~") == 0) {			// there's no arguments or only argument is a tilde, go home
        if (pwd == NULL) pwd = calloc(PATH_MAX+1, sizeof(char));			// no previous working directory
        char *home = getenv("HOME");
        if (chdir(home) == 0) {							// change directory to home, success?
          strcpy(cwd, home);
          strcpy(pwd, twd);
        }
      } else if (strcmp(args[0], "-") == 0) {		// only argument is a hyphen, go to previous working directory
        if (pwd == NULL) printf("No previous directory\n");
        else {
					if (chdir(pwd) == 0) {						// change directory to previous working directory, success?
						strcpy(cwd, pwd);
						strcpy(pwd, twd);
					}
        }
			} else {												// only argument is not a tilde or a hyphen, go to relative/absolute address
				if (pwd == NULL) pwd = calloc(PATH_MAX+1, sizeof(char));		
        if (chdir(args[0]) == 0) {			// change directory to relative/absolute address, success?
          cwd = getcwd(cwd, PATH_MAX+1);
          strcpy(pwd, twd);
        }
      }
      free(twd);			// free local variable (temporary working directory)
    } else if (strcmp(command, "exit") == 0) {					/* exit */
			printf("Executing built-in %s\n", command);
      printf("exit\n");
			if (args == NULL || args[1] == NULL) {		// 0 or 1 argument, SUCCESS
				int status = args == NULL ? 0 : atoi(args[0]);
        // free all local variables
				freeArgv(my_argv);
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
      } else printf("mysh: exit: too many arguments\n");	// too many arguments (2+)
    } else if (strcmp(command, "history") == 0) {				/* history */
			printf("Executing built-in %s\n", command);
      int n = 10;
      if (args != NULL) n = atoi(args[0]);	// user specified number of history to print
      if (n > num_history) n = num_history;		// number to print is larger than the actual number of history
          for (i = num_history - n; i < num_history; i++)	
              printf("\t%i\t%s\n", i+1, command_history[i]);	// print each history element
    } else if (strcmp(command, "kill") == 0) {					/* kill */
			printf("Executing built-in %s\n", command);
      if (args == NULL) printf("kill: usage: kill [-signum] pid\n");	// no args, error
			else if (args[1] == NULL) {			// only one argument, use SIGTERM
				const int pid = atoi(args[0]);
        if (kill(pid, SIGTERM) == -1)
            printf("kill: (%i) - No such process\n", pid);	// failure
      } else {
				int signum = atoi(args[0]+1); // ignore - infront of number
				if (signum > 31) signum = 0;
				char **args_p = args+1;
				for (i = 1; *args_p; i++) {			// loop through each pid
					const int pid = atoi(args[i]);
          if (kill(pid, signum) == -1)
              printf("kill: (%i) - No such process\n", pid);	// failure
					args_p++;
        }
      }
    } else if (strcmp(command, "list") == 0) {					/* list */
			printf("Executing built-in %s\n", command);
      if (args == NULL) list(cwd);
      else if (args[1] == NULL) list(args[0]); // only one arg, list that arg
			else {	// at least 2 arguments, list each arg
				char **args_p = args;
				for (i = 0; *args_p; i++) {
					printf("%s:\n", args[i]);
					list(args[i]);
					if (args[i+1] != NULL) printf("\n"); // blank space between each dir
					args_p++;
				}
      }
    } else if (strcmp(command, "pid") == 0) {					/* pid */
			printf("Executing built-in %s\n", command);
      printf("%i\n", getpid());	// print pid
    } else if (strcmp(command, "printenv") == 0) {				/* printenv */
			printf("Executing built-in %s\n", command);
      if (args == NULL) {				// no args, print all environment variables
        char **env;
        for (env = environ; *env != 0; env++) printf("%s\n", *env);
			} else if (args[1] == NULL) {			// only one argument, print this environment variable
				char *env = getenv(args[0]);
        if (env != NULL) printf("%s\n", env);		// exists? print it
      } else printf("printenv: Too many arguments.\n");	// too many arguments (2+)
    } else if (strcmp(command, "prompt") == 0) {				/* prompt */
			printf("Executing built-in %s\n", command);
      if (args == NULL) {	// no args, prompt user for prompt prefix
        printf("  input prompt prefix: ");
        fgets(prompt, PROMPTMAX, stdin);
        prompt[strlen(prompt)-1] = '\0'; // set new line char to null terminator
      } else {
        strcpy(prompt, args[0]);	// set prompt to first arg
      }
    } else if (strcmp(command, "pwd") == 0) {					/* pwd */
			printf("Executing built-in %s\n", command);
      printf("%s\n", cwd);	// print current working directory
    } else if (strcmp(command, "setenv") == 0) {				/* setenv */
			printf("Executing built-in %s\n", command);
      if (args == NULL) { // no arguments, print all environment variables
        char **env;
        for (env = environ; *env != 0; env++) printf("%s\n", *env);
			} else if (args[1] == NULL) {	 // one argument, set environment variable to blank
					setenv(args[0], "", 1); // overwrite set
        if (strcmp(args[0], "PATH") == 0) {
					freePathList(pathList);
					pathList = get_path(); // update path list
				}
			} else if (args[2] == NULL) {			// two arguments, set environment variable (arg[0]) to arg[1]
				setenv(args[0], args[1], 1); // overwrite set
				if (strcmp(args[0], "PATH") == 0) {
					freePathList(pathList);
					pathList = get_path();	// udate path list
				}
			} else printf("setenv: Too many arguments.\n"); // too many arguments (3+)
    } else if (strcmp(command, "where") == 0) {					/* where */
			printf("Executing built-in %s\n", command);
			if (args !=	NULL) {		// there are args, print where each binary is
				char **args_p = args;
				for (i = 0; *(args_p++); i++) where(args[i], pathList);
			}
		} else if (strcmp(command, "which") == 0) {					/* which */
			printf("Executing built-in %s\n", command);
			if (args != NULL) {	// there are args, print which each binary is
				char *path;
				char **args_p = args;
				for (i = 0; *(args_p++); i++) {
					path = which(args[i], pathList);
					if (path != NULL) {
						printf("%s\n", path);
						free(path);
					}
				}
			}
		} else { /* else program to exec */
			// Check for alias command!!!
			char *alias_argv = getAlias(alias_head, command);
			if (alias_argv != NULL) {	// Execute alias command if it exists
				freeArgv(my_argv);
				my_argv = allocArgv(alias_argv);
				command = getCommand(my_argv);
				args = getArgs(my_argv);
			}
			// Continue execution
			if (isAbsolutePath(command) == 0) { // is absolute path
				if (access(command, F_OK) == 0) { // is found
					if (access(command, X_OK) == 0) {	// is executable
						struct stat path_stat;
						stat(command, &path_stat);
						if (S_ISREG(path_stat.st_mode)) {
							// Absolute/relative path - Execute command
							// do fork(), execve(), and waitpid()...
printf("absolute/relative and executable.\n");
							c_pid = fork();
							if (c_pid == 0) { // Child process success
								printf("Executing %s\n", command);
								if (execve(command, my_argv, environ) == -1) printf("%s: Command not found.\n", command);
							} else if (c_pid > 0) { // Parent process success
								int status;
								alarm(timeout); // set up alarm (if set at runtime)
								waitpid(c_pid, &status, 0);
								if (WEXITSTATUS(status) != 0) printf("%s: Command exited with status: %d\n", command, WEXITSTATUS(status));
							} else { // Error forking
								printf("%s: Unable to fork child process.\n", command);
							}
							c_pid = 0; // reset c_pid for signal handling
						} else printf("%s: Cannot execute a directory.\n", command); // Do not execute a directory
					} else printf("%s: Permission denied.\n", command); // It is not executable by this user
				} else printf("%s: Command not found.\n", command); // Command is not found
      } else { // is not absolute path, FIND IT
				// "Global" executable file name, find it with my 'which' command
				char *commandPath = which(command, pathList);
        if (commandPath == NULL) printf("%s: Command not found.\n", command); // command was not found!!!
        else { // command was found!!!
					// Command found
					// do fork(), execve(), and waitpid()...
					c_pid = fork();
					if (c_pid == 0) { // Child process success
						printf("Executing %s at %s\n", command, commandPath);
						if (execve(commandPath, my_argv, environ) == -1) printf("%s: Command not found.\n", commandPath);
					} else if (c_pid > 0) { // Parent process success
						int status;
						alarm(timeout); // set up alarm (if set at runtime)
						waitpid(c_pid, &status, 0); // set up child process
					  if (WEXITSTATUS(status) != 0) printf("%s: Command exited with status: %d\n", commandPath, WEXITSTATUS(status));
					} else { // Error forking
						printf("%s: Unable to fork child process.\n", commandPath);
					}
					c_pid = 0;
				}
        free(commandPath);
      }
    }
		freeArgv(my_argv); // free argv
  }									/* End of control loop */
  return 0;
} /* sh() */






// is this command an absolute/relative path
int isAbsolutePath(char *command) {
  if (command[0] == '/') return 0;
  else if (command[0] == '.') {
    if (strlen(command) > 2 && command[1] == '/') return 0;
    else if (strlen(command) > 3 && command[1] == '.' && command[2] == '/') return 0;
  }
  return 1;
}

// which executable should this command execute within this path list
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

// what executables could this command execute within this path list
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

// list the files/directories within this directory
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

