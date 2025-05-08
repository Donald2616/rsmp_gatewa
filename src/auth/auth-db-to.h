/**
* @file auth-db-to.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-07
* 
* @copyright Copyright (c) 2024
* 
*/
#ifndef AUTH_AUTH_DB_TO_H
#define AUTH_AUTH_DB_TO_H
/**
 * @defgroup auth-db-to 
 * 
 * @brief 
 * 
 * @ingroup auth
 * @{
 */
#include "auth/auth.h"
#include "auth/auth-basic.h"

extern const auth_dbId_t _auth_to_dbId;

AUTH_PROTECTED auth_t * auth_to_find_by_key(AUTH_PROTECTED auth_key_t *key);
AUTH_PROTECTED auth_t *auth_to_new(const auth_login_t login, const auth_sId_t sId, size_t len, char const *pwd, HoroDate const *ts, bool active, size_t new_pwd_len, char const *new_pwd, bool new_pwd_pending);
#define auth_to_free(p) auth_free(p) ;
void auth_to_delete_by_key(AUTH_PROTECTED auth_key_t *key);
int auth_to_foreach(const auth_sId_t sId, int(fct)(void *context, const auth_dbId_t dbId, auth_t const *auth), void *context);

AUTH_PROTECTED auth_key_t *auth_to_key_set(auth_key_t *key, const auth_login_t login, const auth_sId_t sId);
bool auth_to_is(const auth_t *item) ;
bool auth_to_dbId_is(const auth_dbId_t val) ;

/**@}*/
#endif //AUTH_AUTH-DB-TO_H