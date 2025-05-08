/**
* @file cipher.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-01-24
* 
* @copyright Copyright (c) 2024
* 
*/
#include "config.h"

#include <stdio.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/rand.h>

#include "crypt/bcd.h"


#define CIPHER_CIPHER_C
#include "cipher.h"
#include "crypt/crypt.h"

/**
 * @ingroup cipher
 * @{
 */

static cipher_t *_cipher_list[__CIPHER_MAX__] =
{
    [0 ... __CIPHER_MAX__ -1] = NULL,
    [__cipher_aes_128_ctr__] = &_cipher_aes128_ctr,
    [__cipher_aes_256_ctr__] = &_cipher_aes256_ctr,
    [__cipher_aes_256_gcm__] = &_cipher_aes256_gcm,
};

char const *_cipher_name[__CIPHER_MAX__] =
{
   [__cipher_aes_128_ctr__] = _cipher_aes_128_ctr_lib_, 
   [__cipher_aes_256_ctr__] = _cipher_aes_256_ctr_lib_, 
   [__cipher_aes_256_gcm__] = _cipher_aes_256_gcm_lib_, 
};

/*

                          88           88  88
                          88           88  ""
                          88           88
8b,dPPYba,   88       88  88,dPPYba,   88  88   ,adPPYba,
88P'    "8a  88       88  88P'    "8a  88  88  a8"     ""
88       d8  88       88  88       d8  88  88  8b
88b,   ,a8"  "8a,   ,a88  88b,   ,a8"  88  88  "8a,   ,aa
88`YbbdP"'    `"YbbdP'Y8  8Y"Ybbd8"'   88  88   `"Ybbd8"'
88
88
*/
// salt utilisé lors de la dérivation de clé
#define __SALT "13BAC1F20AF1A5F08F9E962CD632B19B"

static struct {
    unsigned char *buf;
    size_t len;
} _key_gen_salt = {.len=0, .buf=NULL};

static void salt_atexit(void)
{
    if (_key_gen_salt.buf) free(_key_gen_salt.buf);
}

int cipher_key_gen(char const *in, size_t in_len, int iterations, size_t key_len, char **key)
{

    if (!_key_gen_salt.buf)
    {
        _key_gen_salt.buf = (unsigned char *)bcd_decode(__SALT, &_key_gen_salt.len);
        assert(_key_gen_salt.buf && _key_gen_salt.len);
        atexit(salt_atexit);
    }
    *key = malloc(key_len); // Allocate memory for derived_key

    int derivationStatus = PKCS5_PBKDF2_HMAC(in, in_len, _key_gen_salt.buf, _key_gen_salt.len, iterations, EVP_sha256(), key_len, (unsigned char*)*key);

    if(derivationStatus != 1) {
        if (*key) {
            free(*key);
            *key = NULL;
        }
        return 1;
    }

    return 0;
}

/**
 * @brief Boucle sur la liste des cipher
 * 
 * 
 * @param callback callback sur chaque item
 * @param context  context d'exécution du callback
 * @return int 
 */
int cipher_foreach(cipher_foreach_cb_t callback, void *context)
{
    int ret = 0;
    if (callback)
    {
        for(size_t i=0; i<__CIPHER_MAX__; i++)
        {
            callback(context, _cipher_list[i]);
            ret++;
        }
    }
    return ret;
}

/**
 * @brief récupération d'un cipher
 * 
 * @param cipher     numéro du cipher
 * @return cipher_t* pointeur sur le cipher choisi
 */
cipher_t const *cipher_get(cipher_enum_t cipher)
{
    return cipher<__CIPHER_MAX__ ? _cipher_list[cipher] : NULL;
}

#ifdef TEST_ENABLED
cipher_t const *cipher_get_fake_one_disabling_encryption()
{
    return &_cipher_disabled;
}
#endif

static inline cipher_t *__cipher_get_by_str(char const *name)
{
    if (name)
    {
        for(size_t i = 0; i<__CIPHER_MAX__; i++)
        {
            if (strcmp(name, _cipher_list[i]->lib)==0) return _cipher_list[i];
        }
    }
    return NULL;
}

/**
 * @brief récupération d'un cipher à partir de son nom
 * 
 * @param name       nom du cipher
 * @return cipher_t* pointeur sur 4le cipher choisi
 */
cipher_t  const *cipher_get_by_str(char const *name)
{
    return __cipher_get_by_str(name);
}


bool cipher_let_enabled_by_str(char const *name, bool is_enabled)
{
    bool ret = false;
    cipher_t *cipher = __cipher_get_by_str(name);
    if (cipher)
    {
        if (is_enabled != cipher->enabled)
        {
            _log_info("cipher %s %s", name, is_enabled?"enabled":"disabled");
        }
        cipher->enabled = is_enabled;
        ret = true;
    }
    return ret;
}


static int cipher_list_callback(void *context, const cipher_t *cipher)
{
    int fd = *((int *)context);
    dprintf(fd, "\t%s%s\n", cipher->lib, cipher->enabled?"":" --disabled--");
    return 1;
}

int cipher_list(int fd)
{
    dprintf(fd,"List of ciphers:\n");
    return cipher_foreach(cipher_list_callback, &fd);
}

int rsmp_cipher_pwd_check(uuid_t id, char const* pwd, char const *pwd_footprint)
{
	//-- encode pwd
	//-- vérifie les deux empreintes...
	//-- log le résultat
	//-- retourne le résultats
	assert(0); //-- Code vide !
	return 0;
}

/**@}*/