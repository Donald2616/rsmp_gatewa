/**
* @file se.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_SE_H
#define RSMP_FIELDS_SE_H
#include <stdlib.h>
#include <stdbool.h>

#include "rsmp/vars/bool.h"
#include "rsmp/vars/array.h"
/**
 * @defgroup se 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_SE_C
#define SE_EXTERN
#define SE_PRIVATE
#endif

#ifndef SE_EXTERN
#define SE_EXTERN extern
#endif

#ifndef SE_PRIVATE
#define SE_PRIVATE const
#endif

typedef bool se;
rsmp_array_declare(se);


#define __rsmp_field_se_encode   __rsmp_field_bool_encode 
#define __rsmp_field_se_decode   __rsmp_field_bool_decode


/**@}*/
#endif //FIELDS_SE_H