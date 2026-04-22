#include "disk/streamer.h"
#include "memory/heap/kheap.h"
#include "config.h"

struct disk_stream* diskstreamer_new(int d_id) {
    struct disk* disk = disk_get(d_id);

    if (!disk) return 0;

    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;

    return streamer;

}

int diskstreamer_seek(struct disk_stream* streamer, int pos) {

    streamer->pos = pos;
    return 0;

}

int diskstreamer_read(struct disk_stream* streamer, void* out, int total) {

    char buf[_DISK_SECTOR_SIZE];
    int res = 0;

    while (total) {

        int sector_no = streamer->pos / _DISK_SECTOR_SIZE;
        int offset = streamer->pos % _DISK_SECTOR_SIZE;

        res = disk_read_block(streamer->disk, sector_no, 1, (void*) buf);
        if (res < 0) goto out;

        int total_to_read = (total > _DISK_SECTOR_SIZE) ? _DISK_SECTOR_SIZE : total;
        for (unsigned long i = 0; i< total_to_read; i++)
            *(char*) out++ = buf[offset + i];

        // Adjust the stream
        streamer->pos += total_to_read;
        total -= total_to_read;

    }

    out: return res;

}

void diskstreamer_close(struct disk_stream* stream) {
    kfree(stream);
}