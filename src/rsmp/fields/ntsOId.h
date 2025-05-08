/**
* @file ntsOId.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_NTSOID_H
#define RSMP_FIELDS_NTSOID_H

#include "rsmp/vars/str.h"
/**
 * @defgroup ntsOId 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_NTSOID_C
#define NTSOID_EXTERN
#define NTSOID_PRIVATE
#endif

#ifndef NTSOID_EXTERN
#define NTSOID_EXTERN extern
#endif

#ifndef NTSOID_PRIVATE
#define NTSOID_PRIVATE const
#endif

typedef str_t ntsOId_t;

#define rsmp_ntsOId_set   rsmp_str_set 
#define rsmp_ntsOId_free  rsmp_str_free 


#define __rsmp_field_ntsOId_encode   __rsmp_field_str_encode 
#define __rsmp_field_ntsOId_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_NTSOID_H