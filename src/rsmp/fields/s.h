/**
* @file s.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_S_H
#define RSMP_FIELDS_S_H

#include "rsmp/vars/str.h"
/**
 * @defgroup s 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_S_C
#define S_EXTERN
#define S_PRIVATE
#endif

#ifndef S_EXTERN
#define S_EXTERN extern
#endif

#ifndef S_PRIVATE
#define S_PRIVATE const
#endif

typedef str_t s_t;

#define rsmp_s_set    rsmp_str_set 
#define rsmp_s_free   rsmp_str_free 


#define __rsmp_field_s_encode   __rsmp_field_str_encode 
#define __rsmp_field_s_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_S_H