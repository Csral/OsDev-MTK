#ifndef __DISK_H__
#define __DISK_H__

typedef unsigned int disk_t; // disk type: Real disk, VFS etc

// a real physical hard disk
#define DISK_TYPE_REAL 0

struct disk {
    disk_t type;
    unsigned long sector_size;
};

void disk_search_and_init(void);
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, unsigned long total, void* buffer);

#endif