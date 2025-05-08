/**
* @file cId.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_CID_H
#define RSMP_FIELDS_CID_H

#include "rsmp/vars/str.h"
/**
 * @defgroup cId 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_CID_C
#define CID_EXTERN
#define CID_PRIVATE
#endif

#ifndef CID_EXTERN
#define CID_EXTERN extern
#endif

#ifndef CID_PRIVATE
#define CID_PRIVATE const
#endif

typedef str_t cId_t;

#define rsmp_cId_set   rsmp_str_set 
#define rsmp_cId_free  rsmp_str_free 


#define __rsmp_field_cId_encode   __rsmp_field_str_encode 
#define __rsmp_field_cId_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_CID_H