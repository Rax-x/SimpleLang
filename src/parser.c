#include "../include/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PARSER_CURR(p) (p->curr)
#define PARSER_PREV(p) (p->prev)

inline parser_t init_parser(string_view_t source) {

    lexer_t lex = INIT_LEXER(source);
    
    return (parser_t) {
        .lexer = lex,
        .curr = next_token(&lex)
    };
}

static inline void parser_advance(parser_t* restrict p) {
    p->prev = p->curr;

    if(p->curr.type != TOK_EOF && p->curr.type != TOK_ERR) {
        p->curr = next_token(&p->lexer);
    }
}

static inline bool parser_match(parser_t* restrict p, token_type_t type) {
    return (PARSER_CURR(p).type == type)
        ? (parser_advance(p), true)
        : false;
}

static inline token_t parser_consume(parser_t* restrict p, int kind) {
    if(parser_match(p, kind)) {
       return PARSER_PREV(p);
    }

    fprintf(stderr, "[Ln: %d] Parser error unexpected token.\n", PARSER_CURR(p).line);
    exit(EXIT_FAILURE);
}

static ast_node_t* parse_expression(parser_t* p);
static ast_node_t* parse_statement(parser_t* p);

static ast_node_t* parse_primary(parser_t* p) {

    if(parser_match(p, INTEGER_LITERAL) || parser_match(p, FLOATING_LITERAL)) {
        const float value = strtof(string_view_data(PARSER_PREV(p).lexeme), NULL);
        return new_literal_expr(value, PARSER_PREV(p).type == INTEGER_LITERAL, false);
    }


    if(parser_match(p, TRUE_KEYWORD)) {
        return new_literal_expr(1, false, true);
    }

    if(parser_match(p, FALSE_KEYWORD)) {
        return new_literal_expr(0, false, true);
    }

    if(parser_match(p, LEFT_PAREN)) {
        ast_node_t* const expr = parse_expression(p);
        parser_consume(p, RIGHT_PAREN);

        return expr;
    }

    if(parser_match(p, IDENTIFIER)) {
        return new_variable_expr(PARSER_PREV(p));
    }

    fprintf(stderr, "[Ln: %d] Unknown expression.\n", PARSER_CURR(p).line);
    exit(EXIT_FAILURE);
}

static ast_node_t* parse_subscript(parser_t* p) {
    ast_node_t* expr = parse_primary(p);

    while(parser_match(p, LEFT_BRACKET)) {
        expr = new_subscript_expr(expr, parse_expression(p));
        parser_consume(p, RIGHT_BRACKET);
    }

    return expr;
}

static ast_node_t* parse_unary(parser_t* p) {

    if(parser_match(p, MINUS) || parser_match(p, PLUS)) {
        token_t op = PARSER_PREV(p);
        return new_unary_expr(op, parse_unary(p));
    }
    
    return parse_subscript(p);
}

static ast_node_t* parse_factor(parser_t* p) {
    ast_node_t* left = parse_unary(p);

    while(parser_match(p, STAR) || parser_match(p, SLASH)) {
        token_t op = PARSER_PREV(p);
        left = new_binary_expr(op, left, parse_unary(p));
    }

    return left;
}

static ast_node_t* parse_term(parser_t* p) {
    ast_node_t* left = parse_factor(p);

    while(parser_match(p, PLUS) || parser_match(p, MINUS)) {
        token_t op = PARSER_PREV(p);
        left = new_binary_expr(op, left, parse_factor(p));
    }

    return left;
}

static ast_node_t* parse_comparison(parser_t* p) {
    ast_node_t* left = parse_term(p);

    while(parser_match(p, GREATER) || 
          parser_match(p, GREATER_EQ) || 
          parser_match(p, LESS) || 
          parser_match(p, LESS_EQ)) {
        token_t op = PARSER_PREV(p);
        left = new_binary_expr(op, left,parse_term(p));
    }

    return left;
}

static ast_node_t* parse_assignment(parser_t* p) {

    ast_node_t* lvalue = parse_comparison(p);

    if(parser_match(p, ASSIGN)) {

        if(lvalue->kind != VARIABLE_EXPR_NODE &&
           lvalue->kind != SUBSCRIPT_EXPR_NODE) {
            fprintf(stderr, "[Ln: %d] Can't assign to an rvalue.\n",
                    PARSER_CURR(p).line);
            exit(EXIT_FAILURE);
        }
    
        return new_assign_expr(lvalue, parse_assignment(p));
    }
    
    return lvalue;
}

static inline ast_node_t* parse_expression(parser_t* p){
    return parse_assignment(p);
}

static ast_node_t* parse_type_expr(parser_t* p) {
    
    token_t name;
    if(parser_match(p, FLOAT_KEYWORD) ||
       parser_match(p, INTEGER_KEYWORD) ||
       parser_match(p, BOOL_KEYWORD)) {
        name = PARSER_PREV(p);
    } else {
        fprintf(stderr, "[Ln: %d] Unknown data type.\n", PARSER_CURR(p).line);
        exit(EXIT_FAILURE);
    }

    ast_node_t* type = new_simple_type_expr(name);

    while(parser_match(p, LEFT_BRACKET)) {

        token_t literal = parser_consume(p, INTEGER_LITERAL);
        unsigned int length = strtol(string_view_data(literal.lexeme), NULL, 10);
        
        type = new_array_type_expr(type, length);

        parser_consume(p, RIGHT_BRACKET);
    }

    return type;
}

static ast_node_t* parse_var_declaration(parser_t* p) {

    const bool is_type_inferred = PARSER_PREV(p).type == LET_KEYWORD;

    token_t name = parser_consume(p, IDENTIFIER);

    ast_node_t* type = !is_type_inferred 
        ? parse_type_expr(p)
        : NULL;

    ast_node_t* initializer = NULL;
    if(parser_match(p, ASSIGN)) {
        initializer = parse_expression(p);
    }

    if(initializer == NULL && is_type_inferred) {
        fprintf(stderr, "[Ln: %d] Variables declared with 'let' must be initialized.\n", name.line);
        exit(EXIT_FAILURE);
    }

    parser_consume(p, SEMICOLON);

    return new_var_decl(name, type, initializer);
}


static inline ast_node_t* parse_expr_statement(parser_t* p) {
    ast_node_t* expr = parse_expression(p);
    parser_consume(p, SEMICOLON);

    return new_expr_stmt(expr);
}

static ast_node_t* parse_if_statement(parser_t* p) {
    ast_node_t* const condition = parse_expression(p);
    
    parser_consume(p, THEN_KEYWORD);
    ast_node_t* const then = parse_statement(p);

    ast_node_t* otherwise = NULL;
    if(parser_match(p, ELSE_KEYWORD)) {
        otherwise = parse_statement(p);
    }

    return new_if_stmt(condition, then, otherwise);
}

static inline ast_node_t* parse_statement(parser_t* p) {

    if(parser_match(p, IF_KEYWORD)) {
        return parse_if_statement(p);
    }

    return parse_expr_statement(p);
}

static inline ast_node_t* parse_decl(parser_t* p) {
    if(parser_match(p, VAR_KEYWORD) || parser_match(p, LET_KEYWORD)) {
        return parse_var_declaration(p);
    }

    return parse_statement(p);
}

ast_node_t* parse_program(parser_t* p) {

    ast_node_t* const program = parse_decl(p);
    ast_node_t* curr = program;

    while(PARSER_CURR(p).type != TOK_EOF) {
        ast_node_t* node = parse_decl(p);
        curr->next = node;
        curr = node;
    }

    return program;
}
