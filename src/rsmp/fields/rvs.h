/**
* @file rvs.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_RVS_H
#define RSMP_FIELDS_RVS_H
#include "rsmp/vars/array.h"
#include "rsmp/fields/cCI.h"
#include "rsmp/fields/n.h"
#include "rsmp/fields/age.h"
#include "rsmp/fields/v.h"
/**
 * @defgroup arg 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_RVS_C
#define RVS_EXTERN
#define RVS_PRIVATE
#endif

#ifndef RVS_EXTERN
#define RVS_EXTERN extern
#endif

#ifndef RVS_PRIVATE
#define RVS_PRIVATE const
#endif


typedef struct rsmp_msg_rvs_cmd_res
{
    cCI_t cCI;
    n_t   n;
    v_t   v;
    age_t age;
} rvs;

rsmp_array_declare_free(rvs);


/**@}*/
#endif //FIELDS_RVS_H