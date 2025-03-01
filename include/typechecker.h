#ifndef _TYPE_CHECKER_H_
#define _TYPE_CHECKER_H_

#include "symbol_table.h"
#include "ast.h"

#include <setjmp.h>
#include <stdbool.h>

typedef struct _typechecker {
    symbol_table_t* const symtbl;
    
    const type_t* current;
    bool had_error;
} typechecker_t;

typechecker_t create_typechecker();
bool typecheck_ast(const ast_node_t* ast, typechecker_t* tcheck);

#endif
