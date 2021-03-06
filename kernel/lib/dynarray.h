#ifndef __LIB__DYNARRAY_H__
#define __LIB__DYNARRAY_H__

#include <stddef.h>
#include <stdbool.h>
#include <lib/alloc.h>
#include <lib/builtins.h>
#include <lib/types.h>

#define DYNARRAY_STRUCT(TYPE) \
    struct {                  \
        TYPE  *storage;       \
        size_t storage_size;  \
        size_t length;        \
    }

#define DYNARRAY_EXTERN(TYPE, THIS) \
    extern DYNARRAY_STRUCT(TYPE) THIS

#define DYNARRAY_GLOBAL(THIS) \
    typeof(THIS) THIS = {0}

#define DYNARRAY_STATIC(TYPE, THIS) \
    static DYNARRAY_STRUCT(TYPE) THIS = {0}

#define DYNARRAY_NEW(TYPE, THIS) \
    DYNARRAY_STRUCT(TYPE) THIS = {0}

#define DYNARRAY_INIT(THIS, INITIAL_SIZE) ({                                       \
    (THIS).storage_size = INITIAL_SIZE;                                            \
    (THIS).storage = alloc((THIS).storage_size * sizeof(typeof(*(THIS).storage))); \
})

#define DYNARRAY_DEL(THIS) ({ \
    free((THIS).storage);     \
})

#define DYNARRAY_GROW(THIS) ({                                                           \
    if ((THIS).storage == NULL) {                                                        \
        DYNARRAY_INIT(THIS, 1);                                                          \
    } else {                                                                             \
        (THIS).storage_size *= 2;                                                        \
        (THIS).storage = realloc((THIS).storage,                                         \
                                 (THIS).storage_size * sizeof(typeof(*(THIS).storage))); \
    }                                                                                    \
})

#define DYNARRAY_PUSHBACK(THIS, ITEM) ({      \
    if ((THIS).length >= (THIS).storage_size) \
        DYNARRAY_GROW(THIS);                  \
    (THIS).storage[(THIS).length++] = ITEM;   \
    (THIS).length - 1;                        \
})

#define DYNARRAY_INSERT(THIS, ITEM) ({     \
    bool found = false;                    \
    size_t i;                              \
    for (i = 0; i < (THIS).length; i++) {  \
        if ((THIS).storage[i] == NULL) {   \
            (THIS).storage[i] = ITEM;      \
            found = true;                  \
            break;                         \
        }                                  \
    }                                      \
    if (found == false) {                  \
        i = DYNARRAY_PUSHBACK(THIS, ITEM); \
    }                                      \
    i;                                     \
})

#define DYNARRAY_INSERT_AT(THIS, ITEM, AT) ({         \
    if ((AT) >= (THIS).storage_size) {                \
        (THIS).storage_size = (AT) + 1;               \
        (THIS).storage =                              \
            realloc((THIS).storage,                   \
                    (THIS).storage_size               \
                  * sizeof(typeof(*(THIS).storage))); \
    }                                                 \
    (THIS).storage[(AT)] = (ITEM);                    \
    if ((AT) >= (THIS).length)                        \
        (THIS).length = (AT) + 1;                     \
    (AT);                                             \
})

#define DYNARRAY_INSERT_LEAST(THIS, ITEM, AT) ({ \
    bool found = false;                          \
    size_t i;                                    \
    if ((AT) >= (THIS).length) {                 \
        i = DYNARRAY_INSERT_AT(THIS, ITEM, AT);  \
    } else {                                     \
        for (i = (AT); i < (THIS).length; i++) { \
            if ((THIS).storage[i] == NULL) {     \
                (THIS).storage[i] = ITEM;        \
                found = true;                    \
                break;                           \
            }                                    \
        }                                        \
        if (found == false) {                    \
            i = DYNARRAY_PUSHBACK(THIS, ITEM);   \
        }                                        \
    }                                            \
    i;                                           \
})

#define DYNARRAY_GET_INDEX_BY_VALUE(THIS, VALUE) ({ \
    ssize_t i;                                      \
    for (i = 0; i < (ssize_t)(THIS).length; i++) {  \
        if ((THIS).storage[i] == (VALUE))           \
            break;                                  \
    }                                               \
    if (i == (ssize_t)((THIS).length))              \
        i = -1;                                     \
    i;                                              \
})

#define DYNARRAY_REMOVE_BY_VALUE(THIS, VALUE) ({   \
    ssize_t i;                                     \
    for (i = 0; i < (ssize_t)(THIS).length; i++) { \
        if ((THIS).storage[i] == (VALUE)) {        \
            (THIS).storage[i] = NULL;              \
            break;                                 \
        }                                          \
    }                                              \
    if (i == (ssize_t)((THIS).length))             \
        i = -1;                                    \
    i;                                             \
})

#define DYNARRAY_REMOVE_BY_VALUE_AND_PACK(THIS, VALUE) ({    \
    ssize_t i;                                               \
    for (i = 0; i < (ssize_t)(THIS).length; i++) {           \
        if ((THIS).storage[i] == (VALUE)) {                  \
            for (size_t j = i + 1; j < (THIS).length; j++) { \
                (THIS).storage[j-1] = (THIS).storage[j];     \
            }                                                \
            break;                                           \
        }                                                    \
    }                                                        \
    if (i == (ssize_t)((THIS).length))                       \
        i = -1;                                              \
    i;                                                       \
})

#define DYNARRAY_REMOVE_AND_PACK(THIS, INDEX) ({           \
    for (size_t i = (INDEX) + 1; i < (THIS).length; i++) { \
        (THIS).storage[i-1] = (THIS).storage[i];           \
    }                                                      \
    --(THIS).length;                                       \
})

#endif
