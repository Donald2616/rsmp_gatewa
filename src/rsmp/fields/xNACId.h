/**
* @file xNACId.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_XNACID_H
#define RSMP_FIELDS_XNACID_H

#include "rsmp/vars/str.h"
/**
 * @defgroup xNACId 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_XNACID_C
#define XNACID_EXTERN
#define XNACID_PRIVATE
#endif

#ifndef XNACID_EXTERN
#define XNACID_EXTERN extern
#endif

#ifndef XNACID_PRIVATE
#define XNACID_PRIVATE const
#endif

typedef str_t xNACId_t;

#define rsmp_xNACId_set    rsmp_str_set 
#define rsmp_xNACId_free   rsmp_str_free 


#define __rsmp_field_xNACId_encode   __rsmp_field_str_encode 
#define __rsmp_field_xNACId_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_XNACID_H