#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>
#include <lexer.h>

bool compile(FILE* executable, const unsigned int length);

#endif
