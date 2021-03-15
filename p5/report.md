# fsWrite

## Initialization
Before writing, retrieve these pieces of information about the file
* iNode
* Open File Table Entry
* Size
* Cursor location
* File block the cursor is on
* Last file block number

Initialize these variables
* Array to store a disc block's bytes
* Array to store the slice of bytes from `buf` to be written
* `i32` to track how many bytes have been written to file
* `i32` to track how many bytes were written outside of the previously allocated blocks
* `i32` which stores how many bytes are to the write of cursor
* `i32` to track how many bytes to write in the current block

## Main `while` Loop
Infinite while loop that only breaks when there are no more bytes to write

1. Break if `numb == 0`
2. If the current file block number equals the file's last file block number *and* loop hasn't broken
   * Extend the file by the addtional blocks required to fit remaining bytes
3. Calculate the number of bytes to the right of cursor
4. Calculate the number of bytes to write by taking the minimum of the bytes
   we have left to write and the bytes we have available
5. Read the bytes of the current `dbn` into a `temp_buf1`
6. Clear the buffer of bytes we will write to disc
7. Copy the bytes we will write from `buf` into  `temp_buf2`
   * Copy the calculated bytes to write starting from the previous location in `buf`
8. Copy `temp_buf2` into `temp_buf1` starting from the cursor's location in the `dbn`
9. Seek the cursor to its new location in file
10. Increment number of bytes we've written
11. Decerment number of bytes left to write
12. Write `temp_buf1` into the current `dbn`
13. Increment current `fbn` by 1

After all `numb` bytes have been written, update the new file size and return the bytes written


# fsRead
## Initialization
Before writing, retrieve these pieces of information about the file
* iNode (i32 inum)
* Open File Table Entry (i32 ofte)
* Size (i32 size)
* Cursor location (i32 curs)
* File block the cursor is on (i32 startingFbn)
* Last file block number (i32 maxFbn)

Initialize these variables
* i32 to save last byte to read (i32 lastRequestedByte)
* i32 to save the last fbn to read (i32 lastRequestedFbn)
* i32 to keep track of the first byte to read each time (i32 currFbnStartingByte)
* i32 to keep track of the first byte to copy to buf from tempbuf (i32 cursorBlockIndex)
* i8 array to save data from one fbn (i8 tempBuf[BYTESPERBLOCK])
* int to keep track of the starting index of the buf to copy to (int offset)
* int to keep track of the starting index of the tempbuf to copy from  (int tempBufOffset)
* int to keep track of the size to copy from temp buf to buf (int copySize)

## Main Flow

1. Calculate last requested byte. If last requested byte exceed actual file size, reassign the numb to size - cursor position, and last requested byte to the end of the file (size)
2. Calculate last requested FBN, and starting FBN
3. Calculate distance between starting byte of starting FBN and cursor 

* Case 1: Read only one FBN. (starting FBN equals lastRequested FBN) 
4. Read FBN and save it to the tempBuf
5. Copy the contents in tempBuf to buf from the cursorBlockIndex with the size of numb.
6. Move the cursor by numb.
7. Return numb.  

* Case2 :Read more than one FBN (For loop from starting FBN to last Requested FBN)
4. Read FBN and save it to the tempBuf
5. If currently reading block is the firstly read block, set offset of the tempbuf to be same as the distance between starting byte of starting FBN and cursor 
6. If currently reading block is the lastly read block, set the copy size to numb minus starting byte of the last FBN.
7. Copy the contents in tempBuf from the tempBufOffset with the size of copySize at offset of buf.
8. If currently reading block is firstly read block, add copysize to the offset, else, add BYTEPERBLOCK (512) to the offset
9. Set tempBufOffset to 0.
10. Set copy size to BYTEPERBLOCK (512)
11. Move the cursor by numb.
12. Return numb.  
