#include "../include/typechecker.h"

#include <setjmp.h>
#include <stdio.h>

#define TYPECHECK_ERROR(tcheck, msg) do {               \
        fprintf(stderr, msg);                           \
        longjmp((tcheck)->error, TYPECHECKER_ERROR);    \
    } while(0)


typechecker_t create_typechecker() {

    symbol_table_t* symtbl = create_symbol_table();

    symbol_table_put(symtbl, new_string_view_from_cstr("integer"), int_type);
    symbol_table_put(symtbl, new_string_view_from_cstr("float"), float_type);
    symbol_table_put(symtbl, new_string_view_from_cstr("bool"), bool_type);

    return (typechecker_t) {
        .symtbl = symtbl,
        .status = TYPECHECKER_OK
    };
}

static const type_t* typecheck_node(const ast_node_t* node, typechecker_t* tcheck) {

    switch(node->kind) {
        case VARIABLE_DECL_NODE: {
            const variable_decl_t* const decl = (variable_decl_t*)node;

            const bool is_type_inferred = (decl->type == NULL);
            
            const type_t* t = !is_type_inferred
                ? typecheck_node(decl->type, tcheck)
                : typecheck_node(decl->rvalue, tcheck);
                
            symbol_table_put(tcheck->symtbl, decl->name.lexeme, t);

            if(decl->rvalue != NULL && !is_type_inferred) {
                if(!can_assign_to(typecheck_node(decl->rvalue, tcheck), t)) {
                    TYPECHECK_ERROR(tcheck, "Variable decl type mismatch!.\n");
                }
            }

            break;
        }
        case IF_STATEMENT_NODE: {
            const if_statement_t* const stmt = (if_statement_t*)node;

            if(!are_types_equal(typecheck_node(stmt->condition, tcheck), bool_type)) {
                TYPECHECK_ERROR(tcheck, "If statement condition isn't boolean!.\n");
            }

            typecheck_node(stmt->then, tcheck);

            if(stmt->otherwise != NULL) {
                typecheck_node(stmt->otherwise, tcheck);
            }

            break;
        }
        case EXPR_STATEMENT_NODE: {
            const expr_statement_t* const stmt = (expr_statement_t*)node;
            return typecheck_node(stmt->expr, tcheck);
        }
        case ASSIGN_EXPR_NODE: {

            const assign_expr_t* const expr = (assign_expr_t*)node;

            const type_t* left = typecheck_node(expr->lvalue, tcheck);
            const type_t* right = typecheck_node(expr->rvalue, tcheck);

            if(!can_assign_to(right, left)) {
                TYPECHECK_ERROR(tcheck, "Assign type mismatch!\n");
            }

            return left;
        }
        case BINARY_EXPR_NODE: {

            const binary_expr_t* const expr = (binary_expr_t*)node;

            const type_t* left = typecheck_node(expr->left, tcheck);
            const type_t* right = typecheck_node(expr->right, tcheck);

            if(!IS_NUMERIC_TYPE(left) || !IS_NUMERIC_TYPE(right)) {
                TYPECHECK_ERROR(tcheck, "Expect numbers.\n");
            }

            switch(expr->op.type) {
                case PLUS:
                case MINUS:
                case STAR:
                case SLASH:
                    return cast_to_bigger(left, right);
                case LESS:
                case GREATER:
                case GREATER_EQ:
                case LESS_EQ:
                    return bool_type;
                default:
                    break;
            }

            break;
        }
        case UNARY_EXPR_NODE: {

            const unary_expr_t* const expr = (unary_expr_t*)node;

            const type_t* right = typecheck_node(expr->right, tcheck);

            if(!IS_NUMERIC_TYPE(right)) {
                TYPECHECK_ERROR(tcheck, "Expect a number.\n");
            }

            return right;
        }
        case SUBSCRIPT_EXPR_NODE: {

            const subscript_expr_t* const expr = (subscript_expr_t*)node;

            const type_t* type = typecheck_node(expr->lvalue, tcheck);

            if(type->kind != TYPE_ARRAY) {
                TYPECHECK_ERROR(tcheck, "Subscript expression expect an array type.\n");
            }

            if(!are_types_equal(typecheck_node(expr->index, tcheck), int_type)) {
                TYPECHECK_ERROR(tcheck, "Subscript expression expect integer as index.\n");
            }

            return ((array_type_t*)type)->underlying;
        }
        case VARIABLE_EXPR_NODE: {

            const variable_expr_t* const var = (variable_expr_t*)node;
            return symbol_table_search(tcheck->symtbl, var->name.lexeme);
        }
        case LITERAL_NODE: {

            const literal_expr_t* const lit = (literal_expr_t*)node;

            if(lit->is_boolean) return bool_type;
            if(lit->is_integer) return int_type;

            return float_type;
        }
        case TYPE_EXPR_NODE: {
            const type_expr_t* const texpr = (type_expr_t*)node;


            if(texpr->kind == TYPE_EXPR_SIMPLE) {
                const simple_type_expr_t* const simple = (simple_type_expr_t*) texpr;
                return symbol_table_search(tcheck->symtbl, simple->name.lexeme);
            } 

            const array_type_expr_t* const array = (array_type_expr_t*) texpr;

            return create_array_type(typecheck_node(array->underlying, tcheck), 
                                     array->length);
        }
        default:
            break;
    }

    return NULL;
}

bool typecheck_ast(ast_node_t* ast, typechecker_t* tcheck) {

    if(setjmp(tcheck->error) == 0) {
        for(const ast_node_t* it = ast; it != NULL; it = it->next){
            typecheck_node(it, tcheck);
        }
    } else {
        tcheck->status = TYPECHECKER_ERROR;
    }

    return tcheck->status != TYPECHECKER_ERROR;
}
