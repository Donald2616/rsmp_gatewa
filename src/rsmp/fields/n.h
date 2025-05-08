/**
* @file n.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_N_H
#define RSMP_FIELDS_N_H

#include "rsmp/vars/str.h"
/**
 * @defgroup n 
 * 
 * @brief 
 * 
 * @{
 */

typedef str_t n_t;

#define rsmp_n_set   rsmp_str_set 
#define rsmp_n_free  rsmp_str_free 

#define __rsmp_field_n_encode   __rsmp_field_str_encode 
#define __rsmp_field_n_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_N_H