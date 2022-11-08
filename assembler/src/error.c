#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <error.h>

#define RED    "\033[31m" //error color
#define GREEN  "\033[32m" //success color
#define YELLOW "\033[33m" //encoding color
#define RESET  "\033[0m"

bool debug = true;

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

#define BYTE_PATTERN "%c%c%c%c%c%c%c%c "
#define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

/* output a formatted encoding */
void output_encoding(int instruction) {
    printf(YELLOW "Emmiting: " BYTE_PATTERN BYTE_PATTERN BYTE_PATTERN BYTE_PATTERN "\n" RESET,
        BYTE_TO_BINARY((instruction & 0xFF000000) >> 24),
        BYTE_TO_BINARY((instruction & 0x00FF0000) >> 16),
        BYTE_TO_BINARY((instruction & 0x0000FF00) >> 8),
        BYTE_TO_BINARY((instruction & 0x000000FF) >> 0)
    );
    return;
}
