/**
* @file cat.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_CAT_H
#define RSMP_FIELDS_CAT_H
#include <stdio.h>
#include "rsmp/coder/vars.h"
/**
 * @defgroup cat 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_CAT_C
#define CAT_EXTERN
#define CAT_PRIVATE
#endif

#ifndef CAT_EXTERN
#define CAT_EXTERN extern
#endif

#ifndef CAT_PRIVATE
#define CAT_PRIVATE const
#endif

typedef enum {
    _rsmp_cat_T_,
    _rsmp_cat_D_,

    _rsmp_cat_no_,
} cat_t;

CAT_EXTERN const char *_rsmp_cat_lib[_rsmp_cat_no_];

static inline char const *rsmp_cat_lib_get(cat_t cat) {
    return (cat < _rsmp_cat_no_) ? _rsmp_cat_lib[cat] : NULL;
};

int __rsmp_field_cat_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_cat_decode(rsmp_var_t const *var, void *field);


/**@}*/
#endif //FIELDS_CAT_H