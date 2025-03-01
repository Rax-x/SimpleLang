#include "../include/typechecker.h"

#include <stdio.h>
#include <stdarg.h>

typedef enum {
    TCHECK_INVALID_ASSIGNMENT,
    TCHECK_VARIABLE_INIT_ERROR,
    TCHECK_INCOMPATIBLE_TYPES,
    TCHECK_EXPECT_BOOLEAN,
    TCHECK_EXPECT_NUMERIC,
    TCHECK_EXPECT_ARRAY,
    TCHECK_CAST_ERROR,
    TCHECK_INITIALIZER_NOT_UNIFORM,
    TCHECK_EXPECT_VALID_INDEX
} typechecker_error_t;

static const char* typechecker_error_messages[] = {
    [TCHECK_INVALID_ASSIGNMENT] = "Invalid assignment: type mismatch.",
    [TCHECK_VARIABLE_INIT_ERROR] = "Variable initialization error.",
    [TCHECK_INCOMPATIBLE_TYPES] = "Incompatible types.",
    [TCHECK_EXPECT_BOOLEAN] = "Expected a boolean type.",
    [TCHECK_EXPECT_NUMERIC] = "Expected a numeric type.",
    [TCHECK_EXPECT_ARRAY] = "Expected an array type.",
    [TCHECK_CAST_ERROR] = "Invalid cast.",
    [TCHECK_INITIALIZER_NOT_UNIFORM] = "Initializer list elements are not of the same type.",
    [TCHECK_EXPECT_VALID_INDEX] = "Expected a valid index for array access." 
};

typechecker_t create_typechecker() {

    symbol_table_t* symtbl = create_symbol_table();

    symbol_table_put(symtbl, new_string_view_from_cstr("integer"), int_type);
    symbol_table_put(symtbl, new_string_view_from_cstr("float"), float_type);
    symbol_table_put(symtbl, new_string_view_from_cstr("bool"), bool_type);

    return (typechecker_t) {
        .symtbl = symtbl,
        .current = NULL,
        .had_error = false
    };
}

static inline void typechecker_error(typechecker_t *tcheck, typechecker_error_t err) {
    if(!tcheck->had_error) {
        tcheck->had_error = true;
    }

    fprintf(stderr, "typechecker: %s\n", typechecker_error_messages[err]);
}

#define SET_RESULT_TYPE(tcheck, result) ((tcheck)->current = result)
#define GET_TYPE_OF(expr, tcheck) (typecheck_node((expr), (tcheck)), (tcheck)->current)

static void typecheck_node(const ast_node_t* node, typechecker_t* tcheck) {

    switch(node->kind) {
        case VARIABLE_DECL_NODE: {
            const variable_decl_t* const decl = (variable_decl_t*)node;
            
            const type_t* t = decl->is_type_inferred
                ? GET_TYPE_OF(decl->rvalue, tcheck)
                : decl->type;
                
            symbol_table_put(tcheck->symtbl, decl->name.lexeme, t);

            if(decl->rvalue != NULL && !decl->is_type_inferred) {
                if(!are_types_equal(GET_TYPE_OF(decl->rvalue, tcheck), t)) {
                    typechecker_error(tcheck, TCHECK_VARIABLE_INIT_ERROR);
                    return;
                }
            }

            break;
        }
        case IF_STATEMENT_NODE: {
            const if_statement_t* const stmt = (if_statement_t*)node;

            if(!are_types_equal(GET_TYPE_OF(stmt->condition, tcheck), bool_type)) {
                typechecker_error(tcheck, TCHECK_EXPECT_BOOLEAN);
                return;
            }

            GET_TYPE_OF(stmt->then, tcheck);

            if(stmt->otherwise != NULL) {
                GET_TYPE_OF(stmt->otherwise, tcheck);
            }

            break;
        }
        case EXPR_STATEMENT_NODE: {
            const expr_statement_t* const stmt = (expr_statement_t*)node;
            SET_RESULT_TYPE(tcheck, GET_TYPE_OF(stmt->expr, tcheck));
            break;
        }
        case ASSIGN_EXPR_NODE: {

            const assign_expr_t* const expr = (assign_expr_t*)node;

            const type_t* left = GET_TYPE_OF(expr->lvalue, tcheck);
            const type_t* right = GET_TYPE_OF(expr->rvalue, tcheck);

            if(!are_types_equal(right, left)) {
                typechecker_error(tcheck, TCHECK_INVALID_ASSIGNMENT);
                return;
            }

            SET_RESULT_TYPE(tcheck, left);
            break;
        }
        case BINARY_EXPR_NODE: {

            const binary_expr_t* const expr = (binary_expr_t*)node;

            const type_t* left = GET_TYPE_OF(expr->left, tcheck);
            const type_t* right = GET_TYPE_OF(expr->right, tcheck);

            if(!IS_NUMERIC_TYPE(left) || !IS_NUMERIC_TYPE(right)) {
                typechecker_error(tcheck, TCHECK_EXPECT_NUMERIC);
                return;
            }

            switch(expr->op.type) {
                case PLUS:
                case MINUS:
                case STAR:
                case SLASH: {
                    const type_t* result = cast_to_bigger(left, right);
                    if(result == NULL) {
                        typechecker_error(tcheck, TCHECK_CAST_ERROR);
                        return;
                    }

                    SET_RESULT_TYPE(tcheck, result);
                    break;
                }
                case LESS:
                case GREATER:
                case GREATER_EQ:
                case LESS_EQ:
                    SET_RESULT_TYPE(tcheck, bool_type);
                    break;
                default:
                    break;
            }
            
            break;
        }
        case UNARY_EXPR_NODE: {

            const unary_expr_t* const expr = (unary_expr_t*)node;

            const type_t* right = GET_TYPE_OF(expr->right, tcheck);

            if(!IS_NUMERIC_TYPE(right)) {
                typechecker_error(tcheck, TCHECK_EXPECT_NUMERIC);
                return;
            }

            SET_RESULT_TYPE(tcheck, right);
            break;
        }
        case CASTING_EXPR_NODE: {

            const casting_expr_t* const expr = (casting_expr_t*)node;

            const type_t* const expr_type = GET_TYPE_OF(expr->expr, tcheck);
            if (!can_cast_to(expr_type, expr->target_type)) {
                typechecker_error(tcheck, TCHECK_CAST_ERROR);
                return;
            }

            SET_RESULT_TYPE(tcheck, expr->target_type);
            break;
        }
        case SUBSCRIPT_EXPR_NODE: {

            const subscript_expr_t* const expr = (subscript_expr_t*)node;

            const type_t* type = GET_TYPE_OF(expr->lvalue, tcheck);

            if(type->kind != TYPE_ARRAY) {
                typechecker_error(tcheck, TCHECK_EXPECT_ARRAY);
                return;
            }

            if(!are_types_equal(GET_TYPE_OF(expr->index, tcheck), int_type)) {
                typechecker_error(tcheck, TCHECK_EXPECT_VALID_INDEX);
                return;
            }

            SET_RESULT_TYPE(tcheck, type->underlying);
            break;
        }
        case VARIABLE_EXPR_NODE: {

            const variable_expr_t* const var = (variable_expr_t*)node;
            const type_t* var_type = symbol_table_search(tcheck->symtbl, var->name.lexeme);

            SET_RESULT_TYPE(tcheck, var_type);
            break;
        }
        case INITIALIZER_NODE: {

            const initializer_t* const initializer = (initializer_t*)node;

            const type_t* prev = NULL;
            int count = 0;
            for(const ast_node_t* it = initializer->init; it != NULL; it = it->next, count++) {
                const type_t* type = GET_TYPE_OF(it, tcheck);

                if(prev != NULL && !are_types_equal(prev, type)) {
                    typechecker_error(tcheck, TCHECK_INITIALIZER_NOT_UNIFORM);
                    return;
                }

                prev = type;
            }

            prev = create_array_type(prev, count);
            
            SET_RESULT_TYPE(tcheck, prev);
            break;
        }
        case LITERAL_NODE: {

            const literal_expr_t* const lit = (literal_expr_t*)node;
            SET_RESULT_TYPE(tcheck, lit->type);
            break;
        }
    }
}

bool typecheck_ast(const ast_node_t* ast, typechecker_t* tcheck) {

    for (const ast_node_t *it = ast; it != NULL; it = it->next){
        typecheck_node(it, tcheck);
    }

    return !tcheck->had_error;
}
