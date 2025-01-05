#include "../include/types.h"
#include "../include/memory.h"

type_t* float_type = &(type_t) {.kind = TYPE_FLOAT, .bytes = sizeof(float) };
type_t* int_type = &(type_t) {.kind = TYPE_INT, .bytes = sizeof(int) };
type_t* bool_type = &(type_t) {.kind = TYPE_BOOL, .bytes = sizeof(bool) };

type_t* create_array_type(const type_t* underlying, int length) {
    array_type_t* t = MALLOC(array_type_t*, sizeof(array_type_t));

    t->base.kind = TYPE_ARRAY;
    t->base.bytes = underlying->bytes * length;
    t->underlying = underlying;
    t->length = length;

    return (type_t*)t;
}

bool are_types_equal(const type_t* t1, const type_t* t2) {

    if(t1->kind != t2->kind) return false;
    if(t2->bytes != t1->bytes) return false;
    
    if(t1->kind == TYPE_ARRAY) {
        const array_type_t* atype1 = (array_type_t*)t1;
        const array_type_t* atype2 = (array_type_t*)t1;

        if(atype1->length != atype2->length) return false;

        return are_types_equal(atype1->underlying, atype2->underlying);
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
