#include <stdio.h>
#include<string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

// void logArgs(char** args, int numArgs, int backgroundFlag, int lessThanIndex, int greaterThanIndex, int pipeIndex) {
void logArgs(char** args, int numArgs) {
	printf("--------LOG-------\n");
	printf("Command: %s\n", args[0]);
	for (int i = 1; i < numArgs; i++) {
		printf("\tParam %d: %s\n", i, args[i]);
	}
	printf("------------------\n");
}

// function to deep copy array
void copyInput(char* src, char* dest) {
	for (int i = 0; i < 256; i++) {
		dest[i] = src[i];
	}
}

// tokenize the raw user input and store it in the args array
int parseAndCountArgs(char* input, char** args) {
	// possible delimeters. include \n to right strip
	const char delim[] = " \t\r\n\v\f";
	// keep track of number of arguments to return
	int i = 0;

	args[i] = strtok(input, delim);
	while (args[i] != NULL) {
		i++;
		args[i] = strtok(NULL, delim); // store null terminator at end of array
	}
	
	return i;
}
// SYMBOL DETECTION ALGORITHMS
int isRepeatCommand(char* input) {
	int res = strcmp(input, "!!\n") == 0;

	return res;
}
int isExitCommand(char** args) {
	char* command = args[0];
	int res = strcmp(command, "exit") == 0;

	return res;
}
// symbol parameter is the symbol you want to search for
int findSymbolIndex(char** args, int numArgs, char* symbol) {
	for (int i = 0; i < numArgs; i++) {
		if (strcmp(args[i], symbol) == 0) {
			return i;
		}
	}
	return -1;
}

// given an args array and a delimeter, place the args before the delimeter in args1
// and args after the deliemeter into args2
void splitArgs(char** args, int numArgs, char** args1, char** args2, int delimeter) {
	for (int i = 0; i < delimeter; i++) {
		args1[i] = args[i];
	}

	int i = 0;
	for (int j = delimeter + 1; j < numArgs; j++, i++) {
		args2[i] = args[j];
	}

	args1[delimeter] = NULL;
	args2[numArgs - delimeter - 1] = NULL;
}

void handleChildProcess(char** args, int numArgs, int* runFlag) {
	// used if there is a redirection or pipe 
	char* args1[MAX_LINE/2 + 1];
	char* args2[MAX_LINE/2 + 1];
	FILE *fp;

	// check if user wants to redirect from a file
	int lessThanIndex = findSymbolIndex(args, numArgs, (char*)"<");
	// check if user wants to redirect into a file
	int greaterThanIndex = findSymbolIndex(args, numArgs, (char*)">");
	// check if user wants to pipe one process into another
	int pipeIndex = findSymbolIndex(args, numArgs, (char*)"|");

	// assume there isn't both < and > in args
	// it's not guaranteed that a delimeter will be equidistant between left and right
	if (lessThanIndex != -1 || greaterThanIndex != -1) {
		int splitIndex;
		char* accessMode;
		int fd2;

		// if redirecting from a file, will need to read in
		if (lessThanIndex != -1) {
			splitIndex = lessThanIndex;
			accessMode = (char*) "r";
			fd2 = STDIN_FILENO;
		// if redirecting into a file, will need to write
		} else { //greaterThanIndex != -1
			splitIndex = greaterThanIndex;
			accessMode = (char*) "w";
			fd2 = STDOUT_FILENO;
		}

		splitArgs(args, numArgs, args1, args2, splitIndex);

		// open file to read OR write
		fp = fopen(args2[0], accessMode);
		if (fp == NULL) {
			printf("ERR: File not found: %s\n", args2[0]);
			return;
		}

		// either dup2 stdout to file write
		// or dup2 stdin to file read
		dup2(fileno(fp), fd2);

		int rc = execvp(args1[0], args1);
		if (rc < 0) { return; }

	} else if (pipeIndex != -1) {
		splitArgs(args, numArgs, args1, args2, pipeIndex);
		int pfd[2]; // open read/write pipe fd
		pipe(pfd); // pipe to connect the two processes

		int pid = fork(); // fork

		if (pid < 0) { return; }
		// child process who will be be executing command on the left of the vert bar
		// and then writing results to the pipe
		else if (pid == 0) {
			close(pfd[0]);
			dup2(pfd[1], 1);
			int rc = execvp(args1[0], args1);

			if (rc < 0) {
				printf("Command Failed: %s\n", args1[0]);
				return;
			}
		} else { //parent
			// child process who will be be reading the child's results from the pipe
			// and executing the command on the right side of the bar
			close(pfd[1]);
			wait(NULL);
			dup2(pfd[0], 0);
			int rc = execvp(args2[0], args2);
			if (rc < 0) {
				printf("Command Failed: %s\n", args1[0]);
				return;
			}
		}
	} else {
		int rc = execvp(args[0], args);
		if (rc < 0) { return; }
	}

	*runFlag = 0;
}

// blocking parent process that waits until the child process finishes
// if background flag is 1, parent can create another parent process
void handleParentProcess(int ampersandIndex, int status) {
	if (ampersandIndex == -1) {
		wait(&status);
	}
}

int main(void) {
  char* args[MAX_LINE/2 + 1];
	int numArgs;
	char input[256];
	char prevInput[256];
	const int status = 1;
  int runFlag = 1;
  int ampersandIndex;
	int repeatFlag;
	int pid;

  while (runFlag) {
		sleep(1); // stdout line orientation sync
    printf("ish>");
		fgets(input, 256, stdin); // read user input
    fflush(stdout);

		// first check if the raw user input is "!!"
		// if so, replace user input with previously stored imnput
		if (isRepeatCommand(input)) {
			copyInput(prevInput, input);
		} else {
			copyInput(input, prevInput);
		}

		// tokenize input into args
		numArgs = parseAndCountArgs(input, args);
		// check if the user wants to exit
		runFlag = !isExitCommand(args);
		// check if the user wants the command to run in the background
		ampersandIndex = findSymbolIndex(args, numArgs, (char*)"&");
		// if so, rightstrip the ampersand symbol
		if (ampersandIndex != -1) {
			args[numArgs-1] = NULL;
		}

		pid = fork();
		if (pid < 0) { return 1; }
		else if (pid == 0) { // child
			handleChildProcess(args, numArgs, &runFlag);
		}	else { // parent
			handleParentProcess(ampersandIndex, status);
		}
  }

  return 0;
}