/**
* @file auth-pwd.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-07
* 
* @copyright Copyright (c) 2024
* 
*/
#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <search.h>
#include <ctype.h>
   
#define AUTH_PROTECT_C
#define AUTH_AUTH_PWD_C
#include "auth.h"
#include "auth-pwd.h"
#include "auth-random.h"
#include "crypt/bcd.h"

/**
 * @ingroup auth-pwd
 * @{
 */

const auth_pwd_t _auth_pwd_null_ = {.buf=NULL, .len=0};


static const size_t _auth_pwd_len_delta=22;

static inline size_t auth_pwd_len_get(void)
{
    unsigned short xsubi[3];
    auth_random((char *)xsubi, sizeof(xsubi));
    return AUTH_PWD_LEN_MIN + erand48(xsubi) *_auth_pwd_len_delta;
}


static inline bool auth_pwd_str_is_hex(char const *str)
{
    assert(str);
    size_t len = strlen(str);
    if (len & 0x01) return false;
    if (len < AUTH_PWD_LEN_MIN<<1 ) return false;
    char const *s = str;
    while (*s != '\0')
    {
        if (!isxdigit(*s)) return false;
        s++;
    }
    return true;
}


void auth_pwd_release(auth_pwd_t *item)
{
    if (item)
    {
        if(item->buf) 
        {
            auth_random(item->buf, item->len);
            free(item->buf);
            item->buf = NULL;
        }
    }
}

auth_pwd_t *auth_pwd_cpy(auth_pwd_t *dest, auth_pwd_t *src)
{
    if (dest && src)
    {
        auth_pwd_set(dest, src->buf, src->len);
    }
    return dest;
}


auth_pwd_t const *auth_pwd_auto_set(auth_pwd_t *pwd)
{
    assert(pwd);
    auth_pwd_release(pwd);
    //-- récupération d'un nombre d'octet aléatoire
    pwd->len = auth_pwd_len_get();
    if ((pwd->buf = malloc(pwd->len)) != NULL)
    { 
        ssize_t ret = auth_random(pwd->buf, pwd->len);
        assert(((size_t)ret)==pwd->len);
    }
    return pwd;
}

auth_pwd_t const * auth_pwd_set(auth_pwd_t *pwd, char const * buf, size_t len)
{
    if (!buf || !len) return auth_pwd_auto_set(pwd);
    static auth_pwd_t _pwd ={.len=0, .buf=NULL};
    if (!pwd) pwd = &_pwd;
    auth_pwd_release(pwd);
    pwd->len = len;
    if ((pwd->buf = malloc(pwd->len)) != NULL)
    { 
        memcpy(pwd->buf, buf, pwd->len);
    }
    assert(pwd->len);
    assert(pwd->buf);
    return pwd;
}

auth_pwd_t const * auth_pwd_from_str_set(auth_pwd_t *pwd, char const * str)
{
    if (!str) return auth_pwd_auto_set(pwd);
    size_t len = strlen(str);
    if(len<AUTH_PWD_LEN_MIN) {
            _log_err("password too short (len = %d)", len);
    } 
    return len<AUTH_PWD_LEN_MIN?NULL:auth_pwd_set(pwd, str, len);
}

auth_pwd_t const * auth_pwd_from_hex_set(auth_pwd_t *pwd, char const * str)
{
    if (!str) return auth_pwd_auto_set(pwd);
    if (!auth_pwd_str_is_hex(str)) return NULL;
    auth_pwd_release(pwd);
    pwd->buf = bcd_decode(str, &pwd->len);
    if(pwd->len < AUTH_PWD_LEN_MIN){
         _log_err("password too short (len = %d)", pwd->len); // ne faire que du log
    }

    return pwd;
}

auth_pwd_t const *auth_pwd_decode(auth_pwd_t *pwd, char const *str)
{
    assert(str);
    assert(pwd);
    size_t len = strlen(str);

    if (len > 2)
    {
        if (str[0]=='$' && str[2]=='$')
        {
            switch(str[1])
            {
                case '0':
                    str += 3;
                    if (*str=='*')
                    {
                        auth_pwd_release(pwd);
                        return pwd;
                    }
                    return auth_pwd_from_hex_set(pwd, str);
                    break;
                default :
                    return NULL;
            }
        }
        else if (len>=AUTH_PWD_LEN_MIN)
        {
            return auth_pwd_from_str_set(pwd, str);
        } else {
            _log_err("1st password too short (len = %d) CANNOT USE IT TO INITIATE LOGIN", len);
        }
    }
     return NULL;
}

int auth_pwd_cmp(auth_pwd_t const *pwd1, auth_pwd_t const *pwd2)
{
    if (!pwd1 && !pwd2) return 0;
    if (!pwd1) return 1;
    if (!pwd2) return -1;
    int ret = pwd1->len - pwd2->len;
    if (!ret) ret = memcmp(pwd1->buf, pwd2->buf, pwd1->len);
    return ret;
}


static inline char *auth_pwd_hash2bdc(char **out, size_t *len)
{
    char *bdc = bcd_encode(*out, *len);
    free(*out);
    if (bdc)
    {
        *out = bdc;
        *len = strlen(*out);
    }
    else
    {
        *out = NULL;
        *len = 0;
    }
    return *out;
}

#ifdef WITH_MHASH
#include <mhash.h>

static inline char *auth_pwd_mhash(hashid hash_id, auth_pwd_t const *pwd, bool bdc_out, char **out, size_t *len)
{
    assert(pwd);
    assert(out);
    assert((!bdc_out && len) || bdc_out);
    size_t _len;
    if (!len) len = &_len;
    *len = 0;
    if (!pwd->len || !pwd->buf) return NULL;
    *len = mhash_get_block_size(hash_id);
    if (!*len) return NULL;
    *out = malloc(*len);
    if (*out)
    {
        MHASH td;
        td = mhash_init(hash_id);
        mhash(td, pwd->buf, pwd->len);
        mhash_deinit(td, *out);
        if (bdc_out) *out = auth_pwd_hash2bdc(out, len);
    }
    return *out;
}
#else
typedef auth_hash_type_t hashid;
#endif

/**
 * @brief transforme un mot de passe pour une inscription dans les log
 * 
 * l'objet est de ne pas indiquer le vrai mot de passe
 * 
 * @param pwd 
 * @return char* 
 */
static char *auth_pwd_hash_frob(hashid id, auth_pwd_t const *pwd, bool bdc_out, char **out, size_t *len)
{
    (void)id;
    assert(pwd);
    assert(out);
    assert((!bdc_out && len) || bdc_out);
    size_t _len;
    if (!len) len = &_len;
    *len = 0;
    if (!pwd->len || !pwd->buf) return NULL;
    *out = memdup(pwd->buf, pwd->len);
    *len = pwd->len;
    if (*out)
    {
        memfrob(*out, pwd->len);
        if (bdc_out) *out = auth_pwd_hash2bdc(out, len);
    }
    return *out;
}

static auth_hash_type_t _auth_pwd_hash_def =
#ifdef WITH_MHASH
 _auth_pwd_hash_sha256_
#else
_auth_pwd_hash_frob_;
#endif
;

#define stringizing(a) #a

static const struct _auth_pwd_hash_fct_t {
    char *(*fct)(hashid hash_id, auth_pwd_t const *pwd, bool bdc_out, char **out, size_t *len);
    hashid hash_id;
    const char *lib;
} _auth_pwd_hash_fct[_auth_pwd_hash_no_] =
{
    [_auth_pwd_hash_frob_]={
        .fct = auth_pwd_hash_frob,
        .hash_id = _auth_pwd_hash_frob_,
        .lib = stringizing(_auth_pwd_hash_frob_),
    },

#ifdef WITH_MHASH
    [_auth_pwd_hash_sha256_]={
        .fct = auth_pwd_mhash,
        .hash_id = MHASH_SHA256,
        .lib = stringizing(_auth_pwd_hash_sha256_),
    },
    [_auth_pwd_hash_md5_]={
        .fct = auth_pwd_mhash,
        .hash_id = MHASH_MD5,
        .lib = stringizing(_auth_pwd_hash_md5_),
    },
#else
    [_auth_pwd_hash_frob_ + 1 ... _auth_pwd_hash_no_ - 1]={
        .fct = auth_pwd_hash_frob,
        .hash_id = _auth_pwd_hash_frob_,
        .lib = stringizing(_auth_pwd_hash_frob_),
    },
#endif
};

/**
 * @brief transforme un mot de passe pour une inscription dans les log
 * 
 * l'objet est de ne pas indiquer le vrai mot de passe
 * 
 * @param pwd 
 * @return char* 
 */
char *auth_pwd_hash(auth_pwd_t const *pwd, bool bdc_out, char **out, size_t *len)
{
    if (pwd)
    {
        const struct _auth_pwd_hash_fct_t *hash = &_auth_pwd_hash_fct[_auth_pwd_hash_def];
        return hash->fct(hash->hash_id, pwd, bdc_out, out, len);
    }
    *out = NULL;
    return *out;
}

void auth_pwd_hash_type_set(auth_hash_type_t hash_type) {_auth_pwd_hash_def=hash_type;}
auth_hash_type_t auth_pwd_hash_type_get(void) {return _auth_pwd_hash_def;}
const char *auth_pwd_hash_type_lib_get(auth_hash_type_t hash_type)
{
    return (hash_type<_auth_pwd_hash_no_)?_auth_pwd_hash_fct[hash_type].lib:"unknown";
}
/**@}*/