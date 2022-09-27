/* dedicated assembler for celeste */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <lexer.h>
#include <parser.h>

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
    char* buffer = (char *)calloc(length, sizeof(char));
    fread(buffer, 1, length, source);
    fclose(source);

    if (tokenize(buffer)) {
        printf("Successfully tokenized...\n");
        if (compile(executable, token_index)) {
            printf("Successfully compiled... \n");
        }
        else {
            printf("Parsing error\n");
        }
    }
    else {
        printf("Lexer error\n");
    }
    return 0;
}