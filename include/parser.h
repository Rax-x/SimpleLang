#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
#include "ast.h"

typedef struct _parser {
    lexer_t lexer;
    token_t curr;
    token_t prev;
} parser_t;

parser_t init_parser(string_view_t source);
const ast_node_t* parse_program(parser_t* p);

#endif
