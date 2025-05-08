/**
* @file vers.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_VERS_H
#define RSMP_FIELDS_VERS_H

#include "rsmp/vars/str.h"
/**
 * @defgroup vers 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_VERS_C
#define VERS_EXTERN
#define VERS_PRIVATE
#endif

#ifndef VERS_EXTERN
#define VERS_EXTERN extern
#endif

#ifndef VERS_PRIVATE
#define VERS_PRIVATE const
#endif

typedef str_t vers_t;

#define rsmp_vers_set   rsmp_str_set 
#define rsmp_vers_free  rsmp_str_free 

#define __rsmp_field_vers_encode   __rsmp_field_str_encode 
#define __rsmp_field_vers_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_VERS_H