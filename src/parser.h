#ifndef KWIK_PARSER_STATE_H
#define KWIK_PARSER_STATE_H

#include "grammar.h"

struct ParserState {
    int num_errors;
};

void* LemonParseAlloc(void* (*alloc_proc)(size_t));
void LemonParse(void* state, int token_id, double token_data, ParserState* s);
void LemonParseFree(void*, void(*free_proc)(void*));


#endif
