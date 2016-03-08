#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include "sh.h"


pid_t c_pid = 0; // child process id
int timeout = 0;


void sig_handler(int signal); 

int main( int argc, char **argv, char **envp ) {
	if (argc == 2) timeout = atoi(argv[1]); // set timeout in seconds
	/* put signal set up stuff here */
	signal(SIGINT, sig_handler);				// Ctrl-C
	signal(SIGALRM, sig_handler);				// Timeout alarm
	sigignore(SIGQUIT);									// Ctrl-D ?
	sigignore(SIGTSTP);									// Ctrl-Z
	sigignore(SIGTERM);									// kill <pid>

  return sh(argc, argv, envp);
}

void sig_handler(int signal) {
  /* define your signal handler */
	switch (signal) {
    case SIGINT:										// Ctrl-C
			if (c_pid > 0) {
				printf("\n");
				kill(c_pid, SIGINT);
			}
			break;
		case SIGALRM:
//			printf("Alarm for %i!\n", c_pid);
			if (c_pid > 0) {
				printf("!!! Taking too long to execute this command !!!\n");
				kill(c_pid, SIGINT);
			}
	}
}

