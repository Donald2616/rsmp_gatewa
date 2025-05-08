/**
* @file bool.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_VARS_BOOL_H
#define RSMP_VARS_BOOL_H
#include "rsmp/coder/vars.h"
/**
 * @defgroup bool 
 * 
 * @brief 
 * 
 * @{
 */
#ifdef RSMP_VARS_BOOL_C
#define BOOL_EXTERN
#define BOOL_PRIVATE
#endif

#ifndef BOOL_EXTERN
#define BOOL_EXTERN extern
#endif

#ifndef BOOL_PRIVATE
#define BOOL_PRIVATE const
#endif

int __rsmp_field_bool_encode(void const *field, rsmp_var_t **var);
int __rsmp_field_bool_decode(rsmp_var_t const *var, void *field);



/**@}*/
#endif //VARS_BOOL_H