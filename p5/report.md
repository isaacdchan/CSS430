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