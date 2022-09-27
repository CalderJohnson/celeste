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
    #define ENCODE_OP(opcode) opcode << 0x18 

    unsigned int length;
    for (length = 0; line[length] != T_NL; length++) {}
    length++;
    switch (line[0]) {
            case T_NOP:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_HLT:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_INT:
                *instruction |= ENCODE_OP(line[0]);
                if (length >= 4 && line[1] == T_SPACE && line[2] == T_VAL && line[3] < 256)
                    *instruction |= line[3] << 8;
                else return false;
                break;
            case T_LEA:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_MOV:
                *instruction |= ENCODE_OP(line[0]);
                if (length >= 7 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_REG && line[7] < 16) { //reg -> reg
                    *instruction |= 0b0000 << 0x14;  //mode
                    *instruction |= line[3] << 0x10; //dest
                    *instruction |= line[7] << 0xC;  //src
                }
                else if (length >= 7 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] < 0xFFFF) { //immv -> reg
                    *instruction |= 0b0110 << 0x14;  //mode
                    *instruction |= line[3] << 0x10; //dest
                    *instruction |= line[7];         //immv
                }
                else return false;
                break;
            case T_PUSH:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_POP:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_JMP:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_CMP:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_JZ:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_JC:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_JS:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_JP:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_JE:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_JNE:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_JG:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_JGE:  
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_JL:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_JLE:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_CALL:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_RET:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_INC:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_DEC:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_ADD:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_SUB:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_MUL:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_DIV:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_AND:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_OR:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_XOR:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_NOT:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_SHL:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_SHR:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_ROL:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_ROR:
                *instruction |= ENCODE_OP(line[0]);
                break;
            case T_IN:
                 *instruction |= ENCODE_OP(line[0]);
                if (length >= 7 && line[1] == T_SPACE && line[2] == T_REG && line[3] < 16 && line[4] == T_COMMA && line[5] == T_SPACE && line[6] == T_VAL && line[7] < 256) {
                    *instruction |= line[3] << 0x10;
                    *instruction |= line[7] << 0x8;
                }
                else return false;
                break;
            case T_OUT:
                *instruction |= ENCODE_OP(line[0]);
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