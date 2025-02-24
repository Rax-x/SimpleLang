#include "../include/ast.h"
#include "../include/memory.h"

#include <stdio.h>

inline const ast_node_t* make_var_decl(token_t name, const type_t* type, const ast_node_t* initializer) {
    variable_decl_t* const node = MALLOC(variable_decl_t*, sizeof(variable_decl_t));

    node->base.kind = VARIABLE_DECL_NODE;

    node->name = name;
    node->type = type;
    node->rvalue = initializer;
    node->is_type_inferred = (type == NULL);

    return (ast_node_t*)node;
}

inline const ast_node_t* make_if_stmt(const ast_node_t* condition, 
                                      const ast_node_t* then, 
                                      const ast_node_t* otherwise) {
    
    if_statement_t* const node = MALLOC(if_statement_t*, sizeof(if_statement_t));

    node->base.kind = IF_STATEMENT_NODE;

    node->condition = condition;
    node->then = then;
    node->otherwise = otherwise;

    return (ast_node_t*)node;
}

inline const ast_node_t* make_expr_stmt(const ast_node_t* expr) {
    expr_statement_t* const node = MALLOC(expr_statement_t*, sizeof(expr_statement_t));

    node->base.kind = EXPR_STATEMENT_NODE;
    node->expr = expr;

    return (ast_node_t*)node;
}

inline const ast_node_t* make_assign_expr(const ast_node_t* lvalue, const ast_node_t* rvalue) {
    assign_expr_t* const node = MALLOC(assign_expr_t*, sizeof(assign_expr_t));

    node->base.kind = ASSIGN_EXPR_NODE;
    node->rvalue = rvalue;
    node->lvalue = lvalue;

    return (ast_node_t*)node;
}

inline const ast_node_t* make_binary_expr(token_t op, const ast_node_t* left, 
                                          const ast_node_t* right) {

    binary_expr_t* const node = MALLOC(binary_expr_t*, sizeof(binary_expr_t));

    node->base.kind = BINARY_EXPR_NODE;

    node->op = op;
    node->right = right;
    node->left = left;

    return (ast_node_t*)node;
}

inline const ast_node_t* make_unary_expr(token_t op, const ast_node_t* right) {
    unary_expr_t* const node = MALLOC(unary_expr_t*, sizeof(unary_expr_t));

    node->base.kind = UNARY_EXPR_NODE;
    node->op = op;
    node->right = right;

    return (ast_node_t*)node;
}

const ast_node_t* make_casting_expr(const ast_node_t* expr, const type_t* target_type) {
    casting_expr_t* const node = MALLOC(casting_expr_t*, sizeof(casting_expr_t));
    
    node->base.kind = CASTING_EXPR_NODE;
    node->expr = expr;
    node->target_type = target_type;

    return (ast_node_t*) node;
}

inline const ast_node_t* make_subscript_expr(const ast_node_t* lvalue, const ast_node_t* index) {
    subscript_expr_t* const node = MALLOC(subscript_expr_t*, sizeof(subscript_expr_t));

    node->base.kind = SUBSCRIPT_EXPR_NODE;
    node->index = index;
    node->lvalue = lvalue;

    return (ast_node_t*)node;
}

inline const ast_node_t* make_variable_expr(token_t name) {
    variable_expr_t* const node = MALLOC(variable_expr_t*, sizeof(variable_expr_t));
 
    node->base.kind = VARIABLE_EXPR_NODE;
    node->name = name;

    return (ast_node_t*) node;
}

inline const ast_node_t* make_initializer(const ast_node_t* init) {
    initializer_t* const node = MALLOC(initializer_t*, sizeof(initializer_t));

    node->base.kind = INITIALIZER_NODE;
    node->init = init;

    return (ast_node_t*) node;
}

inline const ast_node_t* make_literal_expr(float value, const type_t* type) {
    literal_expr_t* const node = MALLOC(literal_expr_t*, sizeof(literal_expr_t));

    node->base.kind = LITERAL_NODE;
    node->value = value;
    node->type = type;

    return (ast_node_t*)node;
}

// =============== AST Printer ===============

static inline void print_tab(const int level) {
    for(int i = 0; i < level; i++) {
        printf("  ");
    }
}

static void print_ast_node(const ast_node_t* node, int level) {
    
    if(node == NULL) return;

    putchar('\n');
    print_tab(level);

    switch(node->kind) {
        case VARIABLE_DECL_NODE: {

            const variable_decl_t* const decl = (variable_decl_t*)node;

            printf("variable_decl: "STRING_VIEW_FORMAT" ", STRING_VIEW_ARG(decl->name.lexeme));
            if(!decl->is_type_inferred) {
                print_type(decl->type);
            }

            print_ast_node(decl->rvalue, level+1);
            break;
        }
        case IF_STATEMENT_NODE: {

            const if_statement_t* const stmt = (if_statement_t*)node;

            printf("if_statement: ");
            print_ast_node(stmt->condition, level+1);
            print_ast_node(stmt->then, level+1);
            print_ast_node(stmt->otherwise, level+1);

            break;
        }
         case EXPR_STATEMENT_NODE: {

             const expr_statement_t* const stmt = (expr_statement_t*)node;

             printf("expr_statement: ");
             print_ast_node(stmt->expr, level+1);
             break;
         }
         case ASSIGN_EXPR_NODE: {

             const assign_expr_t* const expr = (assign_expr_t*)node;

             printf("assign_expr: ");
             print_ast_node(expr->lvalue, level+1);
             print_ast_node(expr->rvalue, level+1);

             break;
         }
         case BINARY_EXPR_NODE: {

             const binary_expr_t* const expr = (binary_expr_t*)node;

             printf("binary_expr: "STRING_VIEW_FORMAT, STRING_VIEW_ARG(expr->op.lexeme));
             print_ast_node(expr->left, level+1);
             print_ast_node(expr->right, level+1);

             break;
         }
         case UNARY_EXPR_NODE: {

             const unary_expr_t* const expr = (unary_expr_t*)node;

             printf("unary_expr: "STRING_VIEW_FORMAT, STRING_VIEW_ARG(expr->op.lexeme));
             print_ast_node(expr->right, level+1);

             break;
         }
         case CASTING_EXPR_NODE: {
             
             const casting_expr_t* const expr = (casting_expr_t*)node;

             printf("casting_expr: ");
             print_type(expr->target_type);
             print_ast_node(expr->expr, level + 1);

             break;
         }
         case SUBSCRIPT_EXPR_NODE: {

             const subscript_expr_t* const expr = (subscript_expr_t*)node;

             printf("subscript_expr: ");
             print_ast_node(expr->lvalue, level+1);
             print_ast_node(expr->index, level+1);

             break;
         }
         case VARIABLE_EXPR_NODE: {

             const variable_expr_t* const var = (variable_expr_t*)node;

             printf("variable_expr: "STRING_VIEW_FORMAT, 
                    STRING_VIEW_ARG(var->name.lexeme));

             break;
         }
         case INITIALIZER_NODE: {

             const initializer_t* const list = (initializer_t*)node;

             printf("initializer: ");
             for(const ast_node_t* it = list->init; it != NULL; it = it->next) {
                 print_ast_node(it, level+1);
             }

             break;
         }
         case LITERAL_NODE: {

             const literal_expr_t* const lit = (literal_expr_t*)node;

             printf("literal_expr: %g (", lit->value);
             print_type(lit->type);
             putchar(')');

             break;
         }
    }
}

void print_ast(const ast_node_t* node) {
    for(const ast_node_t* it = node; it != NULL; it = it->next){
        print_ast_node(it, 0);
    }
}
