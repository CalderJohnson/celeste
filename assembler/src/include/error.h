#ifndef ERROR_H
#define ERROR_H

/* error codes */
typedef enum {
    LEXER,
    PARSER,
} type;

void error (type t, int instruction, int line_num);
void success (type t);

#endif
