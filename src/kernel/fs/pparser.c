#include "../includes/fs/pparser.h"
#include "../includes/fs/fs.h"
#include "../includes/kernel.h"
#include "../includes/string/string.h"
#include "../includes/memory/heap/kheap.h"
#include "../includes/memory/memory.h"
#include "../includes/status.h"

static int path_valid_format(const char* filename) {
    int len = strnlen(filename, _FS_MAX_PATH_LEN);
    return (len > 2 && isdigit(filename[0]) && memcmp((void*) &filename[1], ":/", 2) == 0);
}

static int get_drive_by_path(const char** path) {
    if (!path_valid_format(*path))
        return -EBADPATH;

    int drive = atoic(*path[0]);
    // Add 3 bytes so we skip DRIVE_NUM, :, /
    *path += 3;
    return drive;

}

static struct path_root* pparser_create_root(int d_number) {
    struct path_root* path_r = kzalloc(sizeof(struct path_root));
    path_r->drive_number = d_number;
    path_r->first = 0;

    return path_r;
}

static const char* pparser_get_path_part(const char** path) {
    char* result_path_part = kzalloc(_FS_MAX_PATH_LEN);

    int ctr = 0;
    while (**path != '/' && **path != '\0') {
        result_path_part[ctr] = **path;
        *path += 1;
        ctr++;
    }

    if (**path == '/') *path += 1; // avoid the extra '/'

    if (ctr == 0) {
        kfree(result_path_part);
        result_path_part = 0;
    }

    return result_path_part;

}

static struct path_part* pparser_parse_path_part(struct path_part* last_part, const char** path) {
    const char* path_part_str = pparser_get_path_part(path);

    if (!path_part_str) return 0;
    struct path_part* part = kzalloc(sizeof(struct path_part));
    part->part = path_part_str;
    part->next = _FS_NULL_ADDRESS;

    if (last_part)
        last_part->next = part;

    return part;

}

void pparser_free(struct path_root* root) {
    struct path_part* part = root->first;

    while (part) {
        struct path_part* n_part = part->next;
        kfree( (void*) part->part);
        kfree(part);
        part = n_part;
    }

    kfree(root);
}

struct path_root* pparser_parse(const char* path, const char* cwd) {
    int res = 0;
    const char* tmp_path = path;

    struct path_root* path_root = 0;
    if (strlen(path) > _FS_MAX_PATH_LEN) goto out;

    res = get_drive_by_path(&tmp_path);
    if (res < 0) goto out;

    path_root = pparser_create_root(res);
    if (!path_root) goto out;

    struct path_part* first_part = pparser_parse_path_part(_FS_NULL_ADDRESS, &tmp_path);
    if (!first_part) goto out;

    path_root->first = first_part;
    struct path_part* part = pparser_parse_path_part(first_part, &tmp_path);

    while (part)
        part = pparser_parse_path_part(part, &tmp_path);

    out:
    return path_root;

}