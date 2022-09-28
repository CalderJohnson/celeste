#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <lexer.h>

/* label resolving */
typedef struct {
    char name[10];
    unsigned int addr;
} label;

static label labels[50];
static unsigned int label_index = 0;

static bool find_labels(int** program_lines) {

    return true;
}

/* encode one line of tokens */
static bool compile_line(const uint32_t* line, uint32_t* instruction) {
    *instruction = 0; //clear garbage value

    //operation tokens are the value of their opcode
    #define ENCODE_OP(instruction, opcode) *instruction |= opcode << 0x18
    #define ENCODE_MODE(instruction, mode) *instruction |= mode   << 0x14

    unsigned int length;
    for (length = 0; line[length] != T_NL; length++) {}
    length++;
    switch (line[0]) {
            case T_NOP:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_HLT:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_INT:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 256)
                    *instruction |= line[3] << 8;     //intcode
                else return false;
                break;
            case T_LEA:
                ENCODE_OP(instruction, line[0]);
                if (length >= 8 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] < 0xFFFF) { //load to reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //dest
                    *instruction |= line[7];          //addr
                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 && line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_VAL && line[8] < 0xFFFF) { //load to reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[3] << 0x10;  //dest (dereferenced)
                    *instruction |= line[7];          //addr
                }
                else return false;
                break;
            case T_MOV:
                ENCODE_OP(instruction, line[0]);
                if (length >= 8 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_REG && line[7] < 16) { //reg -> reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //dest
                    *instruction |= line[7] << 0xC;   //src
                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_DEREF && line[7] == T_REG && line[8] < 16) { //reg (addr) -> reg
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[3] << 0x10;  //dest
                    *instruction |= line[8] << 0xC;   //src (dereferenced)

                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 && line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_REG && line[8] < 16) { //reg -> reg (addr)
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[4] << 0x10;  //dest (dereferenced)
                    *instruction |= line[7] << 0xC;   //src
                }
                else if (length >= 10 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 && line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_DEREF && line[8] == T_REG && line[9] < 16) { //reg (addr) -> reg (addr)
                    ENCODE_MODE(instruction, 0b0011); //mode
                    *instruction |= line[4] << 0x10;  //dest (dereferenced)
                    *instruction |= line[8] << 0xC;   //src (dereferenced)

                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_DEREF && line[7] == T_VAL && line[8] < 0xFFFF) { //addr -> reg
                    ENCODE_MODE(instruction, 0b0100); //mode
                    *instruction |= line[3] << 0x10;  //dest
                    *instruction |= line[8];          //addr
                }
                else if (length >= 10 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 && line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_DEREF && line[8] == T_VAL && line[9] < 0xFFFF) { //addr -> reg (addr)
                    ENCODE_MODE(instruction, 0b0101); //mode
                    *instruction |= line[4] << 0x10;  //dest (dereferenced)
                    *instruction |= line[9];          //addr
                }
                else if (length >= 8 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] < 0xFFFF) { //immv -> reg
                    ENCODE_MODE(instruction, 0b0110); //mode
                    *instruction |= line[3] << 0x10;  //dest
                    *instruction |= line[7];          //immv
                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 && line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_VAL && line[8] <= 0xFFFF) { //immv -> reg (dereferenced)
                    ENCODE_MODE(instruction, 0b0111); //mode
                    *instruction |= line[4] << 0x10;  //dest (dereferenced)
                    *instruction |= line[8];          //immv
                }
                else if (length >= 8 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] <= 0xFFFF) { //reg -> addr
                    ENCODE_MODE(instruction, 0b1000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                    *instruction |= line[7];          //addr
                }
                else if (length >= 9 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16 && line[5] == T_COMMA && line[6] == T_SPACE && line[7] == T_VAL && line[8] < 0xFFFF) { //reg (addr) -> addr
                    ENCODE_MODE(instruction, 0b1001); //mode
                    *instruction |= line[4] << 0x10;  //reg (dereferenced)
                    *instruction |= line[8];          //addr
                }
                else return false;
                break;
            case T_PUSH:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //push reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //push reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (dereferenced)
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_VAL && line[4] < 0xFFFF) { //push addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[4];          //addr
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] <= 0xFFFF) { //push immv
                    ENCODE_MODE(instruction, 0b0011); //mode
                    *instruction |= line[3];          //immv
                }
                else return false;
                break;
            case T_POP:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //pop reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //pop reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (dereferenced)
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_VAL && line[4] < 0xFFFF) { //pop addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[4];          //addr
                }
                break;
            case T_JMP:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                break;
            case T_CMP:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_JZ:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                break;
            case T_JC:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                
                break;
            case T_JS:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                break;
            case T_JP:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                break;
            case T_JE:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                break;
            case T_JNE:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                break;
            case T_JG:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                break;
            case T_JGE:  
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                break;
            case T_JL:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                break;
            case T_JLE:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                break;
            case T_CALL:
                ENCODE_OP(instruction, line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16) { //jump to addr in reg
                    ENCODE_MODE(instruction, 0b0000); //mode
                    *instruction |= line[3] << 0x10;  //reg
                }
                else if (length >= 5 && line[1] == T_SPACE && line[2] == T_DEREF && line[3] == T_REG && line[4] < 16) { //jump to addr in reg (addr)
                    ENCODE_MODE(instruction, 0b0001); //mode
                    *instruction |= line[4] << 0x10;  //reg (addr)
                }
                else if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 0xFFFF) { //jump to addr
                    ENCODE_MODE(instruction, 0b0010); //mode
                    *instruction |= line[3];          //addr
                }
                else return false;
                break;
            case T_RET:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_INC:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_DEC:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_ADD:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_SUB:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_MUL:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_DIV:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_AND:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_OR:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_XOR:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_NOT:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_SHL:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_SHR:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_ROL:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_ROR:
                ENCODE_OP(instruction, line[0]);
                break;
            case T_IN:
                ENCODE_OP(instruction, line[0]);
                if (length >= 7 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] < 256) {
                    *instruction |= line[3] << 0x10;
                    *instruction |= line[7] << 0x8;
                }
                else return false;
                break;
            case T_OUT:
                ENCODE_OP(instruction, line[0]);
                if (length >= 7 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] < 256) {
                    *instruction |= line[3] << 0x10;
                    *instruction |= line[7] << 0x8;
                }
                else return false;
                break;
            case T_DW:
                if (length == 3)
                    *instruction = line[2]; //data value
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
    /* split program into individual lines */
    unsigned int line_count = 0; //number of lines in the program
    unsigned int parser_head = 0; //index of current token
    for (int i = 0; i < length; i++) { //count lines
        if (tokenlist[i] == T_NL) line_count++;
    }
    int** program_lines = (int **)calloc(line_count, sizeof(int *));
    for (int i = 0; i < line_count; i++) { //allocate memory for each line
        program_lines[i] = (int *)calloc(MAX_LINE_LENGTH, sizeof(int));
    }
    for (int i = 0; i < line_count; i++) {
        for (int j = 0; parser_head < length && j < MAX_LINE_LENGTH; j++) {
            program_lines[i][j] = tokenlist[parser_head++];
            if (program_lines[i][j] == T_NL) break;
        }
    }

    /* resolve all labels */
    if (!find_labels(program_lines)) return false;
    /* compile lines one by one */
    uint32_t current_instruction;
    for (int i = 0; i < line_count; i++) {
        if(!compile_line(program_lines[i], &current_instruction)) return false;
        fwrite(&current_instruction, sizeof(uint32_t), 1, executable);
    }

    return true;
}