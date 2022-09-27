#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <vm.h>

/* instructions */
enum {
    OP_NOP  = 0x0,  //no operation
    OP_HLT  = 0x1,  //halt
    OP_INT  = 0x2,  //call to interrupt
    OP_LEA  = 0x3,  //load effective address
    OP_MOV  = 0x4,  //move
    OP_PUSH = 0x5,  //push
    OP_POP  = 0x6,  //pop
    OP_JMP  = 0x7,  //unconditional jump
    OP_CMP  = 0x8,  //compare
    OP_JZ   = 0x9,  //jump if zero
    OP_JC   = 0xA,  //jump if carry
    OP_JS   = 0xB,  //jump if sign
    OP_JP   = 0xC,  //jump if parity
    OP_JE   = 0xD,  //jump if equal
    OP_JNE  = 0xE,  //jump if unequal
    OP_JG   = 0xF,  //jump if greater than
    OP_JGE  = 0x10, //jump if greater than or equal to
    OP_JL   = 0x11, //jump if less than
    OP_JLE  = 0x12, //jump if less than or equal to
    OP_CALL = 0x13, //call to function
    OP_RET  = 0x14, //return
    OP_INC  = 0x15, //increment
    OP_DEC  = 0x16, //decrement
    OP_ADD  = 0x17, //add
    OP_SUB  = 0x18, //subtract
    OP_MUL  = 0x19, //multiply
    OP_DIV  = 0x1A, //divide
    OP_AND  = 0x1B, //and
    OP_OR   = 0x1C, //or
    OP_XOR  = 0x1D, //xor
    OP_NOT  = 0x1E, //invert
    OP_SHL  = 0x1F, //shift left
    OP_SHR  = 0x20, //shift right
    OP_ROL  = 0x21, //rotate left
    OP_ROR  = 0x22, //rotate right
    OP_IN   = 0x23, //IO word in
    OP_OUT  = 0x24, //IO word out
};

void run(char* diskfile);

#endif