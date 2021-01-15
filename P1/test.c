#include <stdio.h>
#include<string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

void copyInput(char* src, char* dest) {
	for (int i = 0; i < 256; i++) {
		dest[i] = src[i];
	}
}

int parseAndCountArgs(char* input, char** args) {
	static char* saveState;

	const char delim[] = " \t\r\n\v\f";
	int i = 0;

	args[i] = strtok(input, delim); // tokenize cmd
	while (args[i] != NULL) {
		i++; // increment to loop
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
int findSymbolIndex(char** args, int numArgs, char* symbol) {
	for (int i = 0; i < numArgs; i++) {
		if (strcmp(args[i], symbol) == 0) {
			return i;
		}
	}
	return -1;
}
int hasAmpersand(char** args, int numArgs) {
	char* lastArg = args[numArgs - 1];
	int res = strcmp(lastArg, "&") == 0;
	return res;
}
// ------------------------------------

// void logArgs(char** args, int numArgs, int backgroundFlag, int lessThanIndex, int greaterThanIndex, int pipeIndex) {
void logArgs(char** args, int numArgs) {
	printf("--------LOG-------\n");
	printf("Command: %s\n", args[0]);
	for (int i = 1; i < numArgs; i++) {
		printf("\tParam %d: %s\n", i, args[i]);
	}
	printf("------------------\n");
}

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
	char* args1[MAX_LINE/2 + 1];
	char* args2[MAX_LINE/2 + 1];
	FILE *fp;

	int lessThanIndex = findSymbolIndex(args, numArgs, (char*)"<");
	int greaterThanIndex = findSymbolIndex(args, numArgs, (char*)">");
	int pipeIndex = findSymbolIndex(args, numArgs, (char*)"|");

	// assume there isn't both < and > in args
	if (lessThanIndex != -1 || greaterThanIndex != -1) {
		int splitIndex;
		char* accessMode;
		int fd2;

		if (lessThanIndex != -1) {
			splitIndex = lessThanIndex;
			accessMode = (char*) "r";
			fd2 = 0;
		} else { //greaterThanIndex != -1
			splitIndex = greaterThanIndex;
			accessMode = (char*) "w";
			fd2 = 1;
		}

		splitArgs(args, numArgs, args1, args2, splitIndex);

		fp = fopen(args2[0], accessMode);
		if (fp == NULL) {
			printf("ERR: File not found: %s\n", args2[0]);
			return;
		}

		dup2(fileno(fp), fd2);
		fclose(fp);

		int rc = execvp(args1[0], args1);
		if (rc < 0) { return; }

	} else if (pipeIndex != -1) {
		splitArgs(args, numArgs, args1, args2, pipeIndex);
		int pfd[2]; // open read/write pipe fd
		pipe(pfd); // pipe 

		if (fork() == 0) {      
			close(STDOUT_FILENO); //close stdout 
			dup(pfd[1]); //assign stdout to copy fd pipe for write 
			close(pfd[1]); // close write end of pipe

			execvp(args1[0], args1); // exec on first set of arguments before pipe
		}

		if (fork() == 0) {
			close(STDIN_FILENO); // close stdin
			dup(pfd[0]); // assign stdin to copy fd pipe for read
			close(pfd[0]); // close read end of pipe

			execvp(args2[0], args2); // exec on second set of arguments after pipe
		}
		
		wait(NULL);	// wait 

	} else {
		int rc = execvp(args[0], args);
		if (rc < 0) { return; }
	}

	*runFlag = 0;
}

void handleParentProcess(int backgroundFlag, int status) {
	if (!backgroundFlag) {
		wait(&status);
	}
}

int main(void) {
  char* args[MAX_LINE/2 + 1]; // command line arguments
	int numArgs;
	char* command = NULL;
	char* prevCommand = NULL;
	char input[256];
	char prevInput[256];
	char tempInput[256];
	const int status = 1;
  int runFlag = 1; // flag to determine when to exit program
  int backgroundFlag; // flag to determine when to exit program
	int repeatFlag;
	int pid;

  while (runFlag) {
    printf("ish>");
		fgets(input, 256, stdin);
    fflush(stdout);

		repeatFlag = isRepeatCommand(input);
		if (repeatFlag) {
			copyInput(tempInput, input);
		} else {
			copyInput(input, tempInput);
		}

		numArgs = parseAndCountArgs(input, args);
		runFlag = !isExitCommand(args);
		backgroundFlag = hasAmpersand(args, numArgs);

		if (backgroundFlag) {
			args[numArgs-1] = NULL;
		}

		pid = fork();
		if (pid < 0) { return 1; }
		else if (pid == 0) {
			handleChildProcess(args, numArgs, &runFlag);
		}	else {
			handleParentProcess(backgroundFlag, status);
			copyInput(tempInput, prevInput);
		}
  }

  return 0;
}

