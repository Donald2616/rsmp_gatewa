/**
* @file mType.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_MTYPE_H
#define RSMP_FIELDS_MTYPE_H
#include <stdlib.h>
#include <stdbool.h>
#include "rsmp/coder/vars.h"

/**
 * @defgroup mType 
 * 
 * @brief 
 * 
 * @{
 */
#ifdef RSMP_FIELDS_MTYPE_C
#define MTYPE_EXTERN
#define MTYPE_PRIVATE
#endif

#ifndef MTYPE_EXTERN
#define MTYPE_EXTERN extern
#endif
 
#ifndef MTYPE_PRIVATE
#define MTYPE_PRIVATE const
#endif

typedef char * mType_t;

bool rsmp_mType_check(char const *value);
int __rsmp_field_mType_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_mType_decode(rsmp_var_t const *var, void *field);

/**@}*/
#endif //RSMP_FIELDS_MTYPE_H
