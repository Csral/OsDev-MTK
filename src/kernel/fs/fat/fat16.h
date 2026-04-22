#ifndef _FS_FAT16_H_
#define _FS_FAT16_H_

#include "status.h"
#include "string/string.h"
#include "../../includes/fs/file.h"
#include "disk/streamer.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "disk.h"
#include <stdint.h>

#define BasicOS_FAT16_signature 0x29
#define BasicOS_FAT16_FAT_ENTRY_SIZE 0x02
#define BasicOS_FAT16_BAD_SECTOR 0xFF7
#define BasicOS_FAT16_UNUSED 0x00

typedef unsigned int fat_item_t;
#define FAT_ITEM_TYPE_DIRECTORY     0U
#define FAT_ITEM_TYPE_FILE          1U

// Bit masks for FAT
#define FAT_MASKS_FILE_READ_ONLY        0x01U
#define FAT_MASKS_FILE_HIDDEN           0x02U
#define FAT_MASKS_FILE_SYSTEM           0x04U
#define FAT_MASKS_FILE_VOLUME_LABEL     0x08U
#define FAT_MASKS_FILE_SUBDIRECTORY     0x10U
#define FAT_MASKS_FILE_ARCHIVED         0x20U
#define FAT_MASKS_FILE_DEVICE           0x40U
#define FAT_MASKS_FILE_RESERVED         0x80U

struct fat_header_extended  {
    uint8_t drive_number;
    uint8_t win_nt_bit;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_id_string[11];
    uint8_t system_id_string[8];
} __attribute__((packed));

struct fat_header {

    uint8_t short_jmp_nop[3];
    uint8_t OEMIdentifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t num_reserved_sectors;
    uint8_t num_fat_copies;
    uint16_t root_directory_entries;
    uint16_t num_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t num_hidden_sectors;
    uint32_t num_sectors_large;

} __attribute__((packed));

struct fat_h {
    struct fat_header primary_header;
    union fat_he {
        struct fat_header_extended extended_header;
    } shared;
};

struct fat_directory_item {
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_sec; // tenths of a sec
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access;
    uint16_t high_16_bits_first_cluster;
    uint16_t last_modification_time;
    uint16_t last_modification_date;
    uint16_t low_16_bits_first_cluster;
    uint32_t file_size;
} __attribute__((packed));

struct fat_directory {
    struct fat_directory_item* item;
    unsigned long total;
    unsigned long sector_pos;           // first sector of this directory
    unsigned long ending_sector_pos;    // last sector of this directory
};

struct fat_item {

    union {
        struct fat_directory_item* item;        // file
        struct fat_directory_item* directory;   // directory
    };

    fat_item_t type;

};

struct fat_item_descriptor {
    struct fat_item* item;
    unsigned long pos;
};

struct fat_private {
    struct fat_h header;
    struct fat_directory root_directory;

    // Disk stream
    struct disk_stream* cluster_read_stream;
    // FAT streamer
    struct disk_stream* fat_read_stream;
    // Directory streamer
    struct disk_stream* directory_stream;
};

unsigned char fat16_resolve(struct disk* disk);
void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode);

struct filesystem* fat16_init();

#endif