#ifndef _KE_FS_FILE_H_
#define _KE_FS_FILE_H_

#include "pparser.h"

typedef unsigned int file_seek_mode;
enum {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef unsigned int FILE_MODE;
enum {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk; // forward-declaration
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part* path, FILE_MODE mode);
/* Can I process this disk - Returns boolean */
typedef unsigned char (*FS_RESVOLE_FUNCTION)(struct disk* disk);

struct filesystem {
    FS_RESVOLE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;

    char name[20];
};

struct file_descriptor {
    int id;
    struct filesystem* fs;
    // private data for internal fd
    void* private;

    // disk the fd uses.
    struct disk* disk;
};

void fs_init();
int fs_open(const char* filename, const char* mode);
void fs_insert_filesystem(struct filesystem* fs);

struct filesystem* fs_resolve(struct disk* disk);

#endif