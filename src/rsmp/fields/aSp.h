/**
* @file aSp.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_ASP_H
#define RSMP_FIELDS_ASP_H
#include <stdio.h>
#include "rsmp/coder/vars.h"
/**
 * @defgroup aSp 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_ASP_C
#define ASP_EXTERN
#define ASP_PRIVATE
#endif

#ifndef ASP_EXTERN
#define ASP_EXTERN extern
#endif

#ifndef ASP_PRIVATE
#define ASP_PRIVATE const
#endif

typedef enum {
    _rsmp_aSp_Issue_,
    _rsmp_aSp_Request_,
    _rsmp_aSp_Acknowledge_,
    _rsmp_aSp_Suspend_,
    _rsmp_aSp_Resume_,

    _rsmp_aSp_no_,
} aSp_t;

ASP_EXTERN const char *_rsmp_aSp_lib[_rsmp_aSp_no_];

static inline char const *rsmp_aSp_lib_get(aSp_t aSp) {
    return (aSp < _rsmp_aSp_no_) ? _rsmp_aSp_lib[aSp] : NULL;
};

int __rsmp_field_aSp_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_aSp_decode(rsmp_var_t const *var, void *field);


/**@}*/
#endif //FIELDS_ASP_H