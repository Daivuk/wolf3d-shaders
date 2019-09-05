// This file was not part of the original Wolf3D source

#ifndef FILEIO_H_INCLUDED
#define FILEIO_H_INCLUDED

#include <stdio.h>

#define O_BINARY 0x1
#define O_RDONLY 0x2
#define O_CREAT 0x4
#define O_WRONLY 0x8
#define S_IREAD 0x10
#define S_IWRITE 0x20
#define S_IFREG 0x40
#define O_TEXT 0x80

#define FA_ARCH 0x1

#define ffblk int

int file_open(const char* filename, int flags, int sb = 0);
void file_read(int handle, void* out, size_t size);
void file_write(int handle, const void* in, size_t size);
void file_close(int handle);
int file_seek(int handle, long pos, int flag);
long filelength(int handle);
int findfirst(const char* ext, int* f, int flag);

#endif
