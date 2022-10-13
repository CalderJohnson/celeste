#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <clock.h>

static uint64_t sysclock; //processor clock
static uint64_t base;     //time when turned on

/* get time in microseconds */
static inline uint64_t current_time () {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000000 + t.tv_usec;
}

/* reset clock cycle */
static inline void restart_clock() {
    sysclock = current_time();
}

/* initialize both timers */
void init_clock () {
    base = current_time();
    sysclock = base;
}

/* time for the next clock cycle? */
bool next_clock (double clocks_per_second) {
    double clocks_per_microsecond = clocks_per_second / 1000000;
    if (current_time() - sysclock < clocks_per_microsecond) {
        return false;
    }
    else {
        restart_clock();
        return true;
    }
}

/* return time since program begun (ms) */
uint64_t get_timer () {
    return (sysclock - base) / 1000;
}
