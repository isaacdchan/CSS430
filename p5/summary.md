# Layer Summary
**fs** - user level view. interact with funcs `fsOpen`, `fsRead`, `fsSeek`, `fsClose`

**bfs** - internal "helper" funcs `bfsFindAvailableBlock`, `bfsInitAvailabilityList`

**bio** - lowest level block IO funcs `bioRead`, `bioWrite`

# **bio** (Block IO)
## Disk Architecture
Unformatted BFS disk has 100 contiguous 512 byte sized blocks
Lives on host filesystem as a file called `BFSDISK`

A block's byte offset is calculated using `block_index * 512`

Blocks must be read/written to in entirety

## Function Prototypes
```c
// read block # b_id from disk into the memory array buf
// return 0 for success. failure aborts the program
i32 bioRead(i32 b_id, void* buf); 

// write the contents of the memory array buf into block # b_id
// return 0 for success. failure aborts the program
i32 bioWrite(i32 b_id, void* buf); // read 
```

# **fs** (User-Level Filesystem)
Keeps track of which of the 100 blocks are free, which blocks are occupied by which files

The blocks a file occupies will grow automatically as user writes more data to the file

`b_id` - the block index 0-100 within the BFS disk (physical mapping)

`fb_id` - the block index within a file (logical mapping)


## <br>Function Headers/Pseudocode
```c
// initilaize BFS disk
i32 fsFormat() {
	if (BFSDISK already exists) {
		delete BFSDISK
	} else {
		res = constructBFSDISK()
		if (res == 0) {
			return res
		} else {
			panic
		}
	}
}
// mount disk to drive
i32 fsMount()
// open file fname. 
// str is typedef for char*
i32 fsOpen(str fname) {
	res = BFSDISK.open(fname)
	if (res == 1) {
		// offset = position a fsRead/fsRead operates
		file.byteOffset = 0
		return fileDescriptor
	} else {
		return FileNotFoundError
	}
}
// reads num bytes from fd starting from cursor offset into buf
// on success: return # of bytes read (could be < num if reach EOL)
// on error: panic
i32 fsRead(i32 fd, i32 num, void* buf)

// write num bytes in buf into fd starting from curor offset. any bytes written past current file length will automatically extend file
i32 fsWrite(i32 fd, i32 num, void* buf)

i32 fsSeek(i32 fd, i32 offset, i32 method) {
	if (method == SEEK_SET) {
		cursor = offset
	} else if (method == SEEK_CUR)  {
		cursor += offset
	} else { // method == SEEK_END
		cursor = (EOF + offset)
	}
}

// close file
i32 fClose(i32 fd) 
// return size in bytes
i32 fsSize(i32 fd)
// return file's current cursor
i32 fsTell(i32 fd)

```

# **bfs** (raw IO level)
## Metablocks
Uses the first 3 blocks out of 100
records which blocks files are stored on and which blocks are free
(hidden from fs interface)

### SuperBlock
`numBlocks` - const 100  
`numINodes` - const 8 (8 max files per BFSDISK)  
`numFree` - free blocks in BFSDISK  
`fressListHead` - block_id of the first free block  
* essentially HEADPTR of `freeList` (linked list containing the free blocks)
* blocks contain u16 cells
* first cell holds the block_id of the next free block (next pointer)

### Inodes
Max 8 per BFSDISK

| Type | Field | Value |
| ----------- | ----------- | ----------- |
| i32 | size | 3172 |
| i16 | physical_block_id0 | 3 |
| i16 | physical_block_id1 | 5 |
| i16 | physical_block_id2 | 8 |
| i16 | physical_block_id3 | 7 |
| i16 | physical_block_id4 | 20 |
| i16 | indirect_block_id | 22 |

Indirect block holds maximum 256 i16 that record the next 256 block_ids
However, there are only 100 blocks total in BFSDISK

### Directory
mapping from fname to iNode number `iNum`
filename is 15 chars max (+ trailing NUL)
maximum 8 entries
empty entries just contain NUL `\0`


## Interface/Implementation

### Writes
3 possible circumstances
* Inside - fits in the currently allocated bytes
* Outside - none of the remaining space in the currently blocks are used
* Overlap - some of the new bytes are placed into existing blocks. new 
  blocks also need to be allocated

## Supporting Files
alias.h provides typedefs (`u8`, `i16`, `str`, `SEEKSET`)
error.c
* Fatal errors print out the file and line number where the error occurred. 

