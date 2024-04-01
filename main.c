#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include "ext2.h"

FILE* file;

int ext2i_read_sector(void* buf, uint32_t lba) {
    fseek(file, lba * EXT2_SECTOR_SIZE, SEEK_SET);
    fread(buf, 1, EXT2_SECTOR_SIZE, file);
}

void dir_print(struct ext2_dirent* entry) {
    if (entry->s_name[0] == '.')
        return;

    struct ext2_inode inode;

    // printf("entry inode=%u\n", entry->s_inode);

    // ext2_get_inode(&inode, entry->s_inode);

    // printf("entry inode=%u\n", entry->s_inode);

    // time_t t = inode.s_creation_time;

    // struct tm* tm = gmtime(&t);

    // const char* fmt = "%a %b %e %H:%M:%S %Y (%e/%m/%Y)";

    // char buf[128];

    // strftime(buf, 8, fmt, tm);

    //printf("%s", buf);

    for (int i = 0; i < entry->s_name_len; i++)
        putchar(entry->s_name[i]);

    if (entry->s_type == DIRENT_DIRECTORY)
        putchar('/');

    putchar(' ');
}

int main(int argc, const char* argv[]) {
    const char** args = malloc(argc * sizeof(const char*));
    const char* dev = NULL;

    for (int i = 0; i < argc; i++)
        args[i] = NULL;

    int j = 0;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-d")) {
            dev = argv[i+1];

            i += 2;
        } else {
            args[j++] = argv[i];
        }
    }

    if (!dev)
        dev = "disk.img";

    file = fopen(dev, "rb");

    if (!file) {
        printf("Could not open \'%s\'\n", dev);

        return 1;
    }

    if (ext2_init()) {
        printf("Couldn't mount \'%s\' as an ext2 filesystem\n", dev);

        return 1;
    }

    struct ext2_inode inode;

    const char* path = args[0] ? args[0] : "/";

    if (ext2_search(&inode, path)) {
        printf("Couldn't find path \'%s\'\n", path);

        return 0;
    }

    if ((inode.s_tp & 0xf000) == INODE_FILE) {
        size_t pos = strlen(path) - 1;

        while (path[pos] != '/' && path[pos] != '\0')
            --pos;

        struct ext2_fd file;

        ext2_fopen(&file, path, "rb");

        printf("file: %s size=%u\n", path, file.inode.s_sizel);

        char* buf = malloc(file.inode.s_sizel);

        ext2_fread(&file, buf, file.inode.s_sizel);

        printf("%s\n", buf);

        ext2_fclose(&file);
    } else {
        ext2_dir_iterate(&inode, dir_print);

        putchar('\n');
    }

    fclose(file);

    return 0;
}