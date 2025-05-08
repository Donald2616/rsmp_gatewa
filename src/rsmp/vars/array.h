/**
* @file string.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-06-28
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_VARS_ARRAY_H
#define RSMP_VARS_ARRAY_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * @defgroup string 
 * 
 * @brief Definition d'une 
 * 
 * @{
 */
#define RSMP_ARRAY_INITIALIZER_FREE(_fct_) {.len=0, item_len=sizeof(*.items), .items=NULL, .free=_fct_};
#define RSMP_ARRAY_INITIALIZER RSMP_ARRAY_INITIALIZER_FREE(NULL)


typedef void(*rsmp_array_item_free_t)(void *);

#define rsmp_array_declare_struct(type) \
\
typedef struct rsmp_array_ ## type \
{\
    size_t len;\
    size_t item_len;\
    type *items;\
    rsmp_array_item_free_t free;\
} rsmp_array_ ## type ## _t; \

/*
// #define rsmp_array_declare_flush(type) 

// #define rsmp_array_declare_flush_free(type) \
// void rsmp_array_ ## type ## _free_item(type *item);\
// static inline void \
//     rsmp_array_ ## type ## _flush(rsmp_array_ ## type ## _t *array)\
//     {\
//         assert(array!=NULL);\
//         if (array->len)\
//         {\
//             for(size_t i=0; i< array->len; i++) rsmp_array_ ## type ## _free_item(&array->items[i]);\
//             if (array->items) free(array->items);\
//             array->len=0;\
//             array->item_len=sizeof(type);\
//             array->items=NULL;\
//         }\
//     }
*/

#define _rsmp_array_declare_declare(type, fct) \
void rsmp_array_ ## type ## _free_item(type *item);\
static inline void \
    rsmp_array_ ## type ## _declare(rsmp_array_ ## type ## _t *array)\
    {\
        assert(array!=NULL);\
        array->len=0;\
        array->item_len=sizeof(type);\
        array->free = (rsmp_array_item_free_t)fct;\
    }\

#define rsmp_array_declare_declare_free(type) \
void rsmp_array_ ## type ## _free_item(type *item);\
_rsmp_array_declare_declare(type, rsmp_array_ ## type ## _free_item)

#define rsmp_array_declare_declare(type) _rsmp_array_declare_declare(type, NULL)


#define rsmp_array_declare_functions(type) \
static inline void \
    rsmp_array_ ## type ## _flush(rsmp_array_ ## type ## _t *array)\
    {\
        assert(array!=NULL);\
        if (array->len)\
        {\
            if (array->free)\
            {\
                for(size_t i=0; i< array->len; i++) \
                {\
                    array->free(&array->items[i]);\
                }\
            }\
            if (array->items) free(array->items);\
            array->len=0;\
            array->items=NULL;\
        }\
    }\
\
typedef int(*rsmp_array_ ## type ## _cmp_t)(type *item1, type *item2);\
\
static inline size_t \
    rsmp_array_ ## type ## _item_sizeof(void)\
    {\
        return sizeof(type);\
    }\
\
static inline void \
    rsmp_array_ ## type ## _initialize(rsmp_array_ ## type ## _t *array, size_t max)\
    {\
        assert(array!=NULL);\
        if (array->len) rsmp_array_ ## type ## _flush(array);\
        if (max>0)\
        {\
            array->len=max;\
            array->items = calloc(array->len, array->item_len);\
            assert(array->items!=NULL);\
        }\
        else \
        {\
            array->items=NULL;\
        }\
    }\
\
static inline rsmp_array_ ## type ## _t * \
    rsmp_array_ ## type ## _new(size_t len)\
    {\
        rsmp_array_ ## type ## _t *array = calloc(len, array->item_len);\
        if (array)\
        {\
            rsmp_array_ ## type ## _declare(array);\
        }\
        return array;\
    }\
\
static inline void \
    rsmp_array_ ## type ## _free(rsmp_array_ ## type ## _t *array)\
    {\
        assert(array!=NULL);\
        rsmp_array_ ## type ## _flush(array);\
        free(array);\
    }\
\
static inline ssize_t \
    rsmp_array_ ## type ##  _extend(rsmp_array_ ## type ##  _t *array, size_t add)\
    {\
        assert(array!=NULL);\
        array->item_len=sizeof(type);\
        size_t newlen = array->len + add;\
        type * items=realloc(array->items, sizeof(*array->items)*newlen);\
        if (items)\
        {\
            memset(&items[array->len], 0, sizeof(*array->items)*add);\
            array->len = newlen;\
            array->items=items;\
        }\
        else\
        {\
            return -1;\
        }\
        return array->len;\
    }\
\
static inline type * \
    rsmp_array_ ## type ##  _new_item(rsmp_array_ ## type ##  _t *array)\
    {\
        assert(array!=NULL);\
        rsmp_array_ ## type ##  _extend(array, 1);\
        return &array->items[array->len-1];\
    }\
\
static inline rsmp_array_ ## type ## _t *\
    rsmp_array_ ## type ##  _shrink(rsmp_array_ ## type ##  _t *array, size_t delta)\
    {\
        assert(array!=NULL);\
        if (delta >= array->len) \
        {\
            array->len = 0;\
            if (array->items)\
            {\
                free(array->items);\
                array->items=NULL;\
            }\
        }\
        type *olds = array->items;\
        array->items = NULL;\
        rsmp_array_ ## type ##  _initialize(array, array->len-delta);\
        memcpy(array->items, olds, array->len*sizeof(*array->items));\
        free(olds);\
        return array;\
    }\
\
static inline size_t \
    rsmp_array_ ## type ## _len(rsmp_array_ ## type ## _t const *array) \
    {\
        assert(array!=NULL);\
        return array->len;\
    }\
\
static inline type *\
    rsmp_array_ ## type ## _get(rsmp_array_ ## type ## _t *array, size_t i)\
    {\
        assert(array!=NULL);\
        return (i < array->len)?array->items + i:NULL;\
    }\
\
static inline rsmp_array_ ## type ## _t * \
    rsmp_array_ ## type ## _set(rsmp_array_ ## type ## _t *array, size_t i, type * item)\
    {\
        assert(array!=NULL);\
        if (i>=array->len) rsmp_array_ ## type ## _extend(array, i-array->len);\
        assert(i<array->len);\
        memcpy(array->items+i, item, sizeof(*array->items));\
        return array;\
    }\
\
static inline rsmp_array_ ## type ## _t * \
    rsmp_array_ ## type ## _cpy_pos(rsmp_array_ ## type ## _t *array1, size_t i1, rsmp_array_ ## type ## _t const *array2, size_t i2, size_t n2)\
    {\
        assert(array1!=NULL);\
        assert(array2!=NULL);\
        if (array2->len == 0|| n2==0) return array1;\
        if (i1>array1->len) i1=array1->len;\
        size_t l = i1+n2;\
        if (l >= array1->len) rsmp_array_ ## type ##  _extend(array1, l-array1->len);\
        memcpy(array1->items+i1, array2->items+i2, n2*sizeof(*array2->items));\
        return array1;\
    }\
\
static inline rsmp_array_ ## type ## _t * \
    rsmp_array_ ## type ## _cpy(rsmp_array_ ## type ## _t *array1, rsmp_array_ ## type ## _t const *array2)\
    {\
        return rsmp_array_ ## type ## _cpy_pos(array1, 0, array2, 0, array2->len);\
    }\
\
\
static inline rsmp_array_ ## type ## _t * \
    rsmp_array_ ## type ## _cat_n(rsmp_array_ ## type ## _t *array1, rsmp_array_ ## type ## _t const *array2, size_t n2)\
    {\
        return rsmp_array_ ## type ## _cpy_pos(array1, array1->len, array2, 0, n2);\
    }\
\
static inline rsmp_array_ ## type ## _t * \
    rsmp_array_ ## type ## _cat(rsmp_array_ ## type ## _t *array1, rsmp_array_ ## type ## _t const *array2)\
    {\
        return rsmp_array_ ## type ## _cat_n(array1, array2, array2->len);\
    }\
\
static inline rsmp_array_ ## type ## _t * \
    rsmp_array_ ## type ## _insert(rsmp_array_ ## type ## _t *array1, size_t i1, rsmp_array_ ## type ## _t const *array2, size_t i2, size_t n2)\
    {\
        assert(array1 != array2);\
        assert(array1!=NULL);\
        assert(array2!=NULL);\
        if (array2->len == 0|| n2==0) return array1;\
        if (i2+n2 > array2->len) n2 = array2->len-i2;\
        size_t max=array1->len-i1;\
        rsmp_array_ ## type ## _extend(array1, n2);\
        for(size_t k=0, j=array1->len-1, i=i1+max-1; k<max; k++,i--,j--)\
        {\
            memcpy(array1->items+j, array1->items+i, sizeof(*array1->items));\
        }\
        return rsmp_array_ ## type ## _cpy_pos(array1, i1, array2, i2, n2);\
    }\
\
static inline int \
    rsmp_array_ ## type ## _cmp(rsmp_array_ ## type ## _t const *array1, rsmp_array_ ## type ## _t const *array2, rsmp_array_ ## type ## _cmp_t cmp_fct)\
    {\
        assert(array1!=NULL);\
        assert(array2!=NULL);\
        assert(cmp_fct!=NULL);\
        if (!array1->len || !array2->len) return (int)array1->len - (int)array2->len;\
        size_t min=array1->len<array2->len?array1->len:array2->len;\
        size_t i=0;\
        int ret=-1;\
        while(i<min)\
        {\
            ret = cmp_fct(array1->items+i, array2->items+i);\
            if (ret) return ret;\
        }\
        return ret;\
    }\

#define rsmp_array_declare_dup(type)\
static inline rsmp_array_ ## type ## _t * \
    rsmp_array_ ## type ## _dup(rsmp_array_ ## type ## _t const *array)\
    {\
        assert(array!=NULL);\
        rsmp_array_ ## type ## _t *_dup = rsmp_array_ ## type ## _new(array->len);\
        if (_dup) rsmp_array_ ## type ## _cpy(_dup, array);\
        return _dup;\
    }\

#define rsmp_array_declare(type) \
rsmp_array_declare_struct(type) \
rsmp_array_declare_declare(type) \
rsmp_array_declare_functions(type)\
rsmp_array_declare_dup(type)

#define rsmp_array_declare_free(type) \
rsmp_array_declare_struct(type) \
rsmp_array_declare_declare_free(type) \
rsmp_array_declare_functions(type)



#define rsmp_array_foreach(array, i) for(size_t i=0; i<array.len; i++)

rsmp_array_declare_struct(void)

static inline void rsmp_array_flush(rsmp_array_void_t *array)
{
    assert(array!=NULL);
    if (array->len)
    {
        if (array->free)
        {
            for(size_t i=0, offset = 0; i< array->len; i++, offset += array->item_len) 
            {
                array->free(array->items + offset);
            }
        }
        if (array->items) free(array->items);
        array->len=0;
        array->items=NULL;
    }
}

static inline void rsmp_array_initialize(rsmp_array_void_t *array, size_t max)
{
    assert(array!=NULL);
    if (array->len) rsmp_array_flush(array);
    array->len=max;
    if (max>0)
    {
        array->items = calloc(array->len, array->item_len);
        assert(array->items!=NULL);
    }
    else
    {
        array->items=NULL;
    }
}

static inline void *rsmp_array_get(rsmp_array_void_t *array, size_t i)
{
    if (array!=NULL && i<array->len) return ((char *)array->items) + (i * array->item_len);
    return NULL;
}


/**@}*/
#endif //VARS_STRING_H