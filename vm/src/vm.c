#include <stdint.h>
#include <stdbool.h>
#include <vm.h>

uint32_t memory[MEMORY_MAX];        //256 kb memory
uint32_t registers[REGISTER_COUNT]; //16 registers

VM celeste = {
    .memory = &memory,
    .registers = &registers,
    .halted = false
};