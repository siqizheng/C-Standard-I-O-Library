// --------------------- stdio.cpp -----------------------------------------
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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdarg.h>   
#include <stdlib.h>  
using namespace std;

char decimal[100];

int recursive_itoa(int arg) 
{
	int div = arg / 10;
	int mod = arg % 10;
	int index = 0;

	if (div > 0)
	{
		index = recursive_itoa(div);
	}

	decimal[index] = mod + '0';
	return ++index;
}

char *itoa(const int arg) 
{
	bzero(decimal, 100);
	int order = recursive_itoa(arg);
	char *new_decimal = new char[order + 1];
	bcopy(decimal, new_decimal, order + 1);
	return new_decimal;
}

//prints formatted output to stdout
int printf(const void *format, ...) 
{
	va_list list;
	va_start(list, format);

	char *msg = (char *)format;
	char buf[1024];
	int nWritten = 0;

	int i = 0, j = 0, k = 0;
	while (msg[i] != '\0') 
	{
		if (msg[i] == '%' && msg[i + 1] == 'd')
		{
			buf[j] = '\0';
			nWritten += write(1, buf, j);
			j = 0;
			i += 2;

			int int_val = va_arg(list, int);
			char *dec = itoa(abs(int_val));
			if (int_val < 0)
			{
				nWritten += write(1, "-", 1);
			}	
			nWritten += write(1, dec, strlen(dec));
			delete dec;
		}
		else
		{
			buf[j++] = msg[i++];
		}	
	}
	if (j > 0)
	{
		nWritten += write(1, buf, j);
	}	
	va_end( list );
	return nWritten;
}

//sets the size of an input/output stream buffer
int setvbuf(FILE *stream, char *buf, int mode, size_t size) 
{
	if (mode != _IONBF && mode != _IOLBF && mode != _IOFBF)
	{
		return -1;
	}	

	stream->mode = mode;
	stream->pos = 0;

	if (stream->buffer != (char *)0 && stream->bufown == true)
	{
		delete stream->buffer;
	}
	
	switch ( mode ) 
	{
		case _IONBF:
			stream->buffer = (char *)0;
			stream->size = 0;
			stream->bufown = false;
			break;
		case _IOLBF:
		case _IOFBF:
			if (buf != (char *)0) 
			{
				stream->buffer = buf;
				stream->size   = size;
				stream->bufown = false;
			}
			else 
			{
				stream->buffer = new char[BUFSIZ];
				stream->size = BUFSIZ;
				stream->bufown = true;
			}
			break;
	}
	return 0;
}

//sets the size of an input / output stream buffer
void setbuf(FILE *stream, char *buf) 
{
	setvbuf(stream, buf, ( buf != (char *)0 ) ? _IOFBF : _IONBF , BUFSIZ);
}

//opens a file, 
//returns a pointer to a FILE object that maintains the attributes of the opened file
FILE *fopen(const char *path, const char *mode) 
{
	FILE *stream = new FILE();
	setvbuf(stream, (char *)0, _IOFBF, BUFSIZ);
	
	// fopen( ) mode
	// r or rb = O_RDONLY
	// w or wb = O_WRONLY | O_CREAT | O_TRUNC
	// a or ab = O_WRONLY | O_CREAT | O_APPEND
	// r+ or rb+ or r+b = O_RDWR
	// w+ or wb+ or w+b = O_RDWR | O_CREAT | O_TRUNC
	// a+ or ab+ or a+b = O_RDWR | O_CREAT | O_APPEND

	switch (mode[0])
	{
	//  r	Open text file for reading. 
	//	r + Open for readingand writing.
	case 'r':  
		if (mode[1] == '\0')            // r
		{
			stream->flag = O_RDONLY;
		}
		else if (mode[1] == 'b')
		{
			if (mode[2] == '\0')          // rb
			{
				stream->flag = O_RDONLY;
			}
			else if (mode[2] == '+')      // rb+
			{
				stream->flag = O_RDWR;
			}
		}
		else if (mode[1] == '+')        // r+  r+b
		{
			stream->flag = O_RDWR;
		}
		break;
	//  w	Truncate file to zero length or create text file for writing. 
	//	w + Open for readingand writing.The file is created if it does not exist, otherwise truncated.
	case 'w':
		if (mode[1] == '\0')   // w
		{         
			stream->flag = O_WRONLY | O_CREAT | O_TRUNC;
		}
		else if (mode[1] == 'b')
		{
			if (mode[2] == '\0')          // wb
			{
				stream->flag = O_WRONLY | O_CREAT | O_TRUNC;
			}
			else if (mode[2] == '+')      // wb+
			{
				stream->flag = O_RDWR | O_CREAT | O_TRUNC;
			}
		}
		else if (mode[1] == '+')        // w+  w+b
		{
			stream->flag = O_RDWR | O_CREAT | O_TRUNC;
		}
		break;
    //  a	Open for appending (writing at end of file). The file is created if it does not exist.
	//	a + Open for readingand appending(writing at end of file).
		//The file is created if it does not exist.The initial file position for reading is at the beginning of the file, 
		//but output is always appended to the end of the file.
	case 'a':
		if (mode[1] == '\0')            // a
		{
			stream->flag = O_WRONLY | O_CREAT | O_APPEND;
		}
		else if (mode[1] == 'b')
		{
			if (mode[2] == '\0')          // ab
			{
				stream->flag = O_WRONLY | O_CREAT | O_APPEND;
			}
			else if (mode[2] == '+')      // ab+
			{
				stream->flag = O_RDWR | O_CREAT | O_APPEND;
			}
		}
		else if (mode[1] == '+')        // a+  a+b
		{
			stream->flag = O_RDWR | O_CREAT | O_APPEND;
		}
		break;
	
	}

	mode_t open_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

	//int open(const char *pathname, int flags, mode_t mode);
	if ((stream->fd = open(path, stream->flag, open_mode)) == -1)
	{
		delete stream;
		printf("fopen failed\n");
		stream = NULL;
	}

	return stream;
  
}

//clears an input/output stream buffer
int fpurge(FILE *stream)
{
	stream->pos = 0;
	stream->actual_size =0;

	return 0;
}

//synchronizes an output stream with the actual file
int fflush(FILE *stream) 
{
	if(stream->lastop == 'w')
	{
		//write buff to file
		write(stream->fd, stream->buffer, stream->actual_size);
		//reset buff to empty
		stream->pos = 0;
		//printf("flush\n");
		stream->actual_size = 0;
	}
	return 0;
}

//read from a file
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
	stream->lastop = 'r';
	
    size_t actualSize = size*nmemb;

	// if last operaiton is write, flush and clear the buffer
	if(stream->lastop == 'w')
	{
        fflush(stream);
	}

	// NoBuffer mode just invoke system call
	if(stream->mode == _IONBF)
	{
		size_t readSize = read(stream->fd, ptr, actualSize);
		// reach EOF
		if(readSize == 0)
		{
			stream->eof = true;
		}
		return readSize;
	}
    
    size_t remainBufferSize = stream->actual_size - stream->pos;

	if(actualSize <= remainBufferSize)
	{
		//read all data from buffer
		memcpy(ptr, stream->buffer + stream->pos, actualSize);
		stream->pos += actualSize;
		return actualSize;
	}
	else
	{
		// first read remain data from buffer
		if(remainBufferSize > 0)
		{	
		    memcpy(ptr, stream->buffer + stream->pos, remainBufferSize);
			stream->pos = 0;
			stream->actual_size = 0;
		}
		// remain data need to read greater than buffer size
        size_t readFileSize = actualSize - remainBufferSize;
		
		if(readFileSize >= stream->size)
		{
			size_t readSize = read(stream->fd, (char*)ptr + remainBufferSize, readFileSize);

			if(readSize < readFileSize)
			{
				stream->eof = true;
			}

			return readSize + remainBufferSize;
		}
		else // remain data need to read less than buffer size
		{
			//read to buff
			size_t readBuffSize = read(stream->fd, stream->buffer, stream->size);
			stream->pos = 0;

			stream->actual_size = readBuffSize;

			if(readBuffSize < stream->size)
			{
				stream->eof = true;
			}

			size_t returnSize = readFileSize < readBuffSize ? readFileSize : readBuffSize;
			
			memcpy((char*)ptr+remainBufferSize, stream->buffer, returnSize);
			stream->pos += returnSize;
			return remainBufferSize + returnSize;
		}
		
	}
}

//writes to a file 
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
	stream->lastop = 'w';
	
    size_t actualSize = size*nmemb;

    // no buff mode, just invoke system call and return
	if(stream->mode == _IONBF)
	{
        return write(stream->fd, ptr, actualSize);
	}

    //if current buffer is reading buffer, clear it before write
	if(stream->lastop == 'r')
	{
		stream->pos = 0;
		stream->actual_size = 0;
	}

	//writing size plus actual buffer size is greater than buffer size
	if(actualSize + stream->actual_size >= stream->size )
	{
		fflush(stream);
		write(stream->fd, ptr, actualSize);
	}
	else
	{
        //only write to buffer
        memcpy(stream->buffer + stream->pos, ptr,  actualSize);
        stream->pos += actualSize;
		stream->actual_size += actualSize;
	}
	return actualSize;
}

//reads a character from a file stream 
int fgetc(FILE *stream) 
{
    char* c = new char[1];

	if (fread(c, 1, 1, stream) > 0)
	{
		return c[0];
	}
	else
	{
		return EOF;
	}
}

//writes a character to a file stream 
int fputc(int c, FILE *stream) 
{
	return fwrite(&c, 1, 1, stream);
}

//reads a character from a file stream 
char *fgets(char *str, int size, FILE *stream) 
{
	size_t readCount = fread(str,1,size-1,stream);
	
    if(readCount > 0)
	{
		str[readCount] = '\0';
		return str;
	}
	else 
	{
		str = NULL;
		return str;
	}
}

//writes a character string to a file stream
int fputs(const char *str, FILE *stream) 
{
	return fwrite(str,1,strlen(str),stream);
}

//checks for the end-of-file
int feof(FILE *stream) 
{
	return stream->eof == true;
}

//moves the file position to a specific location in a file 
int fseek(FILE *stream, long offset, int whence) 
{
	//flush written buff before seek
	fflush(stream);
	stream->pos=0;
	stream->actual_size = 0;
    return 	lseek(stream->fd,offset,whence);
}

//close the file
int fclose(FILE *stream) 
{
	//flush written buff before close
	fflush(stream);
	//system call to close file
	close(stream->fd);
	delete stream;
	stream = NULL;
	return 0;
}