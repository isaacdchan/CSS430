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
    i32 startingFbn =  g_oft[ofte].curs / BYTESPERBLOCK; // get fbn the cursor is currently on
    i32 maxFbn = size / BYTESPERBLOCK; // get the file's last fbn
    if (g_oft[ofte].curs + numb > size) {
      numb = size - g_oft[ofte].curs;
    }
    i32 cursorAtTheFbn = g_oft[ofte].curs % BYTESPERBLOCK;
    i32 lastRequestedByte = g_oft[ofte].curs + numb;
    i32 lastRequestedFbn = (lastRequestedByte / BYTESPERBLOCK); // get the last requested fbn
    if (lastRequestedFbn > maxFbn) { // the user wants more bytes than the file has remaining
        lastRequestedFbn = maxFbn; // floor() the lastRequested fbn
    }
    printf("cursor %d\n", g_oft[ofte].curs);
    printf("starting %d ending %d\n", startingFbn, lastRequestedFbn);
    i8 tempBuf[BYTESPERBLOCK];
    // looping all the fbns from the cursor's current fbn to the lastRequested fbn
    if (startingFbn == lastRequestedFbn) {
      if (bfsRead(inum, startingFbn, tempBuf) != 0) {
            FATAL(ENYI);
      }
      memcpy(buf, tempBuf + cursorAtTheFbn, numb);
      fsSeek(fd, numb, SEEK_CUR);
      return numb;
    }
    int offset = 0; //where at the original buffer 
    int tempBufOffset = 0; //where at the tempBuf start copy
    int copySize = BYTESPERBLOCK; //size of copying
    
    for (i32 fbn = startingFbn; fbn <= lastRequestedFbn; fbn++) {
        // read all the bytes in the corresponding dbn into the buffer
        if (bfsRead(inum, fbn, tempBuf) != 0) {
            FATAL(ENYI);
        }
        if (fbn == startingFbn) { //fisrt block 
          tempBufOffset = cursorAtTheFbn;
          copySize = BYTESPERBLOCK - cursorAtTheFbn;
        }
        if (fbn == lastRequestedFbn) { //last block
          copySize = (numb - (BYTESPERBLOCK - cursorAtTheFbn)) % BYTESPERBLOCK;
          if(copySize == 0) {
            lastRequestedFbn--;
          }
        }
        printf("COPY FBN: %d\n To the buf starting index: %d, and from partBuf at: %d, size: %d\n" , 
        fbn, offset, tempBufOffset, copySize);
        memcpy(buf + offset, tempBuf + tempBufOffset, copySize);            
        if (fbn == startingFbn) { 
        offset += copySize;
        } else {
          offset += BYTESPERBLOCK;
        }
        tempBufOffset = 0;
        copySize = BYTESPERBLOCK;
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

  // ++++++++++++++++++++++++
  // Insert your code here
  // ++++++++++++++++++++++++

  FATAL(ENYI);                                  // Not Yet Implemented!
  return 0;
}
