// This file was not part of the original Wolf3D source

#ifndef FILEIO_H_INCLUDED
#define FILEIO_H_INCLUDED

#include <stdio.h>
#include <string>

#define O_BINARY 0x1
#define O_RDONLY 0x2
#define O_CREAT 0x4
#define O_WRONLY 0x8
#if !defined(_WIN32)
#define S_IREAD 0x10
#define S_IWRITE 0x20
#define S_IFREG 0x40
#endif
#define O_TEXT 0x80

#define FA_ARCH 0x1

#include <dirent.h>

struct ffblk
{
    DIR *dir;
    std::string upExt;
    char ff_name[1024 + 1];
};

int file_open(const char* filename, int flags, int sb = 0);
void file_read(int handle, void* out, size_t size);
void file_write(int handle, const void* in, size_t size);
void file_close(int handle);
int file_seek(int handle, long pos, int flag);
long filelength(int handle);
int findfirst(const char* ext, ffblk* f, int flag);
int findnext(ffblk* f);
void closefind(ffblk* f);

#endif
