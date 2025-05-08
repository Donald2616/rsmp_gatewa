/**
* @file sS.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_SS_H
#define RSMP_FIELDS_SS_H
#include <stdio.h>
#include "rsmp/coder/vars.h"
/**
 * @defgroup sS 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_SS_C
#define SS_EXTERN
#define SS_PRIVATE
#endif

#ifndef SS_EXTERN
#define SS_EXTERN extern
#endif

#ifndef SS_PRIVATE
#define SS_PRIVATE const
#endif

typedef enum {
    _rsmp_sS_Suspended_,
    _rsmp_sS_notSuspended_,

    _rsmp_sS_no_,
} sS_t;

SS_EXTERN const char *_rsmp_sS_lib[_rsmp_sS_no_];

static inline char const *rsmp_sS_lib_get(sS_t sS) {
    return (sS < _rsmp_sS_no_) ? _rsmp_sS_lib[sS] : NULL;
};

int __rsmp_field_sS_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_sS_decode(rsmp_var_t const *var, void *field);


/**@}*/
#endif //FIELDS_SS_H