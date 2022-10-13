#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <vm.h>
#include <interrupts.h>
#include <disk.h>
#include <screen.h>
#include <clock.h>

/* timer interrupt */
static void isr_timer () {
    memory[PORT_TIMER] = get_timer();
}

/* keyboard callback */
static void isr_keyboard () {
    return;
}

/* write to display */
static void isr_screen () {
    uint8_t position_x = (registers[R_AX] & 0xFF);        //x position goes in ax
    uint8_t position_y = (registers[R_BX] & 0xFF);        //y position goes in bx
    uint8_t r = (memory[PORT_SCREEN] & 0xFF0000) >> 0x10; //red
    uint8_t g = (memory[PORT_SCREEN] & 0xFF00)   >> 0x8;  //green
    uint8_t b = (memory[PORT_SCREEN] & 0xFF);             //blue
    uint8_t mode = registers[R_DX];
    if (mode == 0) { //draw
        draw(position_x, position_y, r, g, b);
    }
    else if (mode == 1) { //update
        update();
    }
    else if (mode == 2) { //clear
        clear();
    }
    else {
        call_trap(TRAP_IOERROR);
    }
    return;
}

/* read from/write to disk */
static void isr_disk () {
    uint32_t offset = registers[R_AX]; //offset goes in the accumulator register
    bool mode       = registers[R_DX]; //mode goes in the data register
    if (mode) {                        //write
        if(!write_disk (offset))       //write data in PORT_DISK
            call_trap(TRAP_IOERROR);
    }
    else {                             //read
        if(!read_disk (offset))        //read data to PORT_DISK
            call_trap(TRAP_IOERROR);
    }
    return;
}

/* write to the serial debugging port */
static void isr_debug () {
    printf("%c", memory[PORT_DEBUG]);
}

/* unimplemented intcode */
static void isr_stub () {
    return;
}

/* service an interrupt */
void call_interrupt (uint8_t intcode) {
    if (!(registers[R_FLAGS] & FL_INTERRUPT)) { //if interrupt flag is not set
        return;
    }
    registers[R_FLAGS] &= ~FL_INTERRUPT; //clear interrupt flag
    switch (intcode) {
        case INT_TIMER:
            isr_timer();
            break;
        case INT_KEYBOARD:
            isr_keyboard();
            break;
        case INT_SCREEN:
            isr_screen();
            break;
        case INT_DISK:
            isr_disk();
            break;
        case INT_DEBUG:
            isr_debug();
            break;
        default:
            isr_stub();
    }
    registers[R_FLAGS] |= FL_INTERRUPT; //set interrupt flag
    return;
}

/* divide by zero exception handler */
static void trap_divbyzero () {
    celeste.halted = true;
    char message[] = "Exception thrown: division by zero\n";
    for (int i = 0; i < 36; i++) {
        memory[PORT_DEBUG] = message[i];
        isr_debug();
    }
    return;
}

/* invalid opcode handler */
static void trap_invalidopcode () {
    celeste.halted = true;
    char message[] = "Exception thrown: invalid instruction\n";
    for (int i = 0; i < 39; i++) {
        memory[PORT_DEBUG] = message[i];
        isr_debug();
    }
    return;
}

/* IO error handler */
static void trap_ioerror () {
    celeste.halted = true;
    char message[] = "Exception thrown: IO error\n";
    for (int i = 0; i < 28; i++) {
        memory[PORT_DEBUG] = message[i];
        isr_debug();
    }
    return;
}

/* unimplemented trapcode */
static void trap_stub () {
    celeste.halted = true;
    char message[] = "Exception thrown: unknown exception\n";
    for (int i = 0; i < 37; i++) {
        memory[PORT_DEBUG] = message[i];
        isr_debug();
    }
    return;
}

/* service a trap */
void call_trap (uint8_t trapcode) {
    switch (trapcode) {
        case TRAP_DIV0:
            trap_divbyzero();
            break;
        case TRAP_INVOP:
            trap_invalidopcode();
            break;
        case TRAP_IOERROR:
            trap_ioerror();
            break;
        default:
            trap_stub();
    }
    return;
}
