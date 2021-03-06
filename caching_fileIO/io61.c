#include "io61.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#define BUFSZ 8192

// io61.c
//    YOUR CODE HERE!


// io61_file
//    Data structure for io61 file wrappers. Add your own stuff.

struct io61_file {
   int fd;
   unsigned char cbuf[BUFSZ];
   off_t tag;      // file offset of the first character in the cache
   off_t end_tag;  // file offset of first invalid position in cache
   off_t pos_tag;  // file offset of next char to write
   int mode;

};


 //io61_fdopen(fd, mode) {

//return f;
//    Return a new io61_file for file descriptor `fd`. `mode` is
//    either O_RDONLY for a read-only file or O_WRONLY for a
//    write-only file. You need not support read/write files.
 
// }


io61_file* io61_fdopen(int fd, int mode) {
    
    assert(fd >= 0);
    io61_file* f = (io61_file*) malloc(sizeof(io61_file));
    f->fd = fd;
    (void) mode;
    f->mode=mode;
    f->tag = f->pos_tag = f->end_tag = 0;
    return f;
}


// io61_close(f)
//    Close the io61_file `f` and release all its resources.

int io61_close(io61_file* f) {
    if (f->mode == O_WRONLY)
    io61_flush(f);
    int r = close(f->fd);
    free(f);
    return r;
}


// io61_readc(f)
//    Read a single (unsigned) character from `f` and return it. Returns EOF
//    (which is -1) on error or end-of-file.

int io61_readc(io61_file* f) {
   /*int a=io61_read(f, f->cbuf, 1);
        if (a != 0)
        return f->cbuf[0];
        else 
            return EOF;
            */
    unsigned char buf[1];
    int a=io61_read(f, (char*) buf, 1);
        if (a == 1)
        return *buf;
        else 
            return EOF;
        
    }
   /* unsigned char buf[1];
    if (read(f->fd, buf, 1) == 1)
        return buf[0];
    else
        return EOF;
        */ 



// io61_read(f, buf, sz)
//    Read up to `sz` characters from `f` into `buf`. Returns the number of
//    characters read on success; normally this is `sz`. Returns a short
//    count, which might be zero, if the file ended before `sz` characters
//    could be read. Returns -1 if an error occurred before any characters
//    were read.
            /*
            io61_read(f, buf, sz)
            f->cpos = f->csz = 0;
            assert(f->cpos <= f->csz);
            */ 

ssize_t io61_read(io61_file* f, char* buf, size_t sz) {
    assert(f->pos_tag >= f->tag && f->pos_tag <= f->tag + BUFSZ);
          size_t pos = 0;   // number of characters read so far
    while (pos != sz) {
            assert(f->pos_tag >= f->tag && f->pos_tag <= f->tag + BUFSZ);
            if (f->pos_tag < f->end_tag) {   
                ssize_t n = sz - pos;
                if (n > f->end_tag - f->pos_tag)
                    n = f->end_tag - f->pos_tag;
                memcpy(&buf[pos], &f->cbuf[f->pos_tag - f->tag], n);
                f->pos_tag += n;
                pos += n;
                assert(f->pos_tag >= f->tag && f->pos_tag <= f->tag + BUFSZ);
            } 
            else {
                f->tag = f->end_tag;
                ssize_t n = read(f->fd, f->cbuf, BUFSZ);
                if (n > 0)
                    f->end_tag += n;
                else
                    return pos ? pos : 0;
            }
    }
    //assert(f->pos_tag >= f->tag && f->pos_tag <= f->tag + BUFSZ);
    return pos;
}




       // return read(f->fd, buf, sz);
    /* size_t nread = 0;
    while (nread != sz) {
        int ch = io61_readc(f);
        if (ch == EOF)
            break;
        buf[nread] = ch;
        ++nread;
    }
    if (nread != 0 || sz == 0 || io61_eof(f))
        return nread;
    else
        return -1;

        */ 



// io61_writec(f)
//    Write a single character `ch` to `f`. Returns 0 on success or
//    -1 on error.

int io61_writec(io61_file* f, int ch) {
    
    
    /*int *i = &ch;
    const char* c = (char*) i;
    int a=io61_write(f, c, 1);
        if (a == 1)
        return 0;
        else 
            return -1;
        
    } */

    
    unsigned char buf[1];
    buf[0] = ch;
    if (io61_write(f, (const char*)buf, 1) == 1)
        return 0;
    else
        return -1;
        
}



// io61_write(f, buf, sz)
//    Write `sz` characters from `buf` to `f`. Returns the number of
//    characters written on success; normally this is `sz`. Returns -1 if
//    an error occurred before any characters were written.

ssize_t io61_write(io61_file* f, const char* buf, size_t sz) {
  if ((f->mode & O_ACCMODE) == O_RDONLY){
    return -1;
  }

    if(f->mode != O_WRONLY) 
        return -1;
     size_t pos = 0;
   while (pos != sz) {
       if (f->pos_tag - f->tag < BUFSZ) { // There is still space in the buffer
           ssize_t n = sz - pos; // Calculate bytes left to write
           if (BUFSZ - (f->pos_tag - f->tag) < n)
               n = BUFSZ - (f->pos_tag - f->tag);
           memcpy(&f->cbuf[f->pos_tag - f->tag], &buf[pos], n);
           f->pos_tag += n;
           if (f->pos_tag > f->end_tag)
                     f->end_tag = f->pos_tag;
           pos += n;
       }
       // The position should never exceed the end tag.
       assert(f->pos_tag <= f->end_tag);

       // Check if we've filled the buffer and if so, call flush to write data.
       if (f->pos_tag - f->tag == BUFSZ)
           io61_flush(f);
   }

   return pos;

}
    //return write(f->fd, buf, sz);
   /* size_t nwritten = 0;
    while (nwritten != sz) {
        if (io61_writec(f, buf[nwritten]) == -1)
            break;
        ++nwritten;
    }
    if (nwritten != 0 || sz == 0)
        return nwritten;
    else
        return -1;
        */ 



// io61_flush(f)
//    Forces a write of all buffered data written to `f`.
//    If `f` was opened read-only, io61_flush(f) may either drop all
//    data buffered for reading, or do nothing.

int io61_flush(io61_file* f) {
if ((f-> mode & O_ACCMODE) != O_RDONLY) {

     if (f->end_tag != f->tag) {
            ssize_t n = write(f->fd, f->cbuf, f->end_tag - f->tag);
            assert(n == f->end_tag - f->tag);
        }

        f->pos_tag = f->tag = f->end_tag;
        

    //(void) f;
    //return 0;
}
return 0;

}


// io61_seek(f, pos)
//    Change the file pointer for file `f` to `pos` bytes into the file.
//    Returns 0 on success and -1 on failure.

/* int io61_seek(io61_file* f, off_t pos) {

        if (pos >= f->tag && pos <= f->end_tag) {
        f->pos_tag = pos;
        return 0;
    } else {
        off_t aligned_off = pos - (pos % BUFSZ);      // ******
    off_t r = lseek(f->fd, (off_t) pos, SEEK_SET);
    if (r == (off_t) pos) {                        // ******
            f->tag = f->end_tag = aligned_off;         // ******
            f->pos_tag = pos;                          // ******
            return 0;
        } else
            return -1;
    }
}
*/ 



int io61_seek(io61_file* f, off_t pos) { 
    if ((f->mode & O_ACCMODE) != O_RDONLY) {
        io61_flush(f);
        off_t r = lseek(f->fd, pos, SEEK_SET);
        if (r !=pos){
              return -1;
          }
        f->tag = f->end_tag = pos;
    }

    else if (pos < f->tag || pos > f->end_tag) {
        off_t aligned_pos = pos -(pos%BUFSZ);
        off_t r = lseek(f->fd, aligned_pos, SEEK_SET);
        if (r != aligned_pos){
              return -1;
              
        }   
    f->tag = f->end_tag = aligned_pos;   
    }

    f->pos_tag = pos;
    return 0;
}
/*
        return 0;
    else
        return -1;
}
*/

// You shouldn't need to change these functions.

// io61_open_check(filename, mode)
//    Open the file corresponding to `filename` and return its io61_file.
//    If `filename == NULL`, returns either the standard input or the
//    standard output, depending on `mode`. Exits with an error message if
//    `filename != NULL` and the named file cannot be opened.

io61_file* io61_open_check(const char* filename, int mode) {
    int fd;
    if (filename)
        fd = open(filename, mode, 0666);
    else if ((mode & O_ACCMODE) == O_RDONLY)
        fd = STDIN_FILENO;
    else
        fd = STDOUT_FILENO;
    if (fd < 0) {
        fprintf(stderr, "%s: %s\n", filename, strerror(errno));
        exit(1);
    }
    return io61_fdopen(fd, mode & O_ACCMODE);

}


// io61_filesize(f)
//    Return the size of `f` in bytes. Returns -1 if `f` does not have a
//    well-defined size (for instance, if it is a pipe).

off_t io61_filesize(io61_file* f) {
    struct stat s;
    int r = fstat(f->fd, &s);
    if (r >= 0 && S_ISREG(s.st_mode))
        return s.st_size;
    else
        return -1;
}


// io61_eof(f)
//    Test if readable file `f` is at end-of-file. Should only be called
//    immediately after a `read` call that returned 0 or -1.

int io61_eof(io61_file* f) {
    char x;
    ssize_t nread = read(f->fd, &x, 1);
    if (nread == 1) {
        fprintf(stderr, "Error: io61_eof called improperly\n\
  (Only call immediately after a read() that returned 0 or -1.)\n");
        abort();
    }
    return nread == 0;
}
