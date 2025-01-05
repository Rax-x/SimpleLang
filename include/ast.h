#ifndef _AST_H_
#define _AST_H_

#include "memory.h"
#include "lexer.h"

#include <stdbool.h>

typedef enum _ast_node_kind {
    VARIABLE_DECL_NODE,
    IF_STATEMENT_NODE,
    EXPR_STATEMENT_NODE,
    ASSIGN_EXPR_NODE,
    BINARY_EXPR_NODE,
    UNARY_EXPR_NODE,
    SUBSCRIPT_EXPR_NODE,
    VARIABLE_EXPR_NODE,
    LITERAL_NODE,
    TYPE_EXPR_NODE
} ast_node_kind_t;

typedef struct _ast_node {
    ast_node_kind_t kind;
    struct _ast_node* next;
} ast_node_t;

typedef enum _type_expr_kind {
    TYPE_EXPR_SIMPLE,
    TYPE_EXPR_ARRAY
} type_expr_kind_t;

typedef struct _type_expr {
    ast_node_t base;
    type_expr_kind_t kind;
} type_expr_t;

typedef struct _simple_type_expr {
    type_expr_t base;
    token_t name;
} simple_type_expr_t;

typedef struct _array_type_expr {
    type_expr_t base;
    ast_node_t* underlying;
    unsigned int length;
} array_type_expr_t;

typedef struct _variable_decl {
    ast_node_t base;

    token_t name;
    struct _ast_node* type;
    struct _ast_node* rvalue;
} variable_decl_t;

typedef struct _if_statement {
    ast_node_t base;

    struct _ast_node* condition;
    struct _ast_node* then;
    struct _ast_node* otherwise;
} if_statement_t;

typedef struct _expr_statement {
    ast_node_t base;
    struct _ast_node* expr;
} expr_statement_t;

typedef struct _assign_expr {
    ast_node_t base;

    struct _ast_node* lvalue;
    struct _ast_node* rvalue;
} assign_expr_t;

typedef struct _binary_expr {
    ast_node_t base;

    token_t op;
    struct _ast_node* left;
    struct _ast_node* right;
} binary_expr_t;

typedef struct _unary_expr {
    ast_node_t base;

    token_t op;
    struct _ast_node* right;
} unary_expr_t;

typedef struct _subscript_expr {
    ast_node_t base;

    struct _ast_node* lvalue;
    struct _ast_node* index;
} subscript_expr_t;

typedef struct _variable_expr {
    ast_node_t base;
    token_t name;
} variable_expr_t;

typedef struct _literal_expr {
    ast_node_t base;

    bool is_integer;
    bool is_boolean;
    float value;
} literal_expr_t;

ast_node_t* new_simple_type_expr(token_t name);
ast_node_t* new_array_type_expr(ast_node_t* underlying, unsigned int length);

ast_node_t* new_var_decl(token_t name, ast_node_t* type, ast_node_t* initializer);
ast_node_t* new_if_stmt(ast_node_t* condition, ast_node_t* then, ast_node_t* otherwise);
ast_node_t* new_expr_stmt(ast_node_t* expr);
ast_node_t* new_assign_expr(ast_node_t* lvalue, ast_node_t* rvalue);
ast_node_t* new_binary_expr(token_t op, ast_node_t* left, ast_node_t* right);
ast_node_t* new_unary_expr(token_t op, ast_node_t* right);
ast_node_t* new_subscript_expr(ast_node_t* lvalue, struct _ast_node* index);
ast_node_t* new_variable_expr(token_t name);
ast_node_t* new_literal_expr(float value, bool is_integer, bool is_bool);

void print_ast(ast_node_t* node, int level);

#endif
