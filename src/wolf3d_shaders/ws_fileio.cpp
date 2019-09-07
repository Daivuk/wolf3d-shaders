#include "ws_fileio.h"
#include <string>
#include <map>
#include <algorithm>

#include <dirent.h>

static int next_file_handle = 1;
static std::map<int, FILE*> file_handles;

int file_open(const char* filename, int flags, int sb)
{
    std::string f;
    if (flags & O_RDONLY) f += "r";
    if (flags & O_WRONLY) f += "w";
    if (flags & O_BINARY) f += "b";

    auto fic = fopen(filename, f.c_str());
    if (!fic) return -1;

    auto ret = next_file_handle++;
    file_handles[ret] = fic;
    return ret;
}

void Quit(char* err);

void file_read(int handle, void* out, size_t size)
{
    auto it = file_handles.find(handle);
    if (it == file_handles.end()) Quit((char*)"file_read invalid handle");
    fread(out, size, 1, it->second);
}

void file_write(int handle, const void* in, size_t size)
{
    auto it = file_handles.find(handle);
    if (it == file_handles.end()) Quit((char*)"file_write invalid handle");
    fwrite(in, size, 1, it->second);
}

void file_close(int handle)
{
    auto it = file_handles.find(handle);
    if (it == file_handles.end()) Quit((char*)"file_close invalid handle");
    fclose(it->second);
    file_handles.erase(it);
}

int file_seek(int handle, long pos, int flag)
{
    auto it = file_handles.find(handle);
    if (it == file_handles.end()) Quit((char*)"file_seek invalid handle");
    return fseek(it->second, pos, flag);
}

long filelength(int handle)
{
    auto it = file_handles.find(handle);
    if (it == file_handles.end()) Quit((char*)"filelength invalid handle");
    int prev = ftell(it->second);
    fseek(it->second, 0, SEEK_END);
    int len = ftell(it->second);
    fseek(it->second, prev, SEEK_SET);
    return len;
}

static std::string toUpper(const std::string& str)
{
    auto ret = str;
    std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
    return ret;
}

static std::string getExtension(const std::string& filename)
{
    auto pos = filename.find_last_of('.');
    if (pos == std::string::npos) return "";
    return toUpper(filename.substr(pos + 1));
}

int findfirst(const char* extension, int* f, int flag)
{
    auto upExt = toUpper(extension);
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(".")) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (!strcmp(ent->d_name, "."))
            {
                continue;
            }
            else if (!strcmp(ent->d_name, ".."))
            {
                continue;
            }

            if (toUpper("*." + getExtension(ent->d_name)) == upExt)
            {
                closedir(dir);
                return 0;
            }
        }
        closedir(dir);
    }

    return -1;
}
