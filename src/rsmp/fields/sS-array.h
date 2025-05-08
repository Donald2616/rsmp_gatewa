/**
* @file sS-array.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_SS_A_H
#define RSMP_FIELDS_SS_A_H
#include "rsmp/vars/array.h"
#include "rsmp/fields/sCI.h"
#include "rsmp/fields/n.h"
#include "rsmp/fields/q.h"
#include "rsmp/fields/s.h"
#include "rsmp/fields/uRt.h"
#include "rsmp/fields/sOc.h"
/**
 * @defgroup sS_array 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_SS_A_C
#define SS_A_EXTERN
#define SS_A_PRIVATE
#endif

#ifndef SS_A_EXTERN
#define SS_A_EXTERN extern
#endif

#ifndef SS_A_PRIVATE
#define SS_A_PRIVATE const
#endif


typedef struct rsmp_msg_sS_stat_req
{
    sCI_t sCI;
    n_t   n;
    s_t   s;
    q_t   q;
    uRt_t uRt;
    sOc_t sOc;
} sSs;

rsmp_array_declare_free(sSs);

// #include <assert.h>
// #define __rsmp_field_sSs_decode exit(0)

/**@}*/
#endif //FIELDS_SS_A_H