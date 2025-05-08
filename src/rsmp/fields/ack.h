/**
* @file ack.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_ACK_H
#define RSMP_FIELDS_ACK_H
#include <stdio.h>
#include "rsmp/coder/vars.h"
/**
 * @defgroup ack 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_ACK_C
#define ACK_EXTERN
#define ACK_PRIVATE
#endif

#ifndef ACK_EXTERN
#define ACK_EXTERN extern
#endif

#ifndef ACK_PRIVATE
#define ACK_PRIVATE const
#endif

typedef enum {
    _rsmp_ack_notAcknowledged_,
    _rsmp_ack_Acknowledged_,

    _rsmp_ack_no_,
} ack_t;

ACK_EXTERN const char *_rsmp_ack_lib[_rsmp_ack_no_];

static inline char const *rsmp_ack_lib_get(ack_t ack) {
    return (ack < _rsmp_ack_no_) ? _rsmp_ack_lib[ack] : NULL;
};

int __rsmp_field_ack_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_ack_decode(rsmp_var_t const *var, void *field);


/**@}*/
#endif //FIELDS_ACK_H