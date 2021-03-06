# Layer Summary
**fs** - user level view. interact with funcs `fsOpen`, `fsRead`, `fsSeek`, `fsClose`

**bfs** - internal "helper" funcs `bfsFindAvailableBlock`, `bfsInitAvailabilityList`

**bio** - lowest level block IO funcs `bioRead`, `bioWrite`

# **bio** (Block IO)
## Disk Architecture
Unformatted BFS disk has 100 contiguous 512 byte sized blocks
Lives on host filesystem as a file called `BFSDISK`

A block's byte offset is calcualted using `block_index * 512`

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

## Supporting Files
i32 is an alias for a 32-bit signed int. defined in *alias.h*

Fatal errors print out the file and line number where the error occurred. See *error.c* and *error.h* for more details

# <br>**fs** (User-Level Filesystem)
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
BFS uses the first 3 blocks (DBN 0,1,2) for  metablocks 
100 blocks
blocks files are stored and which blocks are free
(hidden from fs interface)

** SuperBlock
** Inodes
** Directory


