/**
* @file arg.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_ARG_H
#define RSMP_FIELDS_ARG_H
#include "rsmp/vars/array.h"
#include "rsmp/fields/cCI.h"
#include "rsmp/fields/n.h"
#include "rsmp/fields/cO.h"
#include "rsmp/fields/v.h"
/**
 * @defgroup arg 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_ARG_C
#define ARG_EXTERN
#define ARG_PRIVATE
#endif

#ifndef ARG_EXTERN
#define ARG_EXTERN extern
#endif

#ifndef ARG_PRIVATE
#define ARG_PRIVATE const
#endif


typedef struct rsmp_msg_arg_cmd_req
{
    cCI_t cCI;
    n_t   n;
    cO_t  cO;
    v_t   v;
} args;

rsmp_array_declare_free(args);


/**@}*/
#endif //FIELDS_ARG_H