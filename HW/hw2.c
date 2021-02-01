#include <stdio.h>
#include <unistd.h>

int main() {
	/* fork a child process */
	fork();
	/* fork another child process */
	fork();
	/* and fork another */
	fork();
	return 0;
}
//----------------------------------------
#include <stdio.h>
#include <unistd.h>

int main() {
	for (int i = 0; i < 4; i++) {
		fork();
		return 0;
	}
}
//----------------------------------------
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main() {
	pid_t pid, pid1;
	/* fork a child process */
	pid = fork();

	if (pid < 0) { /* error occurred */
		fprintf(stderr, "Fork Failed");
		return 1;
	} else if (pid == 0) { /* child process */
		pid1 = getpid();
		printf("child: pid = %d",pid); /* A */
		printf("child: pid1 = %d",pid1); /* B */
	} else { /* parent process */
		pid1 = getpid();

		printf("parent: pid = %d",pid); /* C */
		printf("parent: pid1 = %d",pid1); /* D */
		wait(NULL);
	}
	
	return 0;
}
//----------------------------------------
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>  
#define SIZE 5

int nums[SIZE] = {0,1,2,3,4};
 
int main() {
	int i;
	pid_t pid;
	pid = fork();

	if (pid == 0) {
		for (i = 0; i < SIZE; i++) {
			nums[i] *= -i;
			printf("CHILD: %d ",nums[i]); /* LINE X */
		}
	} else if (pid > 0) {
		wait(NULL);

		for (i = 0; i < SIZE; i++) {
			printf("PARENT: %d ",nums[i]); /* LINE Y */
		}
	}

	return 0;
}