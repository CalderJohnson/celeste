#include <stdio.h>
#include <string.h>
#include <error.h>

#define RED   "\033[31m" //error color
#define GREEN "\033[32m" //success color
#define RESET "\033[0m"

/* instruction names map */
char* instruction_names[] = {
    [0]  = "nop",
    [1]  = "hlt",
    [2]  = "int",
    [3]  = "lea",
    [4]  = "mov",
    [5]  = "push",
    [6]  = "pop",
    [7]  = "jmp",
    [8]  = "cmp",
    [9]  = "jz",
    [10] = "jc",
    [11] = "js",
    [12] = "jp",
    [13] = "je",
    [14] = "jne",
    [15] = "jg",
    [16] = "jge",
    [17] = "jl",
    [18] = "jle",
    [19] = "call",
    [20] = "ret",
    [21] = "inc",
    [22] = "dec",
    [23] = "add",
    [24] = "sub",
    [25] = "mul",
    [26] = "div",
    [27] = "and",
    [28] = "or",
    [29] = "xor",
    [30] = "not",
    [31] = "shl",
    [32] = "shr",
    [33] = "rol",
    [34] = "ror",
    [35] = "in",
    [36] = "out",
    [37] = "non instruction",
};

/* emits a formatted error */
void error (type t, int instruction, int line_num) {
    if (t == LEXER)
        printf(RED "Lexer error on line %d\n" RESET, line_num);
    else if (t == PARSER)
        printf(RED "Parser error, for instruction %s on line %d\n" RESET, instruction_names[instruction], line_num);
    return;
}

/* emits a formatted success message */
void success (type t) {
    if (t == LEXER)
        printf(GREEN "Successfully tokenized...\n" RESET);
    else if (t == PARSER)
        printf(GREEN "Successfully compiled...\n" RESET);
    return;
}