/**
* @file auth-secret.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-03-10
* 
* @copyright Copyright (c) 2024
* 
*/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cipher/cipher.h"
#include "coder/coder.h"
#include "crypt/bcd.h"

#define AUTH_AUTH_SECRET_C
#include "auth.h"
#include "auth-secret.h"
#include "auth-random.h"

/**
 * @ingroup auth-secret
 * @{
 */
#ifdef YCO
static const cipher_t *_cipher =NULL;
static auth_secret_t *_secret = NULL;
static bool _initialized = false;

static int _iterations=1000;

static void auth_secret_free(auth_secret_t *secret)
{
    if (secret) 
    {
        auth_random(secret, sizeof(*secret));
        free(secret);
    }
}

static void auth_secret_atexit(void)
{
    auth_secret_free(_secret);
}

static void auth_secret_initialize(void)
{
    atexit(auth_secret_atexit);
    _cipher = cipher_get(__cipher_aes_256_ctr__);
}

static void auth_secret_create(auth_secret_t *secret)
{
    if (!_initialized) auth_secret_initialize();
    if (secret) auth_secret_free(secret);
    secret = calloc(1, sizeof(*secret));
    if (secret)
    {
        static char const *part="\x2b\xf5\xaf\xf2\x69\x7f\xef\x13\xe7\x48\x8b\xe2\xb8\x5d\xd3\x6c\x2a\xd3\x61\x13\x02\x28\x71\xc8\x61\xa1\x89\xf4\x68\xec\x82\x1e";
        //-- Il faut dériver une clé à partir d'une information connue et unique
        //-- et d'un sel (peut-être?)
        //-- ip l | egrep -om 1 -e 'link/ether [0-9:a-fA-F]+' | cut -d ' ' -f 2
        char *in;
        FILE *fd = popen("ip l | egrep -om 1 -e 'link/ether [0-9:a-fA-F]+' | cut -d ' ' -f 2", "r");
        if (fd)
        {
            char data[50];
            if (fgets(data, sizeof(data), fd))
            {
                char *in = malloc(32+6);
                if (in)
                {
                    char *s;
                    if ((s=strchr(data, '\n')) != NULL) *s='\0';
                    if ((s=strchr(data, '\r')) != NULL) *s='\0';
                    memcpy(in, part, 32);
                    bcd_decode(in+32, data);
                }
            }
            pclose(fd);
        }
        _cipher->key_gen(in, 32+6, _iterations, &secret->buf, &secret->len);
    }
}


auth_secret_t const *auth_secret_get(void)
{
    if (!_secret || !_secret->len || !_secret->buf) auth_secret_create(_secret);
    return _secret;
}


void auth_secret_interation(int iteration)
{
    if (iteration>=1000) _iterations = iteration;
}


int auth_secret_encrypt(char  const *in, size_t in_len, char **out, size_t *out_len)
{

}
#endif

auth_secret_t const *auth_secret_cpy(auth_secret_t *dest, auth_secret_t const *src)
{
    if (dest && src)
    {
        auth_secret_release(dest);
        if (src->len && src->buf)
        {
            dest->len = src->len;
            dest->buf = memdup(src->buf, src->len);
        }
    }
    return dest;
}

void auth_secret_swap(auth_secret_t *s1, auth_secret_t *s2)
{
    if (s1 && s2)
    {
        auth_secret_t s3={.len=0, .buf=NULL};
        auth_secret_cpy(&s3, s1);
        auth_secret_cpy(s1, s2);
        auth_secret_cpy(s2, &s3);
    }
}

auth_secret_t const *auth_secret_release(auth_secret_t *item)
{
    if (item)
    {
        if (item->buf)
        {
            auth_random(item->buf, item->len);
            free(item->buf);
            item->buf = NULL;
        }
        item->len = 0;
    }
    return item;
}


/**@}*/