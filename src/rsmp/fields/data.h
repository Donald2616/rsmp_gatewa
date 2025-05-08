/**
* @file data.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_DATA_H
#define RSMP_FIELDS_DATA_H

#include "rsmp/vars/array.h"
#include "rsmp/vars/str.h"
/**
 * @defgroup data 
 * 
 * @brief 
 * 
 * @ingroup fields
 * @{
 */

#ifdef RSMP_FIELDS_DATA_C
#define DATA_EXTERN
#define DATA_PRIVATE
#endif

#ifndef DATA_EXTERN
#define DATA_EXTERN extern
#endif

#ifndef DATA_PRIVATE
#define DATA_PRIVATE const
#endif

typedef str_t data;

rsmp_array_declare(data);

// #define rsmp_data_set    rsmp_str_set 
// #define rsmp_data_free   rsmp_str_free 


#define __rsmp_field_data_encode __rsmp_field_str_encode 
#define __rsmp_field_data_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_DATA_H
