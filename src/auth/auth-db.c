/**
* @file auth-db.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-09
* 
* @copyright Copyright (c) 2024
* 
*/
#include "config.h"


#define AUTH_AUTH_DB_C
#include "auth-db.h"

/**
 * @ingroup auth-db
 * @{
 */
static int auth_check_db(const auth_dbId_t val)
{
    return auth_form_dbId_is(val) || auth_to_dbId_is(val);
}

ssize_t auth_db_from_file(char const *fname)
{
    return auth_unserialize_from_file(fname, auth_check_db);
}


/**@}*/