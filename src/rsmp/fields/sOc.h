/**
* @file sOc.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_SOC_H
#define RSMP_FIELDS_SOC_H
#include "rsmp/coder/vars.h"
#include "rsmp/vars/bool.h"
/**
 * @defgroup sOc 
 * 
 * @brief 
 * 
 * @{
 */
#ifdef RSMP_FIELDS_SOC_C
#define SOC_EXTERN
#define SOC_PRIVATE
#endif

#ifndef SOC_EXTERN
#define SOC_EXTERN extern
#endif

#ifndef SOC_PRIVATE
#define SOC_PRIVATE const
#endif

typedef bool sOc_t;

#define __rsmp_field_sOc_encode __rsmp_field_bool_encode
#define __rsmp_field_sOc_decode __rsmp_field_bool_decode



/**@}*/
#endif //FIELDS_SOC_H