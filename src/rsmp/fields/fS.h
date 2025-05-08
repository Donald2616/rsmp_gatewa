/**
* @file fS.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_FS_H
#define RSMP_FIELDS_FS_H

#include "rsmp/vars/str.h"
/**
 * @defgroup fS 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_FS_C
#define FS_EXTERN
#define FS_PRIVATE
#endif

#ifndef FS_EXTERN
#define FS_EXTERN extern
#endif

#ifndef FS_PRIVATE
#define FS_PRIVATE const
#endif

typedef str_t fS_t;

#define rsmp_fS_set   rsmp_str_set 
#define rsmp_fS_free  rsmp_str_free 

#define __rsmp_field_fS_encode   __rsmp_field_str_encode 
#define __rsmp_field_fS_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_FS_H