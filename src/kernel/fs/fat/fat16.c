#include "fat16.h"

unsigned char fat16_resolve(struct disk* disk);
void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode);
int fat16_read(struct disk* disk, void* desc, size_t size, uint32_t nmemb, char* out);
int fat16_seek(void* private, size_t offset, file_seek_mode seek_mode);
int fat16_stat(struct disk* disk, void* private, struct file_stat* stat);
int fat16_close(void* private);

struct filesystem fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open,
    .read = fat16_read,
    .seek = fat16_seek,
    .stat = fat16_stat,
    .close = fat16_close
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

void fat16_to_string(char** out, const char* in) {
    while (*in != 0x00 && *in != 0x20) {
        **out = *in;
        *out += 1;
        in += 1;
    }

    if (*in == 0x20) **out = 0x00;
}

void fat16_get_full_relative_filename(struct fat_directory_item* item, char* out, unsigned long size) {
    memset(out, 0x00, size);
    char* out_tmp = out;
    fat16_to_string(&out_tmp, (const char*) item->filename);

    if (item->ext[0] != 0x00 && item->ext[0] != 0x20) {
        *out_tmp++ = '.';
        fat16_to_string(&out_tmp, (const char*) item->ext);
    }
}

struct fat_directory_item* fat16_clone_directory_item(struct fat_directory_item* item, unsigned long size) {

    if ((long) size < sizeof(struct fat_directory_item)) return 0;

    struct fat_directory_item* item_copy = kzalloc(size);
    if (!item_copy) return 0;

    memcpy(item_copy, item, size);
    return item_copy;

}

static inline __attribute__((__always_inline__)) unsigned long fat16_get_first_cluster(struct fat_directory_item* item) {
    return (item->high_16_bits_first_cluster) | (item->low_16_bits_first_cluster);
}

static inline __attribute__((__always_inline__)) unsigned long fat16_cluster_to_sector(struct fat_private* private, unsigned long cluster) {
    return private->root_directory.ending_sector_pos + (unsigned long) (( (long) cluster - 2) * private->header.primary_header.sectors_per_cluster);
}

static inline __attribute__((__always_inline__)) unsigned long fat16_get_first_fat_sector(struct fat_private* private) {
    return private->header.primary_header.num_reserved_sectors;
}

static int fat16_get_fat_entry(struct disk* disk, unsigned long cluster) {

    int res = -1;
    struct fat_private* private = disk->fs_private;
    struct disk_stream* stream = private->fat_read_stream;
    if (!stream) goto out;

    unsigned long fat_table_position = fat16_get_first_fat_sector(private) * disk->sector_size;
    res = diskstreamer_seek(stream, fat_table_position * (cluster * BasicOS_FAT16_FAT_ENTRY_SIZE));

    if (res < 0) goto out;

    unsigned short result = 0;
    res = diskstreamer_read(stream, &result, sizeof(result));
    if (res < 0) goto out;
    res = result;

    out: return res;

}

static int fat16_get_cluster_for_offset(struct disk* disk, unsigned long starting_cluster, long offset) {

    /*
        Get the correct cluster to use based on starting cluster and offset
    */

    int res = 0;
    struct fat_private* private = disk->fs_private;
    unsigned long size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    unsigned long cluster_to_use = starting_cluster;
    unsigned long cluster_ahead = offset / size_of_cluster_bytes;

    for (unsigned long i = 0; i < cluster_ahead; i++) {
        int entry = fat16_get_fat_entry(disk, cluster_to_use);

        if (entry == 0xFF8 || entry == 0xFFF) {
            // We are at the end/last entry of file/directory
            res = -EIO;
            goto out;
        }

        if (entry == BasicOS_FAT16_BAD_SECTOR) {
            // Is the sector bad?
            res = -EIO;
            goto out;
        } else if (entry >= 0xFF0 || entry <= 0xFF6) {
            // Reserved sector
            res = -EIO;
            goto out;
        } else if (entry == 0x00) {
            // whats with our entry?
            res = -EIO;
            goto out;
        }

        cluster_to_use = entry;

    }

    res = cluster_to_use;

    out: return res;

}

static int fat16_read_internal_from_stream(struct disk* disk, struct disk_stream* stream, unsigned long cluster, long offset, unsigned long total, void* out) {

    int res = 0;

    struct fat_private* private = disk->fs_private;
    unsigned long size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    
    while (total > 0) {

        int cluster_to_use = fat16_get_cluster_for_offset(disk, cluster, offset);
        if (cluster_to_use < 0) {
            res = cluster_to_use;
            goto out;
        }

        long offset_from_cluster = offset % size_of_cluster_bytes;
        unsigned long starting_sector = fat16_cluster_to_sector(private, cluster_to_use);
        unsigned long starting_pos = (starting_sector * disk->sector_size) + offset_from_cluster;
        unsigned long total_to_read = (total > size_of_cluster_bytes) ? size_of_cluster_bytes : total;

        res = diskstreamer_seek(stream, starting_pos);
        if (res != NE) goto out;
        
        res = diskstreamer_read(stream, out, total_to_read);
        if (res != NE) goto out;

        total -= total_to_read;
        offset += total_to_read;
        out += total_to_read;

    }

    out:
    return res;

}

static int fat16_read_internal(struct disk* disk, unsigned long starting_cluster, long offset, unsigned long total, void* out) {
    struct fat_private* fs_private = disk->fs_private;
    struct disk_stream* stream = fs_private->cluster_read_stream;
    return fat16_read_internal_from_stream(disk, stream, starting_cluster, offset, total, out);
}

void fat16_free_directory(struct fat_directory* directory) {
    if (!directory) return;
    if (directory->item) kfree(directory->item);
    kfree(directory);
}

void fat16_fat_item_free(struct fat_item* item) {
    if (item->type == FAT_ITEM_TYPE_DIRECTORY) {
        fat16_free_directory(item->directory);
    } else if (item->type == FAT_ITEM_TYPE_FILE) {
        kfree(item->item);
    } else {
        kfree(item);
        kernel_panic("[FAT16 driver] Free: Item being fred is of invalid type!\n");
    }
}

struct fat_directory* fat16_load_fat_directory(struct disk* disk, struct fat_directory_item* item) {

    int res = 0;

    struct fat_directory* directory = 0;
    struct fat_private* private = disk->fs_private;

    if (!(item->attributes & FAT_MASKS_FILE_SUBDIRECTORY)) {
        res = -EINVARG;
        goto out;
    }

    directory = kzalloc(sizeof(struct fat_directory));
    if (!directory) {
        res = -ENOMEM;
        goto out;
    }

    unsigned long cluster = fat16_get_first_cluster(item);
    unsigned long cluster_sector = fat16_cluster_to_sector(private, cluster);
    unsigned long total_items = fat16_get_total_items_for_directory(disk, cluster_sector);

    directory->total = total_items;
    unsigned long directory_size = directory->total * sizeof(struct fat_directory_item);
    directory->item = kzalloc(directory_size);

    if (!directory->item) {
        res = -ENOMEM;
        goto out;
    }

    res = fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item);
    if (res != NE) goto out;

    out:

    if (res != NE) fat16_free_directory(directory);
    return directory;

}

struct fat_item* fat16_new_fat_item_for_directory_item(struct disk* disk, struct fat_directory_item* item) {

    struct fat_item* f_item = kzalloc(sizeof(struct fat_item));
    if (!f_item) return 0;

    if (item->attributes & FAT_MASKS_FILE_SUBDIRECTORY) {
        f_item->directory = fat16_load_fat_directory(disk, item);
        f_item->type = FAT_ITEM_TYPE_DIRECTORY;
    }
    
    f_item->type = FAT_ITEM_TYPE_FILE;
    f_item->item = fat16_clone_directory_item(item, sizeof(struct fat_directory_item));

    return f_item;

}

struct fat_item* fat16_find_item_in_directory(struct disk* disk, struct fat_directory* directory, const char* path) {

    struct fat_item* f_item = 0;
    char tmp_filename[_FS_MAX_PATH_LEN];

    for (unsigned long i = 0; i < directory->total; i++) {
        fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));
        if (istrncmp(tmp_filename, path, sizeof(tmp_filename)) == 0) {
            f_item = fat16_new_fat_item_for_directory_item(disk, &directory->item[i]);
        }
    }

    return f_item;

}

struct fat_item* fat16_get_directory_entry(struct disk* disk, struct path_part* path) {

    struct fat_private* private = disk->fs_private;
    struct fat_item* current_item = 0;
    struct fat_item* root_item = fat16_find_item_in_directory(disk, &private->root_directory, path->part);

    if (!root_item) goto out;

    struct path_part* next_part = path->next;
    current_item = root_item;

    while (next_part != 0) {
        if (current_item->type != FAT_ITEM_TYPE_DIRECTORY) {
            current_item = 0;
            break;
        }

        struct fat_item* tmp_item = fat16_find_item_in_directory(disk, current_item->directory, next_part->part);
        fat16_fat_item_free(current_item);
        current_item = tmp_item;
        next_part = next_part->next;
    }

    out:
    return current_item;

}

void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode) {

    if (mode != FILE_MODE_READ) return ERROR(-ERDONLY);

    struct fat_file_descriptor* descriptor = 0;
    descriptor = kzalloc(sizeof(struct fat_file_descriptor));

    if (!descriptor)
        return ERROR(-ENOMEM);

    descriptor->item = fat16_get_directory_entry(disk, path);
    if (!descriptor->item) return ERROR(-EIO);
    descriptor->pos = 0;

    return descriptor;
}

static void fat16_free_file_descriptor(struct fat_file_descriptor* desc) {
    fat16_fat_item_free(desc->item);
    kfree(desc);
}

int fat16_close(void* private) {
    fat16_free_file_descriptor((struct fat_file_descriptor*) private);

    out:
    return 0;
}

int fat16_stat(struct disk* disk, void* private, struct file_stat* stat) {

    int res = 0;
    struct fat_file_descriptor* desc = (struct fat_file_descriptor*) private;
    struct fat_item* desc_item = desc->item;

    if (desc->item->type != FAT_ITEM_TYPE_FILE) {
        res = -EINVARG;
        goto out;
    }

    struct fat_directory_item* ritem = desc_item->item;
    stat->file_size = ritem->file_size;
    stat->flags = 0x00U;

    if (ritem->attributes & FAT_MASKS_FILE_READ_ONLY) stat->flags |= FILE_STAT_RO;

    out:
    return res;

}

int fat16_read(struct disk* disk, void* desc, size_t size, uint32_t nmemb, char* out) {
    int res = 0;

    struct fat_file_descriptor* fat_desc = desc;
    struct fat_directory_item* item = fat_desc->item->item;
    int offset = fat_desc->pos;

    for (unsigned long i = 0; i < nmemb; i++) {
        res = fat16_read_internal(disk, fat16_get_first_cluster(item), offset, size, out);
        if (IS_ERR(res)) goto out;

        out += size;
        offset += size;

    }

    res = nmemb;

    out:
    return res;
}

int fat16_seek(void* private, size_t offset, file_seek_mode seek_mode) {

    int res = 0;
    struct fat_file_descriptor* desc = private;
    struct fat_item* desc_item = desc->item;

    if (desc->item->type != FAT_ITEM_TYPE_FILE) {
        res = -EINVARG;
        goto out;
    }

    struct fat_directory_item* ritem = desc_item->item;
    if (offset >= ritem->file_size) {
        res = -EIO;
        goto out;
    }

    switch (seek_mode) {
        case SEEK_SET:
            desc->pos = offset;
            break;
        case SEEK_END:
            res = -EUNIMP;
            break;
        case SEEK_CUR:
            desc->pos += offset;
            break;

        default:
            res = -EINVARG;
            break;
    }

    out:
    return res;

}