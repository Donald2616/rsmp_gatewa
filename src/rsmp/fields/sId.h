/**
* @file sId.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_SID_H
#define RSMP_FIELDS_SID_H

#include "rsmp/vars/str.h"
/**
 * @defgroup sId 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_SID_C
#define SID_EXTERN
#define SID_PRIVATE
#endif

#ifndef SID_EXTERN
#define SID_EXTERN extern
#endif

#ifndef SID_PRIVATE
#define SID_PRIVATE const
#endif

typedef str_t sId_t;

#define rsmp_sId_set   rsmp_str_set 
#define rsmp_sId_free  rsmp_str_free 

#define __rsmp_field_sId_encode   __rsmp_field_str_encode 
#define __rsmp_field_sId_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_SID_H