Files and memory are just different "things" that we read/write memory to
* How they are R/W are accomplished differently

## Memory (RAM)
Ask for a specific byte (address register, data register, memory)
* ex. `LOAD(#7AA1, A0)`
L-Value and R-Value table associates mem locations with data
Disappears when power is lost

## Files
Stored sequentially (location + offset)
Historical reasons. Data was stored seqeuntially in disks, tapes, etc.
E.g. if you were readying byte 200 and wanted byte 500, you needed to scan over 300 bytes (using an arm)
Humans generally read data in order too (source files, media files)
Buffers (used to mitigate slowness)
* Based on the observation that when reading a file, almost always going to transfer it to memory
* Will automatically pre-fetch/cache data in the 1024/4096 bytes in memory
* CPU caches are automatic and transparent. No programming at the user-level needed
Helpful abstractions to interact (R/W) files
* don't have to manually (R/W) bytes. *e.g. stdin/stdout)
* IPC via file descriptors


## File Descriptors
Integer value given to user level process (ex. VIM) that acts an access card to the OS that allows the process to read/write/seek/close  
All FDs are stored in a per-process descriptor table
* Ex. fd5 is the fd stored in index 5. Contains a pointer to the file entry in the open file table
* fd0, fd1, fd2, are reserved `stdin, stdout, stderr`

Run `ulimit -a` to see max number of open files (max size of FD table) a process can have open at once  
Multiple file descriptors can point to the same entry in the open file table - but different positions/modes

## Open File Table
Kernel level
For each open file, contains a refcount (# of FDs pointing at this entry) AND pointer to V-node

## Partition layout
Blocks of a partitions can either be
* super block: specifies which blocks are i-nodes and where the directory block is
* Directory block: lists where the i-node block for a specific file are
	* Just a file. Will change depending on which directory you are in
* i-node blocks: contain the i-nodes for each file on partition
	* i-node contains file metadata (permissions, size, type) and which blocks the file is stored on
	* i-nodes should be small so more files can be stored on disk at a time
* data block: self-explanatory


## Partitions/Drives/Volumes
Partitions are portions of a physical drive that appear as separate drives. After a partitions i created, it must be given a drive letter and is now a drive.  
Drive is synonomous with volume


## UFS i-node
4K bytes per block (2^12 bytes)
15 pointers in each node (each pointer is 32 bits 2^2 bytes)
	* 12 pointers point directly to data
	* 13th points to single indirect
		* block which contains pointers to data blocks
		* Can hold 2^12 / 2^2 pointers = 2^10
	* 14th points to double indirect block
		* block which contains 2^10 pointers to single indirect block
	* 15th points to triple index block

Total of (12 + 2^10 + 2^20 + 2^30) * 2^12 bytes in a single i-node
If your file is smaller than 12 * 4K, the first 12 pointers will contain all the data of your file