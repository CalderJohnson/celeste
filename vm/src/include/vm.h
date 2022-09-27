#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stdbool.h>

/* intial conditions */
#define MEMORY_MAX     (1 << 16)                                    //65536 addresses
#define DEVICE_OFFSET  0xFF00                                       //memory mapped IO
#define REGISTER_COUNT 0x10                                         //16 accessible registers
#define CLOCK_SPEED    1000000                                      //1 mhz clock speed
#define FLAGS_INIT     FL_ZERO | FL_PARITY | FL_INTERRUPT           //zero and parity enabled , interrupt and trap routines enabled

typedef struct { 
    uint32_t* memory;
    uint32_t* registers;
    bool halted;
} VM;

extern VM celeste;
extern uint32_t memory[MEMORY_MAX];
extern uint32_t registers[REGISTER_COUNT];

/* registers */
enum {
    R_AX    = 0x0, //accumulator
    R_BX    = 0x1, //base
    R_CX    = 0x2, //counter
    R_DX    = 0x3, //data
    R_SI    = 0x4, //source index
    R_DI    = 0x5, //destination index
    R_SP    = 0x6, //stack pointer
    R_BP    = 0x7, //stack base pointer
    R_IP    = 0x8, //program counter
    R_FLAGS = 0x9, //flags
    R_R0    = 0xA, //6 gprs
    R_R1    = 0xB,
    R_R2    = 0xC,
    R_R3    = 0xD,
    R_R4    = 0xE,
    R_R5    = 0xF,
};

/* flags */
enum {
    FL_ZERO      = 1 << 0, //set if the most recent operation is zero
    FL_SIGN      = 1 << 1, //set if most recent operation is negative (can double as overflow flag)
    FL_CARRY     = 1 << 2, //set if the most recent operation overflows the 32 bit unsigned integer limit
    FL_GREATER   = 1 << 3, //set if in most recent operation, A > B
    FL_PARITY    = 1 << 4, //set if most recent operation is even
    FL_DIRECTION = 1 << 5, //direction strings are processed
    FL_INTERRUPT = 1 << 6, //interrupts enabled/disabled
    FL_TRAP      = 1 << 7, //single step mode
};

/* ports */
enum {
    PORT_TIMER    = 0x0 + DEVICE_OFFSET,
    PORT_KEYBOARD = 0x1 + DEVICE_OFFSET,
    PORT_SCREEN   = 0x2 + DEVICE_OFFSET,
    PORT_DISK     = 0x3 + DEVICE_OFFSET,
    PORT_DEBUG    = 0x4 + DEVICE_OFFSET,
};

#endif