#include "fs/file.h"
#include "config.h"
#include "kernel.h"
#include "fat/fat16.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "status.h"
#include "string/string.h"
#include "disk.h"

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

static inline void file_free_descriptor(struct file_descriptor* desc) {
    file_descriptors[desc->id - 1]= 0x00U;
    kfree(desc);
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

FILE_MODE file_get_mode_by_str(const char* str) {

    FILE_MODE mode = FILE_MODE_INVALID;

    if (strncmp(str, "r", 1) == 0) mode = FILE_MODE_READ;
    else if (strncmp(str, "w", 1) == 0) mode = FILE_MODE_WRITE;
    else if (strncmp(str, "a", 1) == 0) mode = FILE_MODE_APPEND;

    return mode;

}

unsigned long fopen(const char* filename, const char* mode_str) {

    int res = 0;

    struct path_root* root_path = pparser_parse(filename, NULL);
    if (!root_path) {
        res = -EINVARG;
        goto out;
    }

    if (!root_path->first) {
        // dont open "0:/"
        res = -EINVARG;
        goto out;
    }

    struct disk* disk = disk_get(root_path->drive_number);
    if (!disk) {
        res = -EIO;
        goto out;
    }

    if (!disk->filesystem) {
        res = -EIO;
        goto out;
    }

    FILE_MODE mode = file_get_mode_by_str(mode_str);
    if (mode == FILE_MODE_INVALID) {
        res = -EINVARG;
        goto out;
    }

    void* descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
    if (IS_ERR(descriptor_private_data)) {
        res = ERROR_I(descriptor_private_data);
        goto out;
    }

    struct file_descriptor* desc = 0;
    res = file_new_descriptor(&desc);
    if (res < 0) goto out;

    desc->fs = disk->filesystem;
    desc->disk = disk;
    desc->private = descriptor_private_data;
    res = desc->id;

    out:
    // pass null not the err_code
    if (res < 0) res = 0;
    return res;
}

int fstat(int fd, struct file_stat* stat) {
    int res = 0;
    struct file_descriptor* desc = file_get_fd(fd);

    if (!desc) {
        res = -EIO;
        goto out;
    }

    res = desc->fs->stat(desc->disk, desc->private, stat);

    out:
    return res;
}

int fclose(int fd) {
    int res = 0;

    struct file_descriptor* desc = file_get_fd(fd);
    if (!desc) {
        res = -EIO;
        goto out;
    }
    
    res = desc->fs->close(desc->private);
    if (res == NE)
        file_free_descriptor(desc);

    out:
    return res;
}

int fseek(int fd, size_t offset, file_seek_mode whence) {
    int res = 0;
    struct file_descriptor* desc = file_get_fd(fd);

    if (!desc) {
        res = -EIO;
        goto out;
    }

    res = desc->fs->seek(desc->private, offset, whence);

    out:
    return res;

}

int fread(void* ptr, size_t size, uint32_t nmemb, int fd) {

    int res = 0;

    if (size == 0 || nmemb == 0 || fd < 1) {
        res = -EINVARG;
        goto out;
    }

    struct file_descriptor* descriptor = file_get_fd(fd);
    if (!descriptor) {
        res = -EINVARG;
        goto out;
    }

    res = descriptor->fs->read(descriptor->disk, descriptor->private, size, nmemb, (char*) ptr);

    out:
    return res;

}