#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <lexer.h>
#include <error.h>

int tokenlist[MAX_PROGRAM_LENGTH]; //contains the tokenized program for the parser
unsigned int token_index = 0;      //points to the next open index for a token

/* map mnemonic to token */
static bool keyword_map (const char* keyword) {
    /* opcodes */
    if      (strcmp(keyword, "nop")   == 0)   tokenlist[token_index++] = T_NOP;
    else if (strcmp(keyword, "hlt")   == 0)   tokenlist[token_index++] = T_HLT;
    else if (strcmp(keyword, "int")   == 0)   tokenlist[token_index++] = T_INT;
    else if (strcmp(keyword, "lea")   == 0)   tokenlist[token_index++] = T_LEA;
    else if (strcmp(keyword, "mov")   == 0)   tokenlist[token_index++] = T_MOV;
    else if (strcmp(keyword, "push")  == 0)   tokenlist[token_index++] = T_PUSH;
    else if (strcmp(keyword, "pop")   == 0)   tokenlist[token_index++] = T_POP;
    else if (strcmp(keyword, "jmp")   == 0)   tokenlist[token_index++] = T_JMP;
    else if (strcmp(keyword, "cmp")   == 0)   tokenlist[token_index++] = T_CMP;
    else if (strcmp(keyword, "jz")    == 0)   tokenlist[token_index++] = T_JZ;
    else if (strcmp(keyword, "jc")    == 0)   tokenlist[token_index++] = T_JC;
    else if (strcmp(keyword, "js")    == 0)   tokenlist[token_index++] = T_JS;
    else if (strcmp(keyword, "jp")    == 0)   tokenlist[token_index++] = T_JP;
    else if (strcmp(keyword, "je")    == 0)   tokenlist[token_index++] = T_JE;
    else if (strcmp(keyword, "jne")   == 0)   tokenlist[token_index++] = T_JNE;
    else if (strcmp(keyword, "jg")    == 0)   tokenlist[token_index++] = T_JG;
    else if (strcmp(keyword, "jge")   == 0)   tokenlist[token_index++] = T_JGE;
    else if (strcmp(keyword, "jl")    == 0)   tokenlist[token_index++] = T_JL;
    else if (strcmp(keyword, "jle")   == 0)   tokenlist[token_index++] = T_JLE;
    else if (strcmp(keyword, "call")  == 0)   tokenlist[token_index++] = T_CALL;
    else if (strcmp(keyword, "ret")   == 0)   tokenlist[token_index++] = T_RET;
    else if (strcmp(keyword, "inc")   == 0)   tokenlist[token_index++] = T_INC;
    else if (strcmp(keyword, "dec")   == 0)   tokenlist[token_index++] = T_DEC;
    else if (strcmp(keyword, "add")   == 0)   tokenlist[token_index++] = T_ADD;
    else if (strcmp(keyword, "sub")   == 0)   tokenlist[token_index++] = T_SUB;
    else if (strcmp(keyword, "mul")   == 0)   tokenlist[token_index++] = T_MUL;
    else if (strcmp(keyword, "div")   == 0)   tokenlist[token_index++] = T_DIV;
    else if (strcmp(keyword, "and")   == 0)   tokenlist[token_index++] = T_AND;
    else if (strcmp(keyword, "or")    == 0)   tokenlist[token_index++] = T_OR;
    else if (strcmp(keyword, "xor")   == 0)   tokenlist[token_index++] = T_XOR;
    else if (strcmp(keyword, "not")   == 0)   tokenlist[token_index++] = T_NOT;
    else if (strcmp(keyword, "shl")   == 0)   tokenlist[token_index++] = T_SHL;
    else if (strcmp(keyword, "shr")   == 0)   tokenlist[token_index++] = T_SHR;
    else if (strcmp(keyword, "rol")   == 0)   tokenlist[token_index++] = T_ROL;
    else if (strcmp(keyword, "ror")   == 0)   tokenlist[token_index++] = T_ROR;
    else if (strcmp(keyword, "in")    == 0)   tokenlist[token_index++] = T_IN;
    else if (strcmp(keyword, "out")   == 0)   tokenlist[token_index++] = T_OUT;

    /* registers */
    else if (strcmp(keyword, "ax")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 0;  }
    else if (strcmp(keyword, "bx")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 1;  }
    else if (strcmp(keyword, "cx")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 2;  }
    else if (strcmp(keyword, "dx")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 3;  }
    else if (strcmp(keyword, "si")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 4;  }
    else if (strcmp(keyword, "di")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 5;  }
    else if (strcmp(keyword, "sp")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 6;  }
    else if (strcmp(keyword, "bp")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 7;  }
    else if (strcmp(keyword, "ip")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 8;  }
    else if (strcmp(keyword, "flags") == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 9;  }
    else if (strcmp(keyword, "r1")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 10; }
    else if (strcmp(keyword, "r2")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 11; }
    else if (strcmp(keyword, "r3")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 12; }
    else if (strcmp(keyword, "r4")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 13; }
    else if (strcmp(keyword, "r5")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 14; }
    else if (strcmp(keyword, "r6")    == 0) { tokenlist[token_index++] = T_REG; tokenlist[token_index++] = 15; }

    /* other */
    else if (strcmp(keyword, "dw")    == 0)  tokenlist[token_index++] = T_DW;
    else return false;
    return true;
}

/* label tokenizing */
static unsigned int label_count = 0;
static char label_names[50][10];

static bool label_found (const char *label) {
    for (unsigned int i = 0; i < label_count; i++) {
        if (strcmp(label, label_names[i]) == 0) return true;
    }
    return false;
}

/* tokenize the asm file */
bool tokenize(const char* buffer) {
    unsigned int line_count = 1; //for error handling
    char tempbuffer[10];
    for (unsigned int index = 0; index < strlen(buffer); index++) {
        if (buffer[index] == ';') { //comment
            while (index < strlen(buffer) && buffer[++index] != '\n') {/* iterate */}
            tokenlist[token_index++] = T_NL;
            line_count++;
        }
        else if (buffer[index] == ' ') { //space
            tokenlist[token_index++] = T_SPACE;
        }
        else if (buffer[index] == ':') { //colon (label definition)
            tokenlist[token_index++] = T_COLON;
        }
        else if (buffer[index] == '\n') { //newline
            tokenlist[token_index++] = T_NL;
            line_count++;
        }
        else if (buffer[index] == '*') { //dereference
            tokenlist[token_index++] = T_DEREF;
        }
        else if (buffer[index] == ',') { //comma
            tokenlist[token_index++] = T_COMMA;
        }
        else if (buffer[index] >= 97 && buffer[index] <= 122) { //letter (label/opcode/reg/dw)
            int tempbuffer_index;
            for(tempbuffer_index = 0; buffer[index] >= 97 && buffer[index] <= 122; tempbuffer_index++) {
                tempbuffer[tempbuffer_index] = buffer[index++];
            }
            tempbuffer[tempbuffer_index] = '\0';
            if (!keyword_map(tempbuffer)) {         //map to keyword
                tokenlist[token_index++] = T_LABEL; //if not, must be a label
                if (!label_found(tempbuffer)) {     //if label is new, assign it a number
                    strcpy(label_names[label_count], tempbuffer);
                    tokenlist[token_index++] = label_count;
                    label_count++;
                }
                else { //if label is not new, find its number
                    for (unsigned int i = 0; i < label_count; i++) {
                        if (strcmp(tempbuffer, label_names[i]) == 0) tokenlist[token_index++] = i;
                    }
                }
            }
            index--; //correct index
        }
        else if (buffer[index] >= 48 && buffer[index] <= 57) { //number (digit)
            tokenlist[token_index++] = T_VAL;
            int tempbuffer_index;
            for (tempbuffer_index = 0; buffer[index] >= 48 && buffer[index] <= 57; tempbuffer_index++) {
                tempbuffer[tempbuffer_index] = buffer[index++];
            }
            tempbuffer[tempbuffer_index] = '\0';
            tokenlist[token_index++] = atoi(tempbuffer);
            index--; //correct index
        }
        else {
            error(LEXER, 0, line_count); //throw error, unknown characters
            return false;
        }
    }
    tokenlist[token_index++] = T_NL; //trailing newline so that the last line is known to the parser
    return true;
}
