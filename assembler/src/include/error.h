#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>

extern bool debug; //toggles the outputting encodings during compilation

/* error codes */
typedef enum {
    LEXER,
    PARSER,
} type;

void error (type t, int instruction, int line_num);
void success (type t);
void output_encoding(int instruction);

#endif
