/**
* @file uRt.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_URT_H
#define RSMP_FIELDS_URT_H

#include "rsmp/vars/str.h"
/**
 * @defgroup uRt 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_URT_C
#define URT_EXTERN
#define URT_PRIVATE
#endif

#ifndef URT_EXTERN
#define URT_EXTERN extern
#endif

#ifndef URT_PRIVATE
#define URT_PRIVATE const
#endif

typedef str_t uRt_t;

#define rsmp_uRt_set    rsmp_str_set 
#define rsmp_uRt_free   rsmp_str_free 


#define __rsmp_field_uRt_encode   __rsmp_field_str_encode 
#define __rsmp_field_uRt_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_URT_H