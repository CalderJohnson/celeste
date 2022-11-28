/* dedicated assembler for celeste */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <preprocessor.h>
#include <lexer.h>
#include <parser.h>
#include <error.h>

int main (int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: assembler <asm_file>\n");
        return 1;
    }

    char* filename = argv[1];
    FILE* source = fopen(filename, "rb");

    if (source == NULL) {
        printf("Cannot find file: %s\n", filename);
        return 1;
    }

    char* executable_name = malloc(sizeof(char) * strlen(filename));
    strncpy(executable_name, filename, strlen(filename) - 4);
    strcpy(&executable_name[strlen(filename) - 4], ".bin\0");
    FILE* executable = fopen(executable_name, "wb");

    fseek(source, 0, SEEK_END);
    int length = ftell(source);
    fseek(source, 0, SEEK_SET);
    char* raw_buffer = (char *)calloc(length, sizeof(char));
    fread(raw_buffer, 1, length, source);
    fclose(source);

    char* buffer = preprocess(raw_buffer);
    free(raw_buffer);

    if (tokenize(buffer)) {
        success(LEXER);
        if (compile(executable, token_index)) {
            success(PARSER);
        }
        else {
            printf("Parsing error\n");
        }
    }
    else {
        printf("Lexer error\n");
    }
    free(buffer);
    free(executable_name);
    return 0;
}
