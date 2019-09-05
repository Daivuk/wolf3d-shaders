#include "FILEIO.H"
#include <string>
#include <map>

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

void file_read(int handle, void* out, size_t size)
{
    auto it = file_handles.find(handle);
    if (it == file_handles.end()) return;
    fread(out, size, 1, it->second);
}

void file_write(int handle, const void* in, size_t size)
{
    auto it = file_handles.find(handle);
    if (it == file_handles.end()) return;
    fwrite(in, size, 1, it->second);
}

void file_close(int handle)
{
    auto it = file_handles.find(handle);
    if (it == file_handles.end()) return;
    fclose(it->second);
    file_handles.erase(it);
}
