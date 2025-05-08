/**
* @file auth-db-from.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-07
* 
* @copyright Copyright (c) 2024
* 
*/
#include "config.h"

#define AUTH_BASIC_LEGACY
#define AUTH_PROTECT_C
#define AUTH_AUTH_DB_FROM_C
#include "auth-basic.h"
#include "auth-db-to.h"

/**
 * @ingroup auth-db-to
 * @{
 */

const auth_dbId_t _auth_from_dbId = "from";

AUTH_PROTECTED auth_t * auth_from_find_by_key(AUTH_PROTECTED auth_key_t *key) 
{
    assert(key);
    ((auth_key_t *)key)->dbId = _auth_from_dbId;
    return auth_find_by_key(key);
}

AUTH_PROTECTED auth_t *auth_from_new(const auth_login_t login, const auth_sId_t sId, size_t len, char const *pwd, HoroDate const *ts, bool active, size_t new_pwd_len, char const *new_pwd, bool new_pwd_pending)
{ return auth_new(_auth_from_dbId, login, sId, len, pwd, ts, active, new_pwd_len, new_pwd, new_pwd_pending);}


void auth_from_delete_by_key(AUTH_PROTECTED auth_key_t *key) 
{
    assert(key);
    ((auth_key_t *)key)->dbId = _auth_from_dbId;
    auth_delete_by_key(key);
}

int auth_from_foreach(const auth_sId_t sId, int(fct)(void *context, const auth_dbId_t dbId, auth_t const *auth), void *context)
{ return auth_foreach(_auth_from_dbId, sId, fct, context);}

AUTH_PROTECTED auth_key_t *auth_from_key_set(auth_key_t *key, const auth_login_t login, const auth_sId_t sId)
{
    _log_info("key = %p", key);
    // sleep(1);
    fflush(stdout);
    return auth_key_set(key, _auth_from_dbId, login, sId);
}

bool auth_from_is(const auth_t *item) 
{ 
    return auth_db_is(item, _auth_from_dbId);
}

bool auth_form_dbId_is(const auth_dbId_t val) 
{
    return auth_dbId_is(val, _auth_from_dbId);
}

/**@}*/