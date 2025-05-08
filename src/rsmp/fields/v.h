/**
* @file v.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_V_H
#define RSMP_FIELDS_V_H
#include "rsmp/coder/vars.h"
#include "rsmp/vars/arraystr.h"
/**
 * @defgroup v 
 * 
 * @brief 
 * 
 * @{
 */
#ifdef RSMP_FIELDS_V_C
#define V_EXTERN
#define V_PRIVATE
#endif

#ifndef V_EXTERN
#define V_EXTERN extern
#endif

#ifndef V_PRIVATE
#define V_PRIVATE const
#endif

typedef rsmp_array_char_t v_t;



int __rsmp_field_v_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_v_decode(rsmp_var_t const *var, void *field);



/**@}*/
#endif //FIELDS_V_H