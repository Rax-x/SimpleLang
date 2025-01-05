#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "types.h"
#include "string_view.h"

typedef struct _symbol_entry {
    string_view_t name;
    const type_t* type;

    struct _symbol_entry* next;
} symbol_entry_t;

typedef struct _symbol_table {
    symbol_entry_t* start;
    symbol_entry_t* end;
} symbol_table_t;

symbol_table_t* create_symbol_table();
void symbol_table_put(symbol_table_t* symtbl, string_view_t name, const type_t* type);
const type_t* symbol_table_search(symbol_table_t* symtbl, string_view_t name);

#endif
