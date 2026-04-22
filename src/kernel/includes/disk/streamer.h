#ifndef _DISK_STREAMER_H_
#define _DISK_STREAMER_H_

#include "../disk.h"

struct disk_stream {
    unsigned long pos;
    struct disk* disk;
};

struct disk_stream* diskstreamer_new(int d_id);
int diskstreamer_seek(struct disk_stream* streamer, int pos);
int diskstreamer_read(struct disk_stream* streamer, void* out, int total);
void diskstreamer_close(struct disk_stream* stream);

#endif