#ifndef DISK_H
#define DISK_H

#include <stdint.h>

bool init_disk (const char* diskname);
void load_program(); // will be replaced 
bool write_disk (const uint32_t position);
bool read_disk (const uint32_t position);

#endif