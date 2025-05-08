/**
* @file aS.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_AS_H
#define RSMP_FIELDS_AS_H
#include <stdio.h>
#include "rsmp/coder/vars.h"
/**
 * @defgroup aS 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_AS_C
#define AS_EXTERN
#define AS_PRIVATE
#endif

#ifndef AS_EXTERN
#define AS_EXTERN extern
#endif

#ifndef AS_PRIVATE
#define AS_PRIVATE const
#endif

typedef enum {
    _rsmp_aS_inActive_,
    _rsmp_aS_Active_,

    _rsmp_aS_no_,
} aS_t;

AS_EXTERN const char *_rsmp_aS_lib[_rsmp_aS_no_];

static inline char const *rsmp_aS_lib_get(aS_t aS) {
    return (aS < _rsmp_aS_no_) ? _rsmp_aS_lib[aS] : NULL;
};

int __rsmp_field_aS_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_aS_decode(rsmp_var_t const *var, void *field);


/**@}*/
#endif //FIELDS_AS_H