#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <stdbool.h>

void init_clock();
bool next_clock(double clocks_per_second);
uint64_t get_timer();

#endif
