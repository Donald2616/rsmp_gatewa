/**
* @file fP.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_FP_H
#define RSMP_FIELDS_FP_H

#include "rsmp/vars/str.h"
/**
 * @defgroup fP 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_FP_C
#define FP_EXTERN
#define FP_PRIVATE
#endif

#ifndef FP_EXTERN
#define FP_EXTERN extern
#endif

#ifndef FP_PRIVATE
#define FP_PRIVATE const
#endif

typedef str_t fP_t;

#define rsmp_fP_set   rsmp_str_set 
#define rsmp_fP_free  rsmp_str_free 

#define __rsmp_field_fP_encode   __rsmp_field_str_encode 
#define __rsmp_field_fP_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_FP_H