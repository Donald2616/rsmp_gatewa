/**
* @file age.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_AGE_H
#define RSMP_FIELDS_AGE_H
#include <stdio.h>
#include "rsmp/coder/vars.h"
#include "rsmp/vars/str.h"
/**
 * @defgroup age 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_AGE_C
#define AGE_EXTERN
#define AGE_PRIVATE
#endif

#ifndef AGE_EXTERN
#define AGE_EXTERN extern
#endif

#ifndef AGE_PRIVATE
#define AGE_PRIVATE const
#endif

typedef enum {
    _rsmp_age_unknown_,
    _rsmp_age_recent_,
    _rsmp_age_old_,
    _rsmp_age_undefined_,

    _rsmp_age_no_,
} age_t;

AGE_EXTERN const char *_rsmp_age_lib[_rsmp_age_no_];

static inline char const *rsmp_age_lib_get(age_t age) {
    return (age < _rsmp_age_no_) ? _rsmp_age_lib[age] : NULL;
};

int __rsmp_field_age_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_age_decode(rsmp_var_t const *var, void *field);


/**@}*/
#endif //FIELDS_AGE_H