#include "../include/symbol_table.h"
#include "../include/memory.h"

inline symbol_table_t* create_symbol_table() {
    symbol_table_t* symtbl = MALLOC(symbol_table_t*, sizeof(symbol_table_t));
    symtbl->end = symtbl->start = NULL;

    return symtbl;
}

void symbol_table_put(symbol_table_t* symtbl, string_view_t name, const type_t* type) {
    symbol_entry_t* entry = MALLOC(symbol_entry_t*, sizeof(symbol_entry_t));
    
    entry->name = name;
    entry->type = type;
    entry->next = NULL;

    if(symtbl->start == NULL) {
        symtbl->start = entry;
        symtbl->end = symtbl->start;
    } else {
        symtbl->end->next = entry;
        symtbl->end = symtbl->end->next;
    }
}

const type_t* symbol_table_search(symbol_table_t* symtbl, string_view_t name) {
    for(const symbol_entry_t* it = symtbl->start; it != NULL; it = it->next) {
        if(string_view_equal(it->name, name)) {
            return it->type;
        }
    }

    return NULL;
}
