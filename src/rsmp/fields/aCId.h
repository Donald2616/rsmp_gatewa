/**
* @file aCId.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_ACID_H
#define RSMP_FIELDS_ACID_H

#include "rsmp/vars/str.h"
/**
 * @defgroup aCId 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_ACID_C
#define ACID_EXTERN
#define ACID_PRIVATE
#endif

#ifndef ACID_EXTERN
#define ACID_EXTERN extern
#endif

#ifndef ACID_PRIVATE
#define ACID_PRIVATE const
#endif

typedef str_t aCId_t;

#define rsmp_aCId_set    rsmp_str_set 
#define rsmp_aCId_free   rsmp_str_free 


#define __rsmp_field_aCId_encode   __rsmp_field_str_encode 
#define __rsmp_field_aCId_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_ACID_H