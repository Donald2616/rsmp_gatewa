/**
* @file auth.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-07
* 
* @copyright Copyright (c) 2024
* 
*/
#ifndef AUTH_AUTH_H
#define AUTH_AUTH_H
#include <stdbool.h>
#include <string.h>

#include "horodate/horodate.h"

/**
 * @defgroup auth 
 * 
 * @brief Authentification
 * 
 * @{
 */
#ifdef AUTH_AUTH_C
#define AUTH_EXTERN
#define AUTH_PRIVATE
#define AUTH_PROTECT_C
#endif

#ifdef AUTH_PROTECT_C
#define AUTH_PROTECTED
#endif

#ifndef AUTH_EXTERN
#define AUTH_EXTERN exterm
#endif

#ifndef AUTH_PRIVATE
#define AUTH_PRIVATE const
#endif

#ifndef AUTH_PROTECTED
#define AUTH_PROTECTED const
#endif

#include "auth/auth-pwd.h"

typedef char * auth_login_t;
typedef char * auth_sId_t;
typedef char * auth_dbId_t;

#define AUTH_DATE_FORMAT "%Y-%m-%dT%H:%M:%SZ"

#define auth_strHorodate(s,l,h) strGmtHorodate(AUTH_DATE_FORMAT, 0, s, l, h)

typedef struct  auth_key_t
{
    auth_dbId_t   dbId;
    auth_login_t  login;
    auth_sId_t    sId;
} auth_key_t;

typedef struct auth_t
{
    AUTH_PROTECTED bool active;
    AUTH_PROTECTED auth_key_t key;
    
    AUTH_PROTECTED auth_pwd_t pwd;
    AUTH_PROTECTED HoroDate timestamp; // date d'enregistrement de pwd

    AUTH_PROTECTED bool new_pwd_pending;
    AUTH_PROTECTED auth_pwd_t new_pwd;
} auth_t;

static inline void *memdup(void *buf, size_t len)
{
    void *out=NULL;
    if (buf && len)
    {
        out=malloc(len);
        if (out) memcpy(out, buf, len);
    }
    return out;
}



/** 
 * Génère un nouveau mot de passe dans la base TO
 * N'écrase pas new_pwd si déjà existant (cf. forget_new_pwd)
*/
int auth_get_new_pwd_b64(auth_t *auth, char **out);

/**
 * Enregistrer le pw reçu dans la base FROM
*/
int auth_set_pwd_b64(auth_t *auth, const char *in);

/** 
 * Basculer le new_pwd en current dans la base TO
*/
int auth_switch_to_new_pwd(auth_t *auth);

/**
 * Oublier un new_pwd 
*/
int auth_forget_new_pwd(auth_t *auth);
/**@}*/
#endif //AUTH_AUTH_H