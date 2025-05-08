/**
* @file xNId.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_XNID_H
#define RSMP_FIELDS_XNID_H

#include "rsmp/vars/str.h"
/**
 * @defgroup xNId 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_XNID_C
#define XNID_EXTERN
#define XNID_PRIVATE
#endif

#ifndef XNID_EXTERN
#define XNID_EXTERN extern
#endif

#ifndef XNID_PRIVATE
#define XNID_PRIVATE const
#endif

typedef str_t xNId_t;

#define rsmp_xNId_set    rsmp_str_set 
#define rsmp_xNId_free   rsmp_str_free 


#define __rsmp_field_xNId_encode   __rsmp_field_str_encode 
#define __rsmp_field_xNId_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_XNID_H