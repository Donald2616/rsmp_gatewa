/**
* @file xACId.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_XACID_H
#define RSMP_FIELDS_XACID_H

#include "rsmp/vars/str.h"
/**
 * @defgroup xACId 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_XACID_C
#define XACID_EXTERN
#define XACID_PRIVATE
#endif

#ifndef XACID_EXTERN
#define XACID_EXTERN extern
#endif

#ifndef XACID_PRIVATE
#define XACID_PRIVATE const
#endif

typedef str_t xACId_t;

#define rsmp_xACId_set    rsmp_str_set 
#define rsmp_xACId_free   rsmp_str_free 


#define __rsmp_field_xACId_encode   __rsmp_field_str_encode 
#define __rsmp_field_xACId_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_XACID_H