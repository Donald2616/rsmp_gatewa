/**
* @file auth-pwd.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-07
* 
* @copyright Copyright (c) 2024
* 
*/
#ifndef AUTH_AUTH_PWD_H
#define AUTH_AUTH_PWD_H


#include "errcode/errcode.h"
#include "memory/checkmem.h"
#include "crypt/base64.h"
/**
 * @defgroup auth-pwd 
 * 
 * @brief 
 * 
 * @{
 */

typedef struct auth_pwd_t
{
    AUTH_PROTECTED char *buf;
    AUTH_PROTECTED size_t len;
} auth_pwd_t;


extern const auth_pwd_t _auth_pwd_null_;
#define AUTH_PWD_LEN_MIN 8
#define AUTH_PWD_NEW_LEN_FOR_CHALLENGE 64
#define AUTH_PWD_LEN_MAX 256
// \todo clarifier comportement pour taille mini log only ou je jette (réponse busy ???)

#define auth_pwd_declare(var) auth_pwd_t var = _auth_pwd_null_

auth_pwd_t const *auth_pwd_auto_set(auth_pwd_t *pwd);
auth_pwd_t const *auth_pwd_set(auth_pwd_t *pwd, char const * buf, size_t len);
auth_pwd_t const *auth_pwd_from_str_set(auth_pwd_t *pwd, char const * str);
void auth_pwd_release(auth_pwd_t *pwd);
auth_pwd_t *auth_pwd_cpy(auth_pwd_t *dest, auth_pwd_t *src);
auth_pwd_t const *auth_pwd_decode(auth_pwd_t *pwd, char const *str);
int auth_pwd_cmp(auth_pwd_t const *pwd1, auth_pwd_t const *pwd2);

#define _auth_pwd_hash_out_bdc_ true
#define _auth_pwd_hash_out_binary_ false

typedef enum {
        _auth_pwd_hash_frob_=0,
        _auth_pwd_hash_sha256_,
        _auth_pwd_hash_md5_,

        _auth_pwd_hash_no_
} auth_hash_type_t;

char *auth_pwd_hash(auth_pwd_t const *pwd, bool bdc_out, char **out, size_t *len);
#define auth_pwd_hash_bdc(pwd, out)         auth_pwd_hash(pwd, _auth_pwd_hash_out_bdc_, out, NULL);
#define auth_pwd_hash_binary(pwd, out, len) auth_pwd_hash(pwd, _auth_pwd_hash_out_binary_, out, len);
void auth_pwd_hash_type_set(auth_hash_type_t hash_type);
auth_hash_type_t auth_pwd_hash_type_get(void);
const char *auth_pwd_hash_type_lib_get(auth_hash_type_t hash_type);



/**@}*/
#endif //AUTH_AUTH-PWD_H