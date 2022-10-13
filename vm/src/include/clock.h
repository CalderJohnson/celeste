#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <stdbool.h>

void init_clock();
bool next_clock();
uint64_t get_timer();

#endif
