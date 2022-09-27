#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <disk.h>
#include <vm.h>

char diskpath[50] = "../bin/";
FILE* diskfile = NULL;

/* will be replaced by proper firmware */
void load_program() {
    for (int i = 0; ; i++) {
        fseek(diskfile, i * 4, SEEK_SET);
        fread(&memory[i], sizeof(uint32_t), 1, diskfile);
        if (memory[i] == 1 << 0x18) break; //hlt
    }
}

/* starts the disk */
bool init_disk (const char* diskname) {
    diskfile = fopen(strcat(diskpath, diskname), "rb+");
    if (diskfile == NULL) return false;
    return true;
}

/* write a word (PORT_DISK) at offset */
bool write_disk (const uint32_t offset) {
    if (diskfile == NULL) return false; //handler will throw IO error
    fseek(diskfile, offset, SEEK_SET);
    fwrite(&memory[PORT_DISK], 4, 1, diskfile);
    return true;
}

/* read a word (PORT_DISK) at offset */
bool read_disk (const uint32_t offset) {
    if (diskfile == NULL) return false; //handler will throw IO error
    fseek(diskfile, offset, SEEK_SET);
    fread(&memory[PORT_DISK], 4, 1, diskfile);
    return true;
}