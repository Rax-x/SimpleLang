#ifndef _TYPE_CHECKER_H_
#define _TYPE_CHECKER_H_

#include "symbol_table.h"
#include "ast.h"

#include <setjmp.h>
#include <stdbool.h>

#define TYPECHECKER_OK 0
#define TYPECHECKER_ERROR 1

typedef struct _typechecker {
    symbol_table_t* symtbl;
    
    jmp_buf error;
    int status;
} typechecker_t;

typechecker_t create_typechecker();
bool typecheck_ast(ast_node_t* ast, typechecker_t* tcheck);

#endif
