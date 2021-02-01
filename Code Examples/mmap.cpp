#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <iostream>
#include <cstring>
using namespace std;

// shmget only returns a chunk of shared memory
// you have shmat attach it to our memory space

int main(int argc, char *argv[]) {
	int input, output;
	size_t fileSize = 10;
	void *source, *target;

	// open a file
	input = open(argv[1], O_RDONLY);
	if (input == -1) {
		cerr << "Could not open file" << endl;
		return -1;
	}
	// create a space in memory the size of the file
	source = mmap(0, fileSize, PROT_READ, MAP_SHARED, input, 0);

	// open another file that the input will write to
	output = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
	// seek to the beginning of the file?
	// can also use SEEK_END to seek from end of file
	int result = lseek(output, fileSize, SEEK_SET);
	
	if (result == -1) {
		close(output);
		perror("Error calling lseek() to 'stretch' the file");
		return -1;
	}
	// write blank to beginning of output file
	write(output, "", 1); 

	// map memory in the output file of size fileSize
	target = mmap(0, fileSize, PROT_WRITE, MAP_SHARED, output, 0);
		
	cout << "target " << target << endl;

	memcpy(target, source, fileSize);

	// deallocate (unmap) the shared memory space in both source and target
	munmap(source, fileSize);
	munmap(target, fileSize);

	close(input);
	close(output);
	return 0;  
}
