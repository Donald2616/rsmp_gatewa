/**
* @file uuid.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_VARS_UUID_H
#define RSMP_VARS_UUID_H
#include "rsmp/coder/vars.h"

#include "uuid/uuid.h"
//  /*
//   * Should be, but isn't, defined in uuid/uuid.h
//   */
#ifndef UUID_STR_LEN
 #warning "UUID_STR_LEN defined in source code"
 #define UUID_STR_LEN	36
#endif
/**
 * @defgroup uuid 
 * 
 * @brief gestion commun des uuid
 * 
 * @{
 */
typedef char uuid_str_t[UUID_STR_LEN];
static inline char const *uuid_to_str(uuid_t uuid, uuid_str_t str) { uuid_unparse(uuid, str); return str;}
int __rsmp_field_uuid_encode(void const *field,rsmp_var_t **var);
int __rsmp_field_uuid_decode(rsmp_var_t const *var, void *field);



/**@}*/
#endif //FIELDS_UUID_H