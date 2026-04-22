#include "fat16.h"

struct filesystem fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open
};

struct filesystem* fat16_init() {
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

static void fat16_init_private(struct disk* disk, struct fat_private* private) {
    memset(private, 0, sizeof(struct fat_private));

    private->cluster_read_stream = diskstreamer_new(disk->id);
    private->fat_read_stream = diskstreamer_new(disk->id);
    private->directory_stream = diskstreamer_new(disk->id);
}

inline __attribute__((__always_inline__)) int fat16_sector_to_absolute(struct disk* disk, unsigned long sector) {
    return (sector * disk->sector_size);
}

int fat16_get_total_items_for_directory(struct disk* disk, uint32_t directory_start_sector) {
    struct fat_directory_item item;
    struct fat_directory_item empty_item;
    memset(&empty_item, 0, sizeof(empty_item));

    struct fat_private* private = disk->fs_private;
    int res = 0;
    int i = 0;
    int directory_start_pos = fat16_sector_to_absolute(disk, directory_start_sector);
    struct disk_stream* stream = private->directory_stream;

    if (diskstreamer_seek(stream, directory_start_pos) != NE) {
        res = -EIO;
        goto out;
    };

    while (1) {
        if (diskstreamer_read(stream, &item, sizeof(item)) != NE) {
            res = -EIO;
            goto out;
        }

        if (item.filename[0] == 0x00U) break; // Item is empty/blank record
        if (item.filename[0] == 0xE5U) continue; // Item is unused

        i++;
    }

    res = i;

    out:
    return res;

}

int fat16_get_root_directory(struct disk* disk, struct fat_private* private, struct fat_directory* out_directory) {
    
    int res = 0;
    struct fat_header* primary_header = &private->header.primary_header;
    int root_dir_sector_pos = (primary_header->num_fat_copies * primary_header->sectors_per_fat) + primary_header->num_reserved_sectors;
    int root_dir_entries = private->header.primary_header.root_directory_entries;
    int root_dir_size = (root_dir_entries * sizeof(struct fat_directory_item));

    int total_sectors = root_dir_size / disk->sector_size;
    if (root_dir_size % disk->sector_size)
        total_sectors += 1;

    int total_items = fat16_get_total_items_for_directory(disk, root_dir_sector_pos);

    struct fat_directory_item* d_item = kzalloc(root_dir_size);
    if (!d_item) {
        res = -ENOMEM;
        goto out;
    }

    struct disk_stream* stream = private->directory_stream;
    if (diskstreamer_seek(stream, fat16_sector_to_absolute(disk, root_dir_sector_pos)) != NE) {
        res = -EIO;
        goto out;
    }

    // Read the root directory now!
    if (diskstreamer_read(stream, d_item, root_dir_size) != NE) {
        res = -EIO;
        goto out;
    }

    out_directory->item = d_item;
    out_directory->total = total_items;
    out_directory->sector_pos = root_dir_sector_pos;
    out_directory->ending_sector_pos = root_dir_sector_pos + (root_dir_size / disk->sector_size);

    out:
    return res;
}

unsigned char fat16_resolve(struct disk* disk) {

    int res = 0;
    
    struct fat_private* fat_priv = kzalloc(sizeof(struct fat_private));
    fat16_init_private(disk, fat_priv);

    disk->fs_private = fat_priv;
    disk->filesystem = &fat16_fs;

    struct disk_stream* stream = diskstreamer_new(disk->id);
    if (!stream) {
        res = -ENOMEM;
        goto out;
    }

    if (diskstreamer_read(stream, &fat_priv->header, sizeof(fat_priv->header)) != NE) {
        res = -EIO;
        goto out;
    }

    if (fat_priv->header.shared.extended_header.signature != 0x29U) {
        res = -EFSNOTSUPPORTED;
        goto out;
    }

    if (fat16_get_root_directory(disk, fat_priv, &fat_priv->root_directory) != NE) {
        res = -EIO;
        goto out;
    }

    out:

    if (stream)
        diskstreamer_close(stream);

    if (res < 0) {
        kfree(fat_priv);
        disk->fs_private = 0;
    }

    return res;

}

void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode) {
    return 0;
}