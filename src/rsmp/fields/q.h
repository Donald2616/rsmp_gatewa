/**
* @file q.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_Q_H
#define RSMP_FIELDS_Q_H
#include <stdio.h>
#include "rsmp/coder/vars.h"
#include "rsmp/vars/str.h"
/**
 * @defgroup q 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_Q_C
#define Q_EXTERN
#define Q_PRIVATE
#endif

#ifndef Q_EXTERN
#define Q_EXTERN extern
#endif

#ifndef Q_PRIVATE
#define Q_PRIVATE const
#endif

typedef enum {
    _rsmp_q_unknown_,
    _rsmp_q_recent_,
    _rsmp_q_old_,
    _rsmp_q_undefined_,

    _rsmp_q_no_,
} q_t;

Q_EXTERN const char *_rsmp_q_lib[_rsmp_q_no_];

static inline char const *rsmp_q_lib_get(q_t q) {
    return (q < _rsmp_q_no_) ? _rsmp_q_lib[q] : NULL;
};

int __rsmp_field_q_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_q_decode(rsmp_var_t const *var, void *field);


/**@}*/
#endif //FIELDS_Q_H