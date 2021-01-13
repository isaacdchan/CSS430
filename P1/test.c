#include <stdio.h>
#include<string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

char* parseArgs(char* rawInput) {
  char* args[MAX_LINE/2 + 1]; // command line arguments
	static char* saveState;

	const char* delim = " ";
	char* ptr = strtok_r(rawInput, delim, &saveState);

	int i = 0;
	while(ptr != NULL) {
		args[i] = ptr;
		ptr = strtok_r(NULL, delim, &saveState);
		i++;
	}
	
	return *args;
}

int isRepeatCommand(char* rawInput) {
	return (strcmp(rawInput, "!!\n") != 0);
}

int isExitCommand(char* command) {
	return (strcmp(command, "exit\n") != 0);
}

void logArgs(char** args) {
	int numArgs = sizeof(args) / sizeof(args[0]);
	printf("Num args: %d\n", numArgs);

	char* command = args[0];
	printf("Command: %s\n", command);
	for (int i=1; i < numArgs; i++) {
		printf("\tParam %d: %s\n", i, args[i]);
	}
}
int checkAmpersand(char** args) {
	int numArgs = sizeof(args) / sizeof(args[0]);
	int inBackground = (args[numArgs - 1] == "&"); // do not pass & into execvp
}

void handleChildProcess(char* command, char** args, int* shouldRun) {
	printf("In child\n");

	int rc = execvp(command, args);
	shouldRun = 0;
	printf("Finished child\n");
}
void handleParentProcess(int inBackground, int status, char** args, char** prevArgs) {
	printf("In parent\n");
	if (!inBackground) {
		wait(&status);
	}

	printf("Finished parent\n");
	*prevArgs = *args;
}

int main(void) {
  char* args[MAX_LINE/2 + 1]; // command line arguments
  char* prevArgs[MAX_LINE/2 + 1]; // command line arguments
	int numArgs;
	char* command = NULL;
	char* prevCommand = NULL;
	char rawInput[256];
	char prevRawInput[256];
  int shouldRun = 1; // flag to determine when to exit program
  int inBackground = 0; // flag to determine when to exit program
	int status = 1;
	int pid;

  while (shouldRun) {
    printf("osh>");
		// rawInput is guaranteed to have at least one char
		fgets(rawInput, 256, stdin);;
		printf("Raw Input:%s\n", rawInput);
    fflush(stdout);

		int repeat = isRepeatCommand(rawInput);
		if (repeat) {
			*args = *prevArgs;
		} else {
			*args = parseArgs(rawInput);
		}
		logArgs(args);

		command = args[0];
		shouldRun = isExitCommand(command);
		printf("Should Run:%d\n", shouldRun);
		inBackground = checkAmpersand(args);

		pid = fork();

		if (pid == 0) {
			handleChildProcess(command, args, &shouldRun);
		}	else {
			handleParentProcess(inBackground, status, args, prevArgs);
		}
		printf("pid: %d, shouldRun: %d\n", pid, shouldRun);
  }

  return 0;
}

