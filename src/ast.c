#include "../include/ast.h"
#include "../include/memory.h"

#include <stdio.h>

inline ast_node_t* new_simple_type_expr(token_t name) {
   simple_type_expr_t* node = MALLOC(simple_type_expr_t*, sizeof(simple_type_expr_t));

    node->base.base.kind = TYPE_EXPR_NODE;
    node->base.kind = TYPE_EXPR_SIMPLE;
    node->name = name;

    return (ast_node_t*)node;
}

inline ast_node_t* new_array_type_expr(ast_node_t* underlying, unsigned int length) {

   array_type_expr_t* node = MALLOC(array_type_expr_t*, sizeof(array_type_expr_t));

    node->base.base.kind = TYPE_EXPR_NODE;
    node->base.kind = TYPE_EXPR_ARRAY;

    node->length = length;
    node->underlying = underlying;

    return (ast_node_t*)node;
}

inline ast_node_t* new_var_decl(token_t name, ast_node_t* type, ast_node_t* initializer) {
    variable_decl_t* node = MALLOC(variable_decl_t*, sizeof(variable_decl_t));

    node->base.kind = VARIABLE_DECL_NODE;
    node->name = name;
    node->type = type;
    node->rvalue = initializer;

    return (ast_node_t*)node;
}

inline ast_node_t* new_if_stmt(ast_node_t* condition, ast_node_t* then, ast_node_t* otherwise) {
    if_statement_t* node = MALLOC(if_statement_t*, sizeof(if_statement_t));

    node->base.kind = IF_STATEMENT_NODE;
    node->condition = condition;
    node->then = then;
    node->otherwise = otherwise;

    return (ast_node_t*)node;
}

inline ast_node_t* new_expr_stmt(ast_node_t* expr) {
    expr_statement_t* node = MALLOC(expr_statement_t*, sizeof(expr_statement_t));

    node->base.kind = EXPR_STATEMENT_NODE;
    node->expr = expr;

    return (ast_node_t*)node;
}

inline ast_node_t* new_assign_expr(ast_node_t* lvalue, ast_node_t* rvalue) {
    assign_expr_t* node = MALLOC(assign_expr_t*, sizeof(assign_expr_t));

    node->base.kind = ASSIGN_EXPR_NODE;
    node->rvalue = rvalue;
    node->lvalue = lvalue;

    return (ast_node_t*)node;
}

inline ast_node_t* new_binary_expr(token_t op, ast_node_t* left, ast_node_t* right) {
    binary_expr_t* node = MALLOC(binary_expr_t*, sizeof(binary_expr_t));

    node->base.kind = BINARY_EXPR_NODE;
    node->op = op;
    node->right = right;
    node->left = left;

    return (ast_node_t*)node;
}

inline ast_node_t* new_unary_expr(token_t op, ast_node_t* right) {
    unary_expr_t* node = MALLOC(unary_expr_t*, sizeof(unary_expr_t));

    node->base.kind = UNARY_EXPR_NODE;
    node->op = op;
    node->right = right;

    return (ast_node_t*)node;
}

inline ast_node_t* new_subscript_expr(ast_node_t* lvalue, struct _ast_node* index) {
    subscript_expr_t* node = MALLOC(subscript_expr_t*, sizeof(subscript_expr_t));

    node->base.kind = SUBSCRIPT_EXPR_NODE;
    node->index = index;
    node->lvalue = lvalue;

    return (ast_node_t*)node;
}

inline ast_node_t* new_variable_expr(token_t name) {
    variable_expr_t* node = MALLOC(variable_expr_t*, sizeof(variable_expr_t));
    node->base.kind = VARIABLE_EXPR_NODE;
    node->name = name;

    return (ast_node_t*) node;
}


inline ast_node_t* new_literal_expr(float value, bool is_integer, bool is_bool) {
    literal_expr_t* node = MALLOC(literal_expr_t*, sizeof(literal_expr_t));

    node->base.kind = LITERAL_NODE;
    node->is_boolean = is_bool;
    node->is_integer = is_integer;
    node->value = value;

    return (ast_node_t*)node;
}

static inline void print_tab(int level) {
    for(int i = 0; i < level; i++) {
        printf("  ");
    }
}

void print_ast(ast_node_t* node, int level) {
    for(const ast_node_t* it = node; it != NULL; it = it->next){
        putchar('\n');
        print_tab(level);

        switch(it->kind) {
            case VARIABLE_DECL_NODE: {

                const variable_decl_t* const decl = (variable_decl_t*)it;

                printf("variable_decl: "STRING_VIEW_FORMAT, STRING_VIEW_ARG(decl->name.lexeme));
                if(decl->type != NULL) {
                    print_ast(decl->type, level+1);
                }

                print_ast(decl->rvalue, level+1);
                
                break;
            }
            case IF_STATEMENT_NODE: {

                const if_statement_t* const stmt = (if_statement_t*)it;

                printf("if_statement: ");
                print_ast(stmt->condition, level+1);
                print_ast(stmt->then, level+1);
                print_ast(stmt->otherwise, level+1);

                break;
            }
            case EXPR_STATEMENT_NODE: {

                const expr_statement_t* const stmt = (expr_statement_t*)it;

                printf("expr_statement: ");
                print_ast(stmt->expr, level+1);
                break;
            }
            case ASSIGN_EXPR_NODE: {

                const assign_expr_t* const expr = (assign_expr_t*)it;

                printf("assign_expr: ");
                print_ast(expr->lvalue, level+1);
                print_ast(expr->rvalue, level+1);

                break;
            }
            case BINARY_EXPR_NODE: {

                const binary_expr_t* const expr = (binary_expr_t*)it;

                printf("binary_expr: "STRING_VIEW_FORMAT, STRING_VIEW_ARG(expr->op.lexeme));
                print_ast(expr->left, level+1);
                print_ast(expr->right, level+1);

                break;
            }
            case UNARY_EXPR_NODE: {

                const unary_expr_t* const expr = (unary_expr_t*)it;

                printf("unary_expr: "STRING_VIEW_FORMAT, STRING_VIEW_ARG(expr->op.lexeme));
                print_ast(expr->right, level+1);

                break;
            }
            case SUBSCRIPT_EXPR_NODE: {

                const subscript_expr_t* const expr = (subscript_expr_t*)it;

                printf("subscript_expr: ");
                print_ast(expr->lvalue, level+1);
                print_ast(expr->index, level+1);

                break;
            }
            case VARIABLE_EXPR_NODE: {

                const variable_expr_t* const var = (variable_expr_t*)it;

                printf("variable_expr: "STRING_VIEW_FORMAT, 
                       STRING_VIEW_ARG(var->name.lexeme));

                break;
            }
            case LITERAL_NODE: {

                const literal_expr_t* const lit = (literal_expr_t*)it;

                printf("literal_expr: %g ", lit->value);
                
                if(lit->is_integer) {
                    printf("(integer)");
                } else if(lit->is_boolean) {
                    printf("(boolean)");
                } else {
                    printf("(floating-point)");
                }

                break;
            }
            case TYPE_EXPR_NODE: {

                const type_expr_t* const texpr = (type_expr_t*)it;

                if(texpr->kind == TYPE_EXPR_SIMPLE) {
                    const simple_type_expr_t* const simple = (simple_type_expr_t*) texpr;
                    printf("simple_type_expr: "STRING_VIEW_FORMAT, 
                           STRING_VIEW_ARG(simple->name.lexeme));
                } else {
                    const array_type_expr_t* const array = (array_type_expr_t*) texpr;
                    printf("array_type_expr: [%u]", array->length);
                    print_ast(array->underlying, level + 1);
                }

                break;
            }
            default:
                break;
        }
    }
}
