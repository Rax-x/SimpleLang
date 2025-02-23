#include "../include/types.h"
#include "../include/memory.h"

type_t* float_type = &(type_t) {.kind = TYPE_FLOAT, .length=0, .underlying=NULL};
type_t* int_type = &(type_t) {.kind = TYPE_INT, .length=0, .underlying=NULL};
type_t* bool_type = &(type_t) {.kind = TYPE_BOOL, .length=0, .underlying=NULL};

type_t* create_array_type(const type_t* underlying, int length) {
    type_t* t = MALLOC(type_t*, sizeof(type_t));

    t->kind = TYPE_ARRAY;
    t->underlying = underlying;
    t->length = length;

    return t;
}

bool are_types_equal(const type_t* t1, const type_t* t2) {

    if(t1->kind != t2->kind) return false;
    
    if(t1->kind == TYPE_ARRAY) {
        if(t1->length != t2->length) return false;
        return are_types_equal(t1->underlying, t2->underlying);
    }

    return true;
}

inline bool can_assign_to(const type_t* from, const type_t* to) {
    return !are_types_equal(from, to) 
        ? can_cast_to(from, to)
        : true;
}

bool can_cast_to(const type_t* from, const type_t* to) {

    switch(from->kind) {
        case TYPE_INT:
            return to->kind == TYPE_FLOAT;
        case TYPE_FLOAT:
            return false;
        case TYPE_BOOL:
            return IS_NUMERIC_TYPE(to);
        case TYPE_ARRAY:
            return false;
    }

    return true; // unrechable
}

inline const type_t* cast_to_bigger(const type_t* t1, const type_t* t2) {
    if(are_types_equal(t1, t2)) return t1;
    return can_cast_to(t1, t2) ? t2 : t1;
}
