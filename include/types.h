#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdbool.h>

typedef enum _type_kind {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_ARRAY,
} type_kind_t;

typedef struct _type {
    type_kind_t kind;

    // For array data type
    int length;
    const struct _type* underlying;
} type_t;

#define IS_ARRAY(t) ((t)->kind == TYPE_ARRAY)
#define IS_NUMERIC_TYPE(t) ((t)->kind == TYPE_INT || (t)->kind == TYPE_FLOAT)

extern type_t* float_type;
extern type_t* int_type;
extern type_t* bool_type;

type_t* create_array_type(const type_t* underlying, int length);

bool are_types_equal(const type_t* t1, const type_t* t2);
bool can_cast_to(const type_t* from, const type_t* to);

const type_t* cast_to_bigger(const type_t* t1, const type_t* t2);

void print_type(const type_t* restrict t);

#endif
