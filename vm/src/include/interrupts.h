#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>
#include <vm.h>

/* interrupt codes */
enum {
    INT_TIMER    = 0x0, //hardware interrupt: timer callback
    INT_KEYBOARD = 0x1, //hardware interrupt: keyboard callback 
    INT_SCREEN   = 0x2, //software interrupt: interface with screen
    INT_DISK     = 0x3, //software interrupt: interface with disk
    INT_DEBUG    = 0x4, //software interrupt: interface with serial debug port
};

/* trap codes */
enum {
    TRAP_DIV0    = 0x0, //exception: division by zero
    TRAP_INVOP   = 0x1, //exception: invalid opcode
    TRAP_IOERROR = 0x2, //exception: IO error
};

void call_interrupt (uint8_t intcode);
void call_trap (uint8_t trapcode);

#endif