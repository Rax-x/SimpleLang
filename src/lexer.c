#include "../include/lexer.h"

#include <stdbool.h>
#include <ctype.h>

static inline token_t make_token(const lexer_t* restrict lex, int type) {
    const int length = lex->current - lex->start;
    return (token_t) {
        .line = lex->line,
        .type = type,
        .lexeme = string_view_substr(lex->source, lex->start, length)
    };
}

static inline bool is_at_end(const lexer_t* restrict lex) {
    return lex->current >= (int)string_view_size(lex->source);
}

static inline char lex_peek(const lexer_t* restrict lex) {
    return string_view_at(lex->source, lex->current);
}

static inline char lex_advance(lexer_t* restrict lex) {
    const char c = lex_peek(lex);
    return !is_at_end(lex)
        ? (lex->current++, c)
        : '\0';
}

static inline bool match(lexer_t* restrict lex, char c) {
    return (lex_peek(lex) == c)
        ? (lex_advance(lex), true)
        : false;
}

static void skip_whitespaces(lexer_t* restrict lex) {
    while(!is_at_end(lex)) {
        switch(lex_peek(lex)) {
            case '#':
                // single line comment
                while(lex_peek(lex) != '\n' && !is_at_end(lex)) {
                    lex_advance(lex);
                }
            case '\n':
                lex->line++;
            case ' ':
            case '\r':
            case '\t':
                break;
            default:
                return;
        }

        lex_advance(lex);
    }
}

token_t next_token(lexer_t* restrict lex) {

    skip_whitespaces(lex);
    lex->start = lex->current;

    const char c = lex_advance(lex);

    switch(c) {
        case '\0':
            return make_token(lex, TOK_EOF);
        case '+':
            return make_token(lex, PLUS);
        case '-':
            return make_token(lex, MINUS);
        case '/':
            return make_token(lex, SLASH);
        case '*':
            return make_token(lex, STAR);
        case '=':
            return make_token(lex, ASSIGN);
        case ',':
            return make_token(lex, COMMA);
        case ';':
            return make_token(lex, SEMICOLON);
        case '(':
            return make_token(lex, LEFT_PAREN);
        case ')':
            return make_token(lex, RIGHT_PAREN);
        case '[':
            return make_token(lex, LEFT_BRACKET);
        case ']':
            return make_token(lex, RIGHT_BRACKET);
        case '{':
            return make_token(lex, LEFT_BRACE);
        case '}':
            return make_token(lex, RIGHT_BRACE);
        case '>':
            return make_token(lex, match(lex, '=') ? GREATER_EQ : GREATER);
        case '<':
            return make_token(lex, match(lex, '=') ? LESS_EQ : LESS);
        default: {

            if(isdigit(c)) {

                token_type_t type = INTEGER_LITERAL;
                while(isdigit(lex_peek(lex)) && !is_at_end(lex)) {
                    lex_advance(lex);
                }

                if(match(lex, '.') && isdigit(lex_peek(lex))) {
                    type = FLOATING_LITERAL;
                    while(isdigit(lex_peek(lex)) && !is_at_end(lex)) {
                        lex_advance(lex);
                    }
                }

                return make_token(lex, type);
            }

            if(isalpha(c)) {
                while(isalpha(lex_peek(lex)) && !is_at_end(lex)) {
                    lex_advance(lex);
                }

                const string_view_t lexeme = string_view_substr(lex->source,
                                                                lex->start,
                                                                lex->current - lex->start);

                token_type_t type = IDENTIFIER;
                if(string_view_equal(lexeme, new_string_view_from_cstr("var"))) {
                    type = VAR_KEYWORD;
                } else if(string_view_equal(lexeme, new_string_view_from_cstr("let"))) {
                    type = LET_KEYWORD;
                } else if(string_view_equal(lexeme, new_string_view_from_cstr("true"))) {
                    type = TRUE_KEYWORD;
                } else if(string_view_equal(lexeme, new_string_view_from_cstr("false"))) {
                    type = FALSE_KEYWORD;
                } else if(string_view_equal(lexeme, new_string_view_from_cstr("if"))) {
                    type = IF_KEYWORD;
                } else if(string_view_equal(lexeme, new_string_view_from_cstr("then"))) {
                    type = THEN_KEYWORD;
                } else if(string_view_equal(lexeme, new_string_view_from_cstr("else"))) {
                    type = ELSE_KEYWORD;
                } else if(string_view_equal(lexeme, new_string_view_from_cstr("float"))) {
                    type = FLOAT_KEYWORD;
                } else if(string_view_equal(lexeme, new_string_view_from_cstr("integer"))) {
                    type = INTEGER_KEYWORD;
                } else if(string_view_equal(lexeme, new_string_view_from_cstr("bool"))) {
                    type = BOOL_KEYWORD;
                }

                return make_token(lex, type);
            }

            break;
        }
    }
    
    return make_token(lex, TOK_ERR);
}
