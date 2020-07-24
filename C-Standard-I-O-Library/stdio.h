// --------------------- stdio.h -----------------------------------------
//
// Siqi Zheng, CSS 503
// Created:         May. 17, 2020
// Last Modified:   May. 25, 2020
// --------------------------------------------------------------
// Purpose: implement my own core input and output functions of the C/C++ standard I/O library
// The implemented functions are : 
// fopen: opens a file
// fflush: synchronizes an output stream with the actual file
// setbuf, setvbuf: sets the size of an input / output stream buffer
// fpurge: clears an input / output stream buffer
// fread: reads from a file
// fwrite: writes to a file
// fgetc: reads a character from a file stream
// fputc: writes a character to a file stream
// fgets: reads a character string from a file stream
// fputs: writes a character string to a file stream
// fseek: moves the file position to a specific location in a file
// feof: checks for the end - of - file
// fclose: closes a file
// ---------------------------------------------------------------------------

#ifndef _MY_STDIO_H_
#define _MY_STDIO_H_

#define BUFSIZ 8192 // default buffer size
#define _IONBF 0    // unbuffered
#define _IOLBF 1    // line buffered
#define _IOFBF 2    // fully buffered
#define EOF -1      // end of file

class FILE 
{
 public:
     FILE()
     {
         fd = 0;
         pos = 0;
         buffer = (char*)0;
         size = 0;
         actual_size = 0;
         mode = _IONBF;
         flag = 0;
         bufown = false;
         lastop = 0;
         eof = false;
     }


     int fd;          // a Unix file descriptor of an opened file
     int pos;         // the current file position in the buffer
     char* buffer;    // an input or output file stream buffer
     int size;        // the buffer size
     int actual_size; // the actual buffer size when read( ) returns # bytes read smaller than size
     int mode;        // _IONBF, _IOLBF, _IOFBF
     int flag;        // O_RDONLY 
                      // O_RDWR 
                      // O_WRONLY | O_CREAT | O_TRUNC
                      // O_WRONLY | O_CREAT | O_APPEND
                      // O_RDWR   | O_CREAT | O_TRUNC
                      // O_RDWR   | O_CREAT | O_APPEND
     bool bufown;     // true if allocated by stdio.h or false by a user
     char lastop;     // 'r' or 'w' 
     bool eof;        // true if EOF is reached
  
};
#include "stdio.cpp"
#endif
