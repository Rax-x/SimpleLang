#ifndef _AST_H_
#define _AST_H_

#include "memory.h"
#include "lexer.h"
#include "types.h"

#include <stdbool.h>

typedef enum {
    VARIABLE_DECL_NODE,
    IF_STATEMENT_NODE,
    EXPR_STATEMENT_NODE,
    ASSIGN_EXPR_NODE,
    BINARY_EXPR_NODE,
    UNARY_EXPR_NODE,
    CASTING_EXPR_NODE,
    SUBSCRIPT_EXPR_NODE,    
    VARIABLE_EXPR_NODE,
    INITIALIZER_NODE,
    LITERAL_NODE
} ast_node_kind_t;

typedef struct _ast_node {
    ast_node_kind_t kind;
    const struct _ast_node* next;
} ast_node_t;

typedef struct {
    ast_node_t base;

    token_t name;

    bool is_type_inferred;
    const type_t* type;

    const ast_node_t* rvalue;
} variable_decl_t;

typedef struct {
    ast_node_t base;

    const ast_node_t* condition;
    const ast_node_t* then;
    const ast_node_t* otherwise;
} if_statement_t;

typedef struct {
    ast_node_t base;
    const ast_node_t* expr;
} expr_statement_t;

typedef struct {
    ast_node_t base;

    const ast_node_t* lvalue;
    const ast_node_t* rvalue;
} assign_expr_t;

typedef struct {
    ast_node_t base;

    token_t op;
    const ast_node_t* left;
    const ast_node_t* right;
} binary_expr_t;

typedef struct {
    ast_node_t base;

    token_t op;
    const ast_node_t* right;
} unary_expr_t;

typedef struct {
    ast_node_t base;

    const ast_node_t* expr;
    const type_t* target_type;
} casting_expr_t;

typedef struct {
    ast_node_t base;

    const ast_node_t* lvalue;
    const ast_node_t* index;
} subscript_expr_t;

typedef struct {
    ast_node_t base;
    token_t name;
} variable_expr_t;

typedef struct _initializer {
    ast_node_t base;
    const ast_node_t* init;
} initializer_t;

typedef struct {
    ast_node_t base;

    const type_t* type;
    float value;
} literal_expr_t;

const ast_node_t* make_var_decl(token_t name, const type_t* type, 
                                const ast_node_t* initializer);

const ast_node_t* make_if_stmt(const ast_node_t* condition, 
                               const ast_node_t* then, 
                               const ast_node_t* otherwise);

const ast_node_t* make_expr_stmt(const ast_node_t* expr);
const ast_node_t* make_assign_expr(const ast_node_t* lvalue, const ast_node_t* rvalue);
const ast_node_t* make_binary_expr(token_t op, const ast_node_t* left, const ast_node_t* right);
const ast_node_t* make_unary_expr(token_t op, const ast_node_t* right);
const ast_node_t* make_casting_expr(const ast_node_t* expr, const type_t* target_type);
const ast_node_t* make_subscript_expr(const ast_node_t* lvalue, const ast_node_t* index);
const ast_node_t* make_variable_expr(token_t name);
const ast_node_t* make_initializer(const ast_node_t* init);
const ast_node_t* make_literal_expr(float value, const type_t* type);

void print_ast(const ast_node_t* node);

#endif
