/**
* @file str.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_STR_H
#define RSMP_FIELDS_STR_H
#include "rsmp/coder/vars.h"
/**
 * @defgroup str 
 * 
 * @brief 
 * 
 * @{
 */
#ifdef RSMP_VARS_STR_C
#define STR_EXTERN
#define STR_PRIVATE
#endif

#ifndef STR_EXTERN
#define STR_EXTERN extern
#endif

#ifndef STR_PRIVATE
#define STR_PRIVATE const
#endif

typedef char * str_t;
int rsmp_str_set(str_t *item, char const *value);

//void rsmp_str_free(str_t *item);
static inline void rsmp_str_free(str_t *item) 
{ 
    if (*item)
    {
        free(*item);
        *item=NULL;
    }
};

int __rsmp_field_str_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_str_decode(rsmp_var_t const *var, void *field);



/**@}*/
#endif //FIELDS_STR_H