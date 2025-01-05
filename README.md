# SimpleLang

Recently, I started working on a compiler for a statically typed low-level programming language, 
but I've encountered a problem. I don't know how to build type checkers or how they are built.
During the Christmas holidays, I developed this simple frontend to learn how to build type checkers. 
I had never developed one before, so I accepted the challenge, and this is what I came up with.

This frontend is not intended to demonstrate how to develop a parser or a lexer; 
it is not clean, it is not efficient, and it is really basic. 
Memory is managed like arena allocators do, 
and the symbol table is implemented with a linked list. 
*For the scope of this project, this is acceptable.*

### Grammar

```
  program: declaration*

  declaration: variable-decl | statement

  type-expr: ('float' | 'integer' | 'bool') ('[' INTEGER ']')*
  variable-decl: ('let' | 'var') IDENTIFIER type-expr? ('=' expression)? ';'

  statement: if-statement | expression-statement
  if-statement: 'if' expression 'then' statement ('else' statement)? 
  expression-statement: expression ';'

  expression: assignment
  assignment: unary '=' assignment | comparison
  comparison: term (('>' | '<' | '>=' | '<=') term)*
  term: factor (('+' | '-') factor)*
  factor: unary (('*' | '/') unary)*
  unary: ('-' | '+') unary | subscipt
  subscript: primary('[' expression ']')*
  primary: INTEGER | FLOATING_POINT | 'false' | 'true' | '(' expression ')'

```

### Example

```js

let PI = 3.14; # type inference
var myBooleanValue bool = true;

var matrix integer[3][3];

matrix[0][0] = 1;
matrix[0][1] = 2;
matrix[0][2] = 3;

matrix[1][0] = 4;
matrix[1][1] = 5;
matrix[1][2] = 6;

matrix[2][0] = 7;
matrix[2][1] = 8;
matrix[2][2] = 9;

if matrix[2][2] <= 9.1 then
   matrix[2][2] = 9.2; # Assign error
else
   matrix[2][2] = -1;


```
