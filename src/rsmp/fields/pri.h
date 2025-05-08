/**
* @file pri.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_PRI_H
#define RSMP_FIELDS_PRI_H
#include <stdio.h>
#include "rsmp/coder/vars.h"
/**
 * @defgroup pri 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_PRI_C
#define PRI_EXTERN
#define PRI_PRIVATE
#endif

#ifndef PRI_EXTERN
#define PRI_EXTERN extern
#endif

#ifndef PRI_PRIVATE
#define PRI_PRIVATE const
#endif

typedef enum {
    _rsmp_pri_1_,
    _rsmp_pri_2_,
    _rsmp_pri_3_,

    _rsmp_pri_no_,
} pri_t;

PRI_EXTERN const char *_rsmp_pri_lib[_rsmp_pri_no_];

static inline char const *rsmp_pri_lib_get(pri_t pri) {
    return (pri < _rsmp_pri_no_) ? _rsmp_pri_lib[pri] : NULL;
};

int __rsmp_field_pri_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_pri_decode(rsmp_var_t const *var, void *field);


/**@}*/
#endif //FIELDS_PRI_H