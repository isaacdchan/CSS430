#include <stdio.h>
#include<string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

int parseArgs(char* rawInput, char** args) {
	static char* saveState;

	const char* delim = " ";
	char* ptr = strtok_r(rawInput, delim, &saveState);

	int i = 0;
	while(ptr != NULL) {
		args[i] = ptr;
		ptr = strtok_r(NULL, delim, &saveState);
		
		i++;
	}

	return i;
}

void logArgs(char** args, int numArgs) {
	char* command = args[0];
	printf("Command: %s\n", command);
	for (int i=1; i < numArgs; i++) {
		printf("\tParam %d: %s\n", i, args[i]);
	}
}

int main(void)
{
  char* args[MAX_LINE/2 + 1]; // command line arguments
	char* prevArgs[MAX_LINE/2 + 1];
	char* prevCommand = NULL;
	char rawInput[256];
  int shouldRun = 1; // flag to determine when to exit program
	int status = 1;

  while (shouldRun) {
    printf("osh>");
		// rawInput is guaranteed to have at least one char
		fgets(rawInput, 256, stdin);;
    fflush(stdout);

		int numArgs = parseArgs(rawInput, args);
		char* command = args[0];

		logArgs(args, numArgs);

		if (strcmp(command, "exit\n") == 0) {
    	printf("exiting!\n");
			shouldRun = 0;
		}
		if (strcmp(command, "!!\n") == 0) {
			if (prevCommand == NULL) {
				// throw error
			} else {
				*command = *prevCommand;
				*args = *prevArgs;
			}
		}

		int inBackground = (args[numArgs - 1] == "&"); // do not pass & into execvp
		int pid = fork();

		if (pid == 0) { // child process
			int rc = execvp(command, args);
			printf("%s\n", command);
			// int rc = execlp("/bin/ls", "ls", "-l", (char *) 0);
			shouldRun = 0;
		}
		else { // parent process
			if (!inBackground) {
				wait(&status);
			}

			*prevCommand = *command;
			*prevArgs = *args;
		}

		printf("pid: %d, shouldRun: %d\n", pid, shouldRun);


    /**
    * After reading user input, the steps are:
    * (1) fork a child process using fork()
    * (2) the child process will invoke execvp()
    * (3) parent will invoke wait() unless command included &
    */
  }

  return 0;
}

