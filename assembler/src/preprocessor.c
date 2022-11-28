#include <stdlib.h>
#include <string.h>

static char* remove_whitespace(const char* buffer) {
    char* result_buffer = (char *)calloc(strlen(buffer), sizeof(char));
    unsigned int index = 0;
    char last_char = ' ';
    for(int i = 0; i < strlen(buffer); i++) {
        if(!(last_char == '\n' || last_char == ' ') || !(buffer[i] == ' ' || buffer[i] == '\n'))
            result_buffer[index++] = buffer[i];
        last_char = buffer[i];
    }
    return result_buffer;
}

/* remove excess whitespace */
char* preprocess(const char* buffer) {
    return remove_whitespace(buffer);
}

//in the future, this is where includes and macros will be resolved
