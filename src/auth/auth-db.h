/**
* @file auth-db.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-09
* 
* @copyright Copyright (c) 2024
* 
*/
#ifndef AUTH_AUTH_DB_H
#define AUTH_AUTH_DB_H
/**
 * @defgroup auth-db 
 * 
 * @brief 
 * 
 * @{
 */
#include "auth/auth-db-to.h"
#include "auth/auth-db-from.h"

ssize_t auth_db_from_file(char const *fname);
#define  auth_db_to_file(fname) auth_serialize_to_file(fname)


/**@}*/
#endif //AUTH_AUTH-DB_H