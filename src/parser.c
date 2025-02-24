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

static const ast_node_t* parse_expression(parser_t* p);
static const ast_node_t* parse_statement(parser_t* p);

static const ast_node_t* parse_primary(parser_t* p) {

    if(parser_match(p, INTEGER_LITERAL) || parser_match(p, FLOATING_LITERAL)) {

        const float value = strtof(string_view_data(PARSER_PREV(p).lexeme), NULL);
        const type_t* type = PARSER_PREV(p).type == FLOATING_LITERAL
            ? float_type
            : int_type;

        return make_literal_expr(value, type);
    }


    if(parser_match(p, TRUE_KEYWORD) || parser_match(p, FALSE_KEYWORD)) {
        const float value = PARSER_PREV(p).type == TRUE_KEYWORD;
        return make_literal_expr(value, bool_type);
    }

    if(parser_match(p, LEFT_PAREN)) {
        const ast_node_t* const expr = parse_expression(p);
        parser_consume(p, RIGHT_PAREN);

        return expr;
    }

    if(parser_match(p, IDENTIFIER)) {
        return make_variable_expr(PARSER_PREV(p));
    }

    fprintf(stderr, "[Ln: %d] Unknown expression.\n", PARSER_CURR(p).line);
    exit(EXIT_FAILURE);
}

static const ast_node_t* parse_subscript(parser_t* p) {
    const ast_node_t* expr = parse_primary(p);

    while(parser_match(p, LEFT_BRACKET)) {
        expr = make_subscript_expr(expr, parse_expression(p));
        parser_consume(p, RIGHT_BRACKET);
    }

    return expr;
}

static const ast_node_t* parse_unary(parser_t* p) {

    if(parser_match(p, MINUS) || parser_match(p, PLUS)) {
        token_t op = PARSER_PREV(p);
        return make_unary_expr(op, parse_unary(p));
    }
    
    return parse_subscript(p);
}

static const type_t* parse_type_suffix(parser_t* p, const type_t* type) {

    if(!parser_match(p, LEFT_BRACKET))  {
        return type;
    }

    token_t literal = parser_consume(p, INTEGER_LITERAL);
    unsigned int length = strtol(string_view_data(literal.lexeme), NULL, 10);
    parser_consume(p, RIGHT_BRACKET);

    return create_array_type(parse_type_suffix(p, type), length);
}

static const type_t* parse_type(parser_t* p) {

    const type_t* type = NULL;

    if(parser_match(p, FLOAT_KEYWORD)) {
        type = float_type;
    } else if(parser_match(p, INTEGER_KEYWORD)) {
        type = int_type;
    } else if(parser_match(p, BOOL_KEYWORD)) {
        type = bool_type;
    } else {
        fprintf(stderr, "[Ln: %d] Unknown data type.\n", PARSER_CURR(p).line);
        exit(EXIT_FAILURE);
    }

    return parse_type_suffix(p, type);
}

static const ast_node_t* parse_casting(parser_t* p) {

    const ast_node_t* left = parse_unary(p);

    while(parser_match(p, AS_KEYWORD)) {
        const type_t* const type = parse_type(p);
        left = make_casting_expr(left, type);
    }

    return left;
}

static const ast_node_t* parse_factor(parser_t* p) {
    const ast_node_t* left = parse_casting(p);

    while(parser_match(p, STAR) || parser_match(p, SLASH)) {
        token_t op = PARSER_PREV(p);
        left = make_binary_expr(op, left, parse_casting(p));
    }

    return left;
}

static const ast_node_t* parse_term(parser_t* p) {
    const ast_node_t* left = parse_factor(p);

    while(parser_match(p, PLUS) || parser_match(p, MINUS)) {
        token_t op = PARSER_PREV(p);
        left = make_binary_expr(op, left, parse_factor(p));
    }

    return left;
}

static const ast_node_t* parse_comparison(parser_t* p) {
    const ast_node_t* left = parse_term(p);

    while(parser_match(p, GREATER) || 
          parser_match(p, GREATER_EQ) || 
          parser_match(p, LESS) || 
          parser_match(p, LESS_EQ)) {
        token_t op = PARSER_PREV(p);
        left = make_binary_expr(op, left,parse_term(p));
    }

    return left;
}

static const ast_node_t* parse_assignment(parser_t* p) {

    const ast_node_t* lvalue = parse_comparison(p);

    if(parser_match(p, ASSIGN)) {

        if(lvalue->kind != VARIABLE_EXPR_NODE &&
           lvalue->kind != SUBSCRIPT_EXPR_NODE) {
            fprintf(stderr, "[Ln: %d] Can't assign to an rvalue.\n",
                    PARSER_CURR(p).line);
            exit(EXIT_FAILURE);
        }
    
        return make_assign_expr(lvalue, parse_assignment(p));
    }
    
    return lvalue;
}

static inline const ast_node_t* parse_expression(parser_t* p){
    return parse_assignment(p);
}

static const ast_node_t* parse_initializer(parser_t* p) {

    if(!parser_match(p, LEFT_BRACE)) {
        return parse_expression(p);
    }

    ast_node_t* const initializer = (ast_node_t*) parse_initializer(p);
    for(ast_node_t* curr = initializer; parser_match(p, COMMA); curr = (ast_node_t*) curr->next) {
        curr->next = parse_initializer(p);
    }
    
    parser_consume(p, RIGHT_BRACE);

    return make_initializer(initializer);
}

static const ast_node_t* parse_var_declaration(parser_t* p) {

    const bool is_type_inferred = PARSER_PREV(p).type == LET_KEYWORD;

    token_t name = parser_consume(p, IDENTIFIER);

    const type_t* type = !is_type_inferred 
        ? parse_type(p)
        : NULL;

    const ast_node_t* initializer = NULL;
    if(parser_match(p, ASSIGN)) {
        initializer = parse_initializer(p);
    }

    if(initializer == NULL && is_type_inferred) {
        fprintf(stderr, "[Ln: %d] Variables declared with 'let' must be initialized.\n", name.line);
        exit(EXIT_FAILURE);
    }

    parser_consume(p, SEMICOLON);

    return make_var_decl(name, type, initializer);
}


static inline const ast_node_t* parse_expr_statement(parser_t* p) {
    const ast_node_t* expr = parse_expression(p);
    parser_consume(p, SEMICOLON);

    return make_expr_stmt(expr);
}

static const ast_node_t* parse_if_statement(parser_t* p) {
    const ast_node_t* const condition = parse_expression(p);
    
    parser_consume(p, THEN_KEYWORD);
    const ast_node_t* const then = parse_statement(p);

    const ast_node_t* otherwise = NULL;
    if(parser_match(p, ELSE_KEYWORD)) {
        otherwise = parse_statement(p);
    }

    return make_if_stmt(condition, then, otherwise);
}

static inline const ast_node_t* parse_statement(parser_t* p) {

    if(parser_match(p, IF_KEYWORD)) {
        return parse_if_statement(p);
    }

    return parse_expr_statement(p);
}

static inline const ast_node_t* parse_decl(parser_t* p) {
    if(parser_match(p, VAR_KEYWORD) || parser_match(p, LET_KEYWORD)) {
        return parse_var_declaration(p);
    }

    return parse_statement(p);
}

const ast_node_t* parse_program(parser_t* p) {

    ast_node_t* program = (ast_node_t*) parse_decl(p);
    ast_node_t* curr = program;

    while(PARSER_CURR(p).type != TOK_EOF) {
        ast_node_t* node = (ast_node_t*)parse_decl(p);
        curr->next = node;
        curr = node;
    }

    return program;
}
