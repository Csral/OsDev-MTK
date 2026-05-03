#ifndef _KE_FS_FILE_H_
#define _KE_FS_FILE_H_

#include <stddef.h>
#include <stdint.h>
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

enum {
    FILE_STAT_RO = 0b00000001,
};

typedef unsigned int FILE_STAT_FLAGS;

struct disk; // forward-declaration
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part* path, FILE_MODE mode);
/* Can I process this disk - Returns boolean */
typedef unsigned char (*FS_RESVOLE_FUNCTION)(struct disk* disk);
// Read nmemb blocks * size bytes
typedef int (*FS_READ_FUNCTION) (struct disk* disk, void* private, size_t size, uint32_t nmemb, char* out);
typedef int (*FS_CLOSE_FUNCTION) (void* private);
typedef int (*FS_SEEK_FUNCTION) (void* private, size_t offset, file_seek_mode seek_mode);

struct file_stat {
    FILE_STAT_FLAGS flags;
    uint32_t file_size;
};

typedef int (*FS_STAT_FUNCTION) (struct disk* disk, void* private, struct file_stat* stat);

struct filesystem {
    FS_RESVOLE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;
    FS_READ_FUNCTION read;
    FS_SEEK_FUNCTION seek;
    FS_STAT_FUNCTION stat;
    FS_CLOSE_FUNCTION close;

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
int fclose(int fd);
int fstat(int fd, struct file_stat* stat);
int fseek(int fd, size_t offset, file_seek_mode whence);
int fread(void* ptr, size_t size, uint32_t nmemb, int fd);
unsigned long fopen(const char* filename, const char* mode_str);

void fs_insert_filesystem(struct filesystem* fs);
struct filesystem* fs_resolve(struct disk* disk);

#endif