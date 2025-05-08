/**
* @file sxl.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_SXL_H
#define RSMP_FIELDS_SXL_H

#include "rsmp/vars/str.h"
/**
 * @defgroup sxl 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_SXL_C
#define SXL_EXTERN
#define SXL_PRIVATE
#endif

#ifndef SXL_EXTERN
#define SXL_EXTERN extern
#endif

#ifndef SXL_PRIVATE
#define SXL_PRIVATE const
#endif

typedef str_t SXL_t;

#define rsmp_SXL_set   rsmp_str_set 
#define rsmp_SXL_free  rsmp_str_free 

#define __rsmp_field_SXL_encode   __rsmp_field_str_encode 
#define __rsmp_field_SXL_decode __rsmp_field_str_decode

/**@}*/
#endif //FIELDS_SXL_H