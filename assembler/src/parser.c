#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <lexer.h>
#include <error.h>

/* label resolving */
static unsigned int labels[50];

static bool find_labels(int* program, const unsigned int length) {
    unsigned int current_addr = 0; //keep track of current address
    unsigned int i; //reusable index
    //find all label definitions
    for (i = 0; i < length; i++) {
        if (program[i] == T_NL) current_addr++;
        if (program[i] == T_LABEL && (i + 2) <= length && program[i+2] == T_COLON) labels[program[i+1]] = current_addr; //label definition found
    }

    //replace all label usages with the resolved addr, (label, index) -> (val, value)
    for (i = 0; i < length; i++) {
        if (program[i] == T_LABEL && ((i + 2) >= length || program[i+2] != T_COLON)) {
            program[i] = T_VAL;
            program[i+1] = labels[program[i+1]];
        }
    }
    return true;
}

/* encode one line of tokens */
static bool compile_line(const uint32_t* line, uint32_t* instruction) {
    *instruction = 0; //clear garbage value

    //line length
    unsigned int length;
    for (length = 0; line[length] != T_NL; length++) {} length++;

    //operation tokens are the value of their opcode
    #define ENCODE_OP(opcode) *instruction |= opcode << 0x18
    #define ENCODE_MODE(mode) *instruction |= mode   << 0x14

    //shared encoding method for all binary mathematical/logical operations
    #define BINARY_OP \
                if (length >= 12 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && \
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_REG && line[7] < 16 && \
                    line[8] == T_COMMA && line[9] == T_SPACE && line[10] == T_REG && line[11] < 16) { /* reg1 = reg2 op reg3 */ \
                    ENCODE_MODE(0b0000); /* mode */  \
                    *instruction |= line[3]  << 0x10; /* dest */  \
                    *instruction |= line[7]  << 0xC;  /* src 1 */ \
                    *instruction |= line[11] << 0x8;  /* src 2 */ \
                } \
                else if (length >= 8 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && \
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_REG && line[7] < 16) { /* reg1 = reg1 op reg2 */ \
                    ENCODE_MODE(0b0001); /* mode */ \
                    *instruction |= line[3] << 0x10;  /* dest */ \
                    *instruction |= line[7] << 0xC;   /* src */  \
                } \
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && \
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_DEREF && line[7] == T_VAL && line[8] < 0xFFFF) {  /* reg = reg op addr */ \
                    ENCODE_MODE(0b0010); /* mode */ \
                    *instruction |= line[3] << 0x10;  /* dest */ \
                    *instruction |= line[8];          /* addr */ \
                } \
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && \
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] <= 0xFFFF) { /* reg = reg op immv */ \
                    ENCODE_MODE(0b0010); /* mode */ \
                    *instruction |= line[3] << 0x10;  /* dest */ \
                    *instruction |= line[7];          /* immv */ \
                } \
                else return false;

    //shared encoding method for all unary mathematical/logical operations
    #define UNARY_OP \
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { /* op reg */ \
                    ENCODE_MODE(0b0000); /* mode */ \
                    *instruction |= line[3] << 0x10;  /* reg */  \
                } \
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { /* op reg (addr) */ \
                    ENCODE_MODE(0b0001); /* mode */       \
                    *instruction |= line[4] << 0x10;  /* reg (addr) */ \
                } \
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_VAL && line[4] < 0xFFFF) { \
                    ENCODE_MODE(0b0010); /* mode */ \
                    *instruction |= line[4];          /* addr */ \
                } \
                else return false;

    //shared encoding method for all jump operations
    #define JMP_OP \
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { /* jump to addr in reg */ \
                    ENCODE_MODE(0b0000); /* mode */ \
                    *instruction |= line[3] << 0x10;  /* reg */  \
                } \
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { /* jump to addr in reg (addr) */ \
                    ENCODE_MODE(0b0001); /* mode */       \
                    *instruction |= line[4] << 0x10;  /* reg (addr) */ \
                } \
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { /* jump to addr */ \
                    ENCODE_MODE(0b0010); /* mode */ \
                    *instruction |= line[3];          /* addr */ \
                } \
                else return false;

    //shared encoding method for in/out
    #define IO_OP \
                if (length >= 7 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && \
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] < 256) { \
                    *instruction |= line[3] << 0x10; /* reg */ \
                    *instruction |= line[7] << 0x8; /* port */ \
                } \
                else return false; \


    switch (line[0]) {
            case T_NOP:
                ENCODE_OP(line[0]);
                break;
            case T_HLT:
                ENCODE_OP(line[0]);
                break;
            case T_INT:
                ENCODE_OP(line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 256)
                    *instruction |= line[3] << 8;     //intcode
                else return false;
                break;
            case T_LEA:
                ENCODE_OP(line[0]);
                if (length >= 8 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 &&
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] < 0xFFFF) { //load to reg
                    ENCODE_MODE(0b0000); //mode
                    *instruction |= line[3] << 0x10;  //dest
                    *instruction |= line[7];          //addr
                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 &&
                    line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_VAL && line[8] < 0xFFFF) { //load to reg (addr)
                    ENCODE_MODE(0b0001); //mode
                    *instruction |= line[3] << 0x10;  //dest (dereferenced)
                    *instruction |= line[7];          //addr
                }
                else return false;
                break;
            case T_MOV:
                ENCODE_OP(line[0]);
                if (length >= 8 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 &&
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_REG && line[7] < 16) { //reg -> reg
                    ENCODE_MODE(0b0000); //mode
                    *instruction |= line[3] << 0x10;  //dest
                    *instruction |= line[7] << 0xC;   //src
                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 &&
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_DEREF && line[7] == T_REG && line[8] < 16) { //reg (addr) -> reg
                    ENCODE_MODE(0b0001); //mode
                    *instruction |= line[3] << 0x10;  //dest
                    *instruction |= line[8] << 0xC;   //src (dereferenced)

                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 &&
                    line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_REG && line[8] < 16) { //reg -> reg (addr)
                    ENCODE_MODE(0b0010); //mode
                    *instruction |= line[4] << 0x10;  //dest (dereferenced)
                    *instruction |= line[7] << 0xC;   //src
                }
                else if (length >= 10 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 &&
                    line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_DEREF && line[8] == T_REG && line[9] < 16) { //reg (addr) -> reg (addr)
                    ENCODE_MODE(0b0011); //mode
                    *instruction |= line[4] << 0x10;  //dest (dereferenced)
                    *instruction |= line[8] << 0xC;   //src (dereferenced)

                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 &&
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_DEREF && line[7] == T_VAL && line[8] < 0xFFFF) { //addr -> reg
                    ENCODE_MODE(0b0100); //mode
                    *instruction |= line[3] << 0x10;  //dest
                    *instruction |= line[8];          //addr
                }
                else if (length >= 10 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 &&
                    line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_DEREF && line[8] == T_VAL && line[9] < 0xFFFF) { //addr -> reg (addr)
                    ENCODE_MODE(0b0101); //mode
                    *instruction |= line[4] << 0x10;  //dest (dereferenced)
                    *instruction |= line[9];          //addr
                }
                else if (length >= 8 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 &&
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] < 0xFFFF) { //immv -> reg
                    ENCODE_MODE(0b0110); //mode
                    *instruction |= line[3] << 0x10;  //dest
                    *instruction |= line[7];          //immv
                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 &&
                    line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_VAL && line[8] <= 0xFFFF) { //immv -> reg (dereferenced)
                    ENCODE_MODE(0b0111); //mode
                    *instruction |= line[4] << 0x10;  //dest (dereferenced)
                    *instruction |= line[8];          //immv
                }
                else if (length >= 8 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 &&
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] <= 0xFFFF) { //reg -> addr
                    ENCODE_MODE(0b1000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                    *instruction |= line[7];          //addr
                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 &&
                    line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_VAL && line[8] < 0xFFFF) { //reg (addr) -> addr
                    ENCODE_MODE(0b1001); //mode
                    *instruction |= line[4] << 0x10;  //reg (dereferenced)
                    *instruction |= line[8];          //addr
                }
                else return false;
                break;
            case T_PUSH:
                ENCODE_OP(line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //push reg
                    ENCODE_MODE(0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //push reg (addr)
                    ENCODE_MODE(0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (dereferenced)
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_VAL && line[4] < 0xFFFF) { //push addr
                    ENCODE_MODE(0b0010); //mode
                    *instruction |= line[4];          //addr
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] <= 0xFFFF) { //push immv
                    ENCODE_MODE(0b0011); //mode
                    *instruction |= line[3];          //immv
                }
                else return false;
                break;
            case T_POP:
                ENCODE_OP(line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //pop reg
                    ENCODE_MODE(0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //pop reg (addr)
                    ENCODE_MODE(0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (dereferenced)
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_VAL && line[4] < 0xFFFF) { //pop addr
                    ENCODE_MODE(0b0010); //mode
                    *instruction |= line[4];          //addr
                }
                break;
            case T_CMP:
                ENCODE_OP(line[0]);
                if (length >= 8 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 &&
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_REG && line[7] < 16) { //reg to reg
                    ENCODE_MODE(0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg 1
                    *instruction |= line[7] << 0xC;   //reg 2
                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 &&
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_DEREF && line[7] == T_VAL && line[8] < 0xFFFF) { //reg to addr
                    ENCODE_MODE(0b0001); //mode
                    *instruction |= line[3] << 0x10;  //reg
                    *instruction |= line[8];          //addr
                }
                else if (length >= 8 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 &&
                    line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] <= 0xFFFF) { //reg to immv
                    ENCODE_MODE(0b0010); //mode
                    *instruction |= line[3] << 0x10;  //reg
                    *instruction |= line[7];          //immv
                }
                else return false;
                break;
            case T_JMP:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_JZ:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_JC:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_JS:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_JP:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_JE:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_JNE:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_JG:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_JGE:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_JL:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_JLE:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_CALL:
                ENCODE_OP(line[0]);
                JMP_OP;
                break;
            case T_RET:
                ENCODE_OP(line[0]);
                break;
            case T_INC:
                ENCODE_OP(line[0]);
                UNARY_OP;
                break;
            case T_DEC:
                ENCODE_OP(line[0]);
                UNARY_OP;
                break;
            case T_ADD:
                ENCODE_OP(line[0]);
                BINARY_OP;
                break;
            case T_SUB:
                ENCODE_OP(line[0]);
                BINARY_OP;
                break;
            case T_MUL:
                ENCODE_OP(line[0]);
                BINARY_OP;
                break;
            case T_DIV:
                ENCODE_OP(line[0]);
                BINARY_OP;
                break;
            case T_AND:
                ENCODE_OP(line[0]);
                BINARY_OP;
                break;
            case T_OR:
                ENCODE_OP(line[0]);
                BINARY_OP;
                break;
            case T_XOR:
                ENCODE_OP(line[0]);
                BINARY_OP;
                break;
            case T_NOT:
                ENCODE_OP(line[0]);
                UNARY_OP;
                break;
            case T_SHL:
                ENCODE_OP(line[0]);
                BINARY_OP;
                break;
            case T_SHR:
                ENCODE_OP(line[0]);
                BINARY_OP;
                break;
            case T_ROL:
                ENCODE_OP(line[0]);
                BINARY_OP;
                break;
            case T_ROR:
                ENCODE_OP(line[0]);
                BINARY_OP;
                break;
            case T_IN:
                ENCODE_OP(line[0]);
                IO_OP;
                break;
            case T_OUT:
                ENCODE_OP(line[0]);
                IO_OP;
                break;
            case T_DW:
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] <= 0xFFFF)
                    *instruction = line[3]; //data value
                break;
            case T_LABEL: //label will have already been found, skip
                break;
            case T_NL: //empty line, skip
                break;
    }
    return true;
}

/* compile a tokenized program to an executable */
bool compile(FILE* executable, const unsigned int length) {
    /* resolve all labels */
    if (!find_labels(tokenlist, length)) return false;

    /* split program into individual lines */
    unsigned int line_count = 0; //number of lines in the program
    unsigned int parser_head = 0; //index of current token
    unsigned int i; //reusable index
    for (i = 0; i < length; i++) { //count lines
        if (tokenlist[i] == T_NL) line_count++;
    }
    int** program_lines = (int **)calloc(line_count, sizeof(int *));
    for (i = 0; i < line_count; i++) { //allocate memory for each line
        program_lines[i] = (int *)calloc(MAX_LINE_LENGTH, sizeof(int));
    }
    for (i = 0; i < line_count; i++) {
        for (int j = 0; parser_head < length && j < MAX_LINE_LENGTH; j++) {
            program_lines[i][j] = tokenlist[parser_head++];
            if (program_lines[i][j] == T_NL) break;
        }
    }

    /* compile lines one by one */
    uint32_t current_instruction;
    for (i = 0; i < line_count; i++) {
        if(!compile_line(program_lines[i], &current_instruction)){
            error(PARSER, program_lines[i][0], i + 1); //throw error
            return false;
        }
        if(debug) {
            output_encoding(current_instruction);
        }
        fwrite(&current_instruction, sizeof(uint32_t), 1, executable);
    }

    free(program_lines);
    return true;
}
