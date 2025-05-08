/**
* @file login.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-06
* 
* @copyright Copyright (c) 2024
* 
*/
#ifndef RSMP_FIELDS_LOGIN_H
#define RSMP_FIELDS_LOGIN_H
#include "rsmp/vars/str.h"

/**
 * @defgroup login 
 * 
 * @brief Champ login
 * 
 * @ingroup fields
 * @{
 */
#ifdef RSMP_FIELDS_LOGIN_C
#define LOGIN_EXTERN
#define LOGIN_PRIVATE
#endif

#ifndef LOGIN_EXTERN
#define LOGIN_EXTERN extern
#endif

#ifndef LOGIN_PRIVATE
#define LOGIN_PRIVATE const
#endif

typedef str_t login_t;

#define rsmp_login_set   rsmp_str_set 
#define rsmp_login_free  rsmp_str_free 

#define __rsmp_field_login_encode   __rsmp_field_str_encode 
#define __rsmp_field_login_decode __rsmp_field_str_decode




/**@}*/
#endif //FIELDS_LOGIN_H