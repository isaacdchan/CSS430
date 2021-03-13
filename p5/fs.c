// ============================================================================
// fs.c - user FileSytem API
// ============================================================================

#include "bfs.h"
#include "fs.h"

// ============================================================================
// Close the file currently open on file descriptor 'fd'.
// ============================================================================
i32 fsClose(i32 fd) { 
  i32 inum = bfsFdToInum(fd);
  bfsDerefOFT(inum);
  return 0; 
}



// ============================================================================
// Create the file called 'fname'.  Overwrite, if it already exsists.
// On success, return its file descriptor.  On failure, EFNF
// ============================================================================
i32 fsCreate(str fname) {
  i32 inum = bfsCreateFile(fname);
  if (inum == EFNF) return EFNF;
  return bfsInumToFd(inum);
}



// ============================================================================
// Format the BFS disk by initializing the SuperBlock, Inodes, Directory and 
// Freelist.  On succes, return 0.  On failure, abort
// ============================================================================
i32 fsFormat() {
  FILE* fp = fopen(BFSDISK, "w+b");
  if (fp == NULL) FATAL(EDISKCREATE);

  i32 ret = bfsInitSuper(fp);               // initialize Super block
  if (ret != 0) { fclose(fp); FATAL(ret); }

  ret = bfsInitInodes(fp);                  // initialize Inodes block
  if (ret != 0) { fclose(fp); FATAL(ret); }

  ret = bfsInitDir(fp);                     // initialize Dir block
  if (ret != 0) { fclose(fp); FATAL(ret); }

  ret = bfsInitFreeList();                  // initialize Freelist
  if (ret != 0) { fclose(fp); FATAL(ret); }

  fclose(fp);
  return 0;
}


// ============================================================================
// Mount the BFS disk.  It must already exist
// ============================================================================
i32 fsMount() {
  FILE* fp = fopen(BFSDISK, "rb");
  if (fp == NULL) FATAL(ENODISK);           // BFSDISK not found
  fclose(fp);
  return 0;
}



// ============================================================================
// Open the existing file called 'fname'.  On success, return its file 
// descriptor.  On failure, return EFNF
// ============================================================================
i32 fsOpen(str fname) {
  i32 inum = bfsLookupFile(fname);        // lookup 'fname' in Directory
  if (inum == EFNF) return EFNF;
  return bfsInumToFd(inum);
}



// ============================================================================
// Move the cursor for the file currently open on File Descriptor 'fd' to the
// byte-offset 'offset'.  'whence' can be any of:
//
//  SEEK_SET : set cursor to 'offset'
//  SEEK_CUR : add 'offset' to the current cursor
//  SEEK_END : add 'offset' to the size of the file
//
// On success, return 0.  On failure, abort
// ============================================================================
i32 fsSeek(i32 fd, i32 offset, i32 whence) {

  if (offset < 0) FATAL(EBADCURS);
 
  i32 inum = bfsFdToInum(fd);
  i32 ofte = bfsFindOFTE(inum);
  
  switch(whence) {
    case SEEK_SET:
      g_oft[ofte].curs = offset;
      break;
    case SEEK_CUR:
      g_oft[ofte].curs += offset;
      break;
    case SEEK_END: {
        i32 end = fsSize(fd);
        g_oft[ofte].curs = end + offset;
        break;
      }
    default:
        FATAL(EBADWHENCE);
  }
  return 0;
}



// ============================================================================
// Return the cursor position for the file open on File Descriptor 'fd'
// ============================================================================
i32 fsTell(i32 fd) {
  return bfsTell(fd);
}



// ============================================================================
// Retrieve the current file size in bytes.  This depends on the highest offset
// written to the file, or the highest offset set with the fsSeek function.  On
// success, return the file size.  On failure, abort
// ============================================================================
i32 fsSize(i32 fd) {
  i32 inum = bfsFdToInum(fd);
  return bfsGetSize(inum);
}


// ===========================================================================
// Read 'numb' bytes of data from the cursor in the file currently fsOpen'd on
// File Descriptor 'fd' into 'buf'.  On success, return actual number of bytes
// read (may be less than 'numb' if we hit EOF).  On failure, abort
// ============================================================================
i32 fsRead(i32 fd, i32 numb, void* buf) {
    i32 inum = bfsFdToInum(fd); // find inode
    i32 ofte = bfsFindOFTE(inum); // find OFTE of inode
    i32 size = bfsGetSize(inum); // get # of bytes of file
    i32 curs = g_oft[ofte].curs;
    i32 startingFbn =  curs / BYTESPERBLOCK; // get fbn the cursor is currently on
    i32 maxFbn = size / BYTESPERBLOCK; // get the file's last fbn
    i32 lastRequestedByte = curs + numb;
    i32 lastRequestedFbn = (lastRequestedByte / BYTESPERBLOCK); // get the last requested fbn

    // the last requestedByte is not the last byte of a block
    // there will be leftover bytes
    if (lastRequestedByte % BYTESPERBLOCK != 0) {
        lastRequestedFbn++;
    }
    if (lastRequestedFbn > maxFbn) { // the user wants more bytes than the file has remaining
        lastRequestedFbn = maxFbn; // floor() the lastRequested fbn
    }

    i8 bufPart[BYTESPERBLOCK];
    int offset = 0;
    // looping all the fbns from the cursor's current fbn to the lastRequested fbn
    for (i32 fbn = startingFbn; fbn < lastRequestedFbn; fbn++) {
        // read all the bytes in the corresponding dbn into the buffer
        if (bfsRead(inum, fbn, bufPart) != 0) {
            FATAL(EBADREAD);
        }
        void* bufPartVoid = bufPart;
        memcpy(buf + offset, bufPart, 512);
        offset += 512;
    }

    fsSeek(fd, numb, SEEK_CUR);
    return numb;
}


// ============================================================================
// Write 'numb' bytes of data from 'buf' into the file currently fsOpen'd on
// filedescriptor 'fd'.  The write starts at the current file offset for the
// destination file.  On success, return 0.  On failure, abort
// ============================================================================
i32 fsWrite(i32 fd, i32 numb, void* buf) {
    i32 inum = bfsFdToInum(fd); // find inode
    i32 ofte = bfsFindOFTE(inum); // find OFTE of inode
    i32 size = bfsGetSize(inum); // get # of bytes of file
    i32 curs = g_oft[ofte].curs;

    i32 currFbn = curs / BYTESPERBLOCK; // get fbn the cursor is currently on
    i32 currLastFbn = size / BYTESPERBLOCK; // get the file's last fbn

    i8 currBlockBytes[BYTESPERBLOCK];
    i8 bytesToWrite[BYTESPERBLOCK];
    i32 bytesWritten = 0;

    // first utilize the blocks already allocated
    while (currFbn <= currLastFbn) {
        // if numb reaches 0 in this while loop, the write fits within pre-allocated blocks
        if (numb == 0) {
            break;
        }

        i32 currFbnStartingByte = currFbn * BYTESPERBLOCK;
        i32 cursorBlockIndex = curs - currFbnStartingByte;
        i32 trailingBytes = BYTESPERBLOCK - cursorBlockIndex;

        // read bytes of currFbn into temp_buf
        bfsRead(inum, currFbn, currBlockBytes);

        // calculate the number of bytes that will be written in currFbn
        // cursorBlockIndex + numBytesToWrite = BYTESPERBLOCK
        i32 numBytesToWrite = min(trailingBytes, numb);

        // copy numBytestoWrite bytes from buf into bytesToWrite
        // starting from an offset of total bytesWritten
        memcpy(bytesToWrite, (buf + bytesWritten), numBytesToWrite);

        // copy the bytes to write from buf into bytesToWrite
        // starting from an offset of cursorBlockIndex
        memcpy((currBlockBytes + cursorBlockIndex), bytesToWrite, numBytesToWrite);

        // move cursor to end of write location
        fsSeek(fd, numBytesToWrite, SEEK_CUR);
        // decrement the number of bytes that LEFT to be written
        numb-=numBytesToWrite;
        // increment the number of bytes that HAVE been written
        bytesWritten+=numBytesToWrite;

        // write the temp_buf to currDbn
        i32 currDbn = bfsFbnToDbn(inum, currFbn);
        bioWrite(currDbn, currBlockBytes); // write temp_buf as the new dbn
        currFbn+=1;
    }

    // there are still bytes that need to be written
    // addtl blocks will need to be allocated
    if (numb != 0) {
        i32 addtlBlocksReq = numb / BYTESPERBLOCK;
        i32 newLastDbn = currFbn + addtlBlocksReq;

        while (currFbn < newLastDbn) {


            currFbn+=1;
            // will have to modify file size
        }
    }

    return 0;
}


i32 max(i32 num1, i32 num2) {
    return (num1 > num2 ) ? num1 : num2;
}
i32 min(i32 num1, i32 num2) {
    return (num1 > num2 ) ? num2 : num1;
}
