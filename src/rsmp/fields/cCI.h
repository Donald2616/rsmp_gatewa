/**
* @file cCI.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_CCI_H
#define RSMP_FIELDS_CCI_H

#include "rsmp/vars/str.h"
/**
 * @defgroup cCI 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_CCI_C
#define CCI_EXTERN
#define CCI_PRIVATE
#endif

#ifndef CCI_EXTERN
#define CCI_EXTERN extern
#endif

#ifndef CCI_PRIVATE
#define CCI_PRIVATE const
#endif

typedef str_t cCI_t;

#define rsmp_cCI_set    rsmp_str_set 
#define rsmp_cCI_free   rsmp_str_free


#define __rsmp_field_cCI_encode   __rsmp_field_str_encode 
#define __rsmp_field_cCI_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_CCI_H