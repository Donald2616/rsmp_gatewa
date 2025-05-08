/**
* @file rea.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_REA_H
#define RSMP_FIELDS_REA_H

#include "rsmp/vars/str.h"
/**
 * @defgroup rea 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_REA_C
#define REA_EXTERN
#define REA_PRIVATE
#endif

#ifndef REA_EXTERN
#define REA_EXTERN extern
#endif

#ifndef REA_PRIVATE
#define REA_PRIVATE const
#endif

typedef str_t rea_t;

#define rsmp_rea_set   rsmp_str_set 
#define rsmp_rea_free  rsmp_str_free 

#define __rsmp_field_rea_encode   __rsmp_field_str_encode 
#define __rsmp_field_rea_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_REA_H