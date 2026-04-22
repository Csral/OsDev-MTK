#include "fs/file.h"
#include "config.h"
#include "kernel.h"
#include "fat/fat16.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "status.h"

struct filesystem* filesystems[MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[MAX_FILE_DESCRIPTORS_ACTIVE];

static struct filesystem** fs_get_free_filesystem() {

    int i = 0;
    for (; i < MAX_FILESYSTEMS; i++)
        if (filesystems[i] == 0)
            return &filesystems[i];

    return 0;

}

void fs_insert_filesystem(struct filesystem* filesystem) {
    struct filesystem** fs;

    if (!filesystem) kernel_panic("Insert filesystem: Filesystem is provided as NULL.\n");
    fs = fs_get_free_filesystem();
    if (!fs) kernel_panic("Insert filesystem: No free filesystems available - failure to insert.");

    *fs = filesystem;
}

static void fs_static_load() {
    // filesystems build into core-kernel.
    fs_insert_filesystem(fat16_init());
}

void fs_load() {
    // load all fs available
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init() {
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static int file_new_descriptor(struct file_descriptor** desc_out) {
    int res = -ENOMEM;
    for (unsigned int i = 0; i < MAX_FILE_DESCRIPTORS_ACTIVE; i++) {
        if (file_descriptors[i] == 0) {
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));

            // Descriptors index starts at 1.
            desc->id = i + 1;
            file_descriptors[i] = desc;

            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static struct file_descriptor* file_get_fd(int fd) {

    //? What if a previous descriptor is gone? - This won't work. So I'll fix this later.

    if (fd < 1 || fd > MAX_FILE_DESCRIPTORS_ACTIVE) return 0;
    
    int index = fd - 1;
    return file_descriptors[index];

}

struct filesystem* fs_resolve(struct disk* disk) {

    struct filesystem* fs = 0;

    for (unsigned int i = 0; i < MAX_FILESYSTEMS; i++) {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0) {
            fs = filesystems[i];
            break;
        }
    }

    return fs;


}

int fopen(const char* filename, const char* mode) {
    return -EIO;
}