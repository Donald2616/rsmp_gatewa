/**
* @file cO.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_CO_H
#define RSMP_FIELDS_CO_H

#include "rsmp/vars/str.h"
/**
 * @defgroup cO 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_CO_C
#define CO_EXTERN
#define CO_PRIVATE
#endif

#ifndef CO_EXTERN
#define CO_EXTERN extern
#endif

#ifndef CO_PRIVATE
#define CO_PRIVATE const
#endif

typedef str_t cO_t;

#define rsmp_cO_set    rsmp_str_set 
#define rsmp_cO_free   rsmp_str_free 


#define __rsmp_field_cO_encode   __rsmp_field_str_encode 
#define __rsmp_field_cO_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_CO_H