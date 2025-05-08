/**
* @file sCI.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_SCI_H
#define RSMP_FIELDS_SCI_H

#include "rsmp/vars/str.h"
/**
 * @defgroup sCI 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_SCI_C
#define SCI_EXTERN
#define SCI_PRIVATE
#endif

#ifndef SCI_EXTERN
#define SCI_EXTERN extern
#endif

#ifndef SCI_PRIVATE
#define SCI_PRIVATE const
#endif

typedef str_t sCI_t;

#define rsmp_sCI_set   rsmp_str_set 
#define rsmp_sCI_free  rsmp_str_free 

#define __rsmp_field_sCI_encode   __rsmp_field_str_encode 
#define __rsmp_field_sCI_decode   __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_SCI_H