/**
* @file sg.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_SG_H
#define RSMP_FIELDS_SG_H
#include <stdlib.h>
#include "rsmp/coder/vars.h"
/**
 * @defgroup sg 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_FIELDS_SG_C
#define SG_EXTERN
#define SG_PRIVATE
#endif

#ifndef SG_EXTERN
#define SG_EXTERN extern
#endif

#ifndef SG_PRIVATE
#define SG_PRIVATE const
#endif


typedef enum {
    _rsmp_sg_GEN_ = 0,  // GEN message
    _rsmp_sg_SLT_,      // SLT message
    _rsmp_sg_IO_,       // IO message
    _rsmp_sg_VAR_,      // VAR message

    _rsmp_sg_no_,       // # no message or error....
} sg_t;

extern char const *_rsmp_sg_lib[_rsmp_sg_no_];
#define rsmp_sg_exits(_t_) ((_t_) < _rsmp_msg_no_)


char const * rsmp_sg_to_string(sg_t sg);
sg_t rsmp_sg_from_string(char const *str);
// bool rsmp_sg_lib(sg_t *sg, char * const *val);

int __rsmp_field_sg_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_sg_decode(rsmp_var_t const *var, void *field);
/**@}*/
#endif //RSMP_FIELDS_SG_H
