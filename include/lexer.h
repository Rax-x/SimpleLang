#ifndef _LEXER_H_
#define _LEXER_H_

#include "string_view.h"

typedef enum _token_type {
    PLUS,
    MINUS,
    STAR,
    SLASH,
    LESS,
    ASSIGN,
    COMMA,
    SEMICOLON,
    GREATER,
    GREATER_EQ,
    LESS_EQ,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_PAREN,
    RIGHT_PAREN,
    INTEGER_LITERAL,
    FLOATING_LITERAL,

    TRUE_KEYWORD,
    FALSE_KEYWORD,
    LET_KEYWORD,
    VAR_KEYWORD,
    AS_KEYWORD,
    IF_KEYWORD,
    ELSE_KEYWORD,
    THEN_KEYWORD,
    FLOAT_KEYWORD,
    INTEGER_KEYWORD,
    BOOL_KEYWORD,
    IDENTIFIER,
    TOK_EOF,
    TOK_ERR
} token_type_t;

typedef struct _token {
    token_type_t type;

    int line;
    string_view_t lexeme;
} token_t;

typedef struct _lexer {
    string_view_t source;

    int current;
    int start;
    int line;
} lexer_t;

#define INIT_LEXER(src) (lexer_t){ .source = (src), .current = 0, .start = 0, .line = 1}

token_t next_token(lexer_t* lex);

#endif
