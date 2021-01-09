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

void checkRepeatCommand(char* command, char* prevCommand) {
	if (strcmp(command, "!!\n") == 0) {
		if (prevCommand == NULL) {
			// throw error
		} else {
			*command = *prevCommand;
			printf("Assigned command to: %s\n", prevCommand);
			// *args = *prevArgs;
		}
	}
}
void checkExitCommand(char* command, int* shouldRun) {
	if (strcmp(command, "exit\n") == 0) {
		*shouldRun = 0;
	}
}

void logArgs(char** args, int numArgs) {
	char* command = args[0];
	printf("Command: %s\n", command);
	for (int i=1; i < numArgs; i++) {
		printf("\tParam %d: %s\n", i, args[i]);
	}
}
int checkAmpersand(char** args, int numArgs) {
	int inBackground = (args[numArgs - 1] == "&"); // do not pass & into execvp
}

void handleChildProcess(char* command, char** args, int* shouldRun) {
	printf("In child\n");

	int rc = execvp(command, args);
	shouldRun = 0;
	printf("Finished child\n");
}
void handleParentProcess(int inBackground, int status, char* prevCommand, char * command) {
	printf("In parent\n");
	if (!inBackground) {
		wait(&status);
	}

	strcpy(prevCommand, "test");
	printf("Assigned prevCommand to: %s\n", command);
	printf("Finished parent\n");
	// *prevArgs = *args;
}

int main(void)
{
  char* args[MAX_LINE/2 + 1]; // command line arguments
	char* prevArgs[MAX_LINE/2 + 1];
	char* command = NULL;
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
		command = args[0];

		// logArgs(args, numArgs);
		checkRepeatCommand(command, prevCommand);
		checkExitCommand(command, &shouldRun);
		int inBackground = checkAmpersand(args, numArgs);

		int pid = fork();

		if (pid == 0) {
			handleChildProcess(command, args, &shouldRun);
		}	else {
			handleParentProcess(inBackground, status, prevCommand, command);
		}
		// printf("pid: %d, shouldRun: %d\n", pid, shouldRun);

  }

  return 0;
}

