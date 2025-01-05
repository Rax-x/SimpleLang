#include <stdio.h>
#include <stdlib.h>

#include "../include/ast.h"
#include "../include/parser.h"
#include "../include/memory.h"
#include "../include/typechecker.h"


/*
  program: declaration*

  declaration: variable-decl | statement

  type-expr: ('float' | 'integer' | 'bool') ('[' INTEGER ']')*
  variable-decl: ('let' | 'var') IDENTIFIER type-expr? ('=' expression)? ';'

  statement: if-statement | expression-statement
  if-statement: 'if' expression 'then' statement ('else' statement)? 
  expression-statement: expression ';'

  expression: assignmentn
  assignment: unary '=' assignment | comparison
  comparison: term (('>' | '<' | '>=' | '<=') term)*
  term: factor (('+' | '-') factor)*
  factor: unary (('*' | '/') unary)*
  unary: ('-' | '+') unary | subscipt
  subscript: primary('[' expression ']')*
  primary: INTEGER | FLOATING_POINT | 'false' | 'true' | '(' expression ')'
  
*/

static char* read_from_file(const char* const restrict file) {

    FILE* const stream = fopen(file, "r");

    fseek(stream, 0, SEEK_END);
    size_t size = ftell(stream);
    rewind(stream);

    char* const buffer = MALLOC(char*, size + 1);
    fread(buffer, sizeof(char), size, stream);
    buffer[size] = '\0';

    fclose(stream);
    return buffer;
}

int main(int argc, char** argv) {

    if(argc < 2) {
        fprintf(stderr, "%s [file]\n", *argv);
        exit(EXIT_FAILURE);
    }

    atexit(free_all);

    const char* const buffer = read_from_file(argv[1]);

    parser_t p = init_parser(new_string_view_from_cstr(buffer));
    ast_node_t* program = parse_program(&p);

    print_ast(program, 0);
    puts("\n");

    typechecker_t tcheck = create_typechecker();
    if(typecheck_ast(program, &tcheck)) {
        puts("The type check was successful.");
    }

    return 0;
}

