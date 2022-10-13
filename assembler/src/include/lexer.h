#ifndef LEXER_H
#define LEXER_H

#define MAX_PROGRAM_LENGTH 1000
#define MAX_LINE_LENGTH 15

#include <stdbool.h>

/* all tokens */
enum {
    /* operation tokens */
    T_NOP    = 0x0,  //no operation operation
    T_HLT    = 0x1,  //halt operation
    T_INT    = 0x2,  //call to interrupt operation
    T_LEA    = 0x3,  //load effective address operation
    T_MOV    = 0x4,  //move operation
    T_PUSH   = 0x5,  //push operation
    T_POP    = 0x6,  //pop operation
    T_JMP    = 0x7,  //unconditional jump operation
    T_CMP    = 0x8,  //compare operation
    T_JZ     = 0x9,  //jump if zero operation
    T_JC     = 0xA,  //jump if carry operation
    T_JS     = 0xB,  //jump if sign operation
    T_JP     = 0xC,  //jump if parity operation
    T_JE     = 0xD,  //jump if equal operation
    T_JNE    = 0xE,  //jump if unequal operation
    T_JG     = 0xF,  //jump if greater than operation
    T_JGE    = 0x10, //jump if greater than or equal to operation
    T_JL     = 0x11, //jump if less than operation
    T_JLE    = 0x12, //jump if less than or equal to operation
    T_CALL   = 0x13, //call to function operation
    T_RET    = 0x14, //return operation
    T_INC    = 0x15, //increment operation
    T_DEC    = 0x16, //decrement operation
    T_ADD    = 0x17, //add operation
    T_SUB    = 0x18, //subtract operation
    T_MUL    = 0x19, //multiply operation
    T_DIV    = 0x1A, //divide operation
    T_AND    = 0x1B, //and operation
    T_OR     = 0x1C, //or operation
    T_XOR    = 0x1D, //xor operation
    T_NOT    = 0x1E, //invert operation
    T_SHL    = 0x1F, //shift left operation
    T_SHR    = 0x20, //shift right operation
    T_ROL    = 0x21, //rotate left operation
    T_ROR    = 0x22, //rotate right operation
    T_IN     = 0x23, //IO word in operation
    T_OUT    = 0x24, //IO word out operation

    /* data tokens */
    T_REG    = 0x25, //register
    T_VAL    = 0x26, //immv/addr
    T_DEREF  = 0x27, //dereference
    T_OFFSET = 0x28, //offset
    T_DW     = 0x29, //define constant
    T_LABEL  = 0x2A, //label

    /* structural tokens */
    T_COMMA  = 0x2B, //comma
    T_SPACE  = 0x2C, //space
    T_NL     = 0x2D, //newline
    T_COLON  = 0x2E, //colon
};

bool tokenize(const char* buffer);

extern int tokenlist[MAX_PROGRAM_LENGTH];
extern unsigned int token_index;

#endif
