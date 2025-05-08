/**
* @file aes.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-01-24
* 
* @copyright Copyright (c) 2024
* 
*/
#include "config.h"
#include <assert.h>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>


#define CRYPT_CRYPT_C
#include "base64.h"
#include "crypt.h"
#include "bcd.h"

static int _get_ivi_size(EVP_CIPHER *evp);

/**
 * @ingroup aes
 * @{
 */

/**
 * @brief chiffre en aes ctr
 * 
 * Applique un chiffrement aes avec une clé sur un buffer en clair "in"
 * 
 * La sortie contient l'iv + données chiffrées encodées en base64
 * 
 * @param[inout] ctx contexte 
 * @param[in]    evp longueur aes 128 ou 256
 * @param[in]    in  buffer en clair
 * @param[in]    in_len longueur du buffer 
 * @param[in]    key clé 
 * @param[in]    key_len longueur de la clé 
 * @param[out]   out pointeur sur le pointeur de la chaîne de sortie en base64 
 * @return int ==0 Ok, !=0 KO
 */
static inline int encrypt_aes(EVP_CIPHER_CTX *ctx, EVP_CIPHER * evp, const char *in, size_t in_len, const char *key, size_t key_len,  char **out_b64)
{
    int ret = 1;
    assert(ctx);
    assert(key);
    assert(in);
    assert(out_b64);    
    assert(in_len>0);
    (void)key_len;
    size_t iv_size = _get_ivi_size(evp);


    _log_dump_debug_out(in, strlen(in), "chaine claire ");
    char* iv;
// #ifdef TEST_ENABLED
//     char *iv_str = IV_EXAMPLE; // reprend la valeur de l'exemple bash+openssl test-0.txt (qui avait été généré aléatoirement)
//     size_t iv_len;
//     iv = bcd_decode(iv_str, &iv_len);
//     assert(iv_len == _get_ivi_size(evp));
// #else
    //-- Initialisation du vecteur
    iv = malloc(iv_size);
    assert(iv);
    RAND_bytes(iv, iv_size);
// #endif
    if (EVP_CIPHER_mode(evp) == EVP_CIPH_GCM_MODE) {
        EVP_EncryptInit_ex(ctx, evp, NULL, NULL, NULL);
        if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_size, NULL)) {
            goto error_free_iv;
        }
        if (EVP_EncryptInit_ex(ctx, NULL, NULL, (const unsigned char *)key, (const unsigned char *)iv) != 1) {
            goto error_free_iv;
        }
    } else {
        if (EVP_EncryptInit_ex(ctx, evp, NULL, (const unsigned char *)key, (const unsigned char *)iv) != 1) {
            goto error_free_iv;
        }
    }

    int ciphertext_len, len;

    unsigned char * ciphertext = malloc(in_len + EVP_MAX_BLOCK_LENGTH /* padding au cas où mais normalement pas besoin en CTR */);
    if(!ciphertext) {
        goto error_free_iv;
    }
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, (const unsigned char *)in, in_len) != 1) {
        goto error_free_iv_ciphertext;
    }

    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
        goto error_free_iv_ciphertext;
    }
    ciphertext_len += len;
///////////////////////////////////
    #define TAG_SIZE 16
    unsigned char tag[TAG_SIZE];
    if (EVP_CIPHER_mode(evp) == EVP_CIPH_GCM_MODE) {
        if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE, tag)) {
            goto error_free_iv_ciphertext;
        }
    }
///////////////////////////////////
    

    // printf("ciphertext = ");
    // print_hex((char *)ciphertext, ciphertext_len);
size_t data_len = 0;
unsigned char *data = malloc(data_len = iv_size);
    if (!data) goto error_free_iv_ciphertext;

    // Copier iv et out_bin dans data
    memcpy(data, iv, iv_size);
    

    
// test si evp est aes 256 gcm
// copier le tag d'authentification obtenu lors du chiffrement
///////////////////////////////////
    if (EVP_CIPHER_mode(evp) == EVP_CIPH_GCM_MODE) {
        data_len += TAG_SIZE;
        unsigned char *realloc_data = realloc(data, data_len);
        if(realloc_data) {
            data = realloc_data;
            memcpy(data + iv_size, tag, TAG_SIZE);
        }
        else {
            goto error_free_iv_ciphertext_data;
        }
    }
///////////////////////////////////
    // copier la chaine cryptée à la fin de data
    data_len += ciphertext_len;
    unsigned char *realloc_data = realloc(data, data_len);
    if(realloc_data) {
        data = realloc_data;
        memcpy(data + data_len - ciphertext_len, ciphertext, ciphertext_len);
    }
    else {
        goto error_free_iv_ciphertext_data;
    }

    // Passer data à base64_encode
    int b64Status = base64_encode(data, data_len, (char **)out_b64);
    if (b64Status != 0) {
        goto error_free_iv_ciphertext_data;
    }

    // _log_dump_info_out(*out_b64, strlen(*out_b64), "chaine encodee ");
    ret = 0;

error_free_iv_ciphertext_data:
    free(data);
error_free_iv_ciphertext:
    free(ciphertext);
error_free_iv:
    free(iv);
    return ret;
}

/**
 * @brief déchiffre en aes ctr
 
 * @param[inout] ctx contexte 
 * @param[in]    evp longueur aes 128 ou 256
 * @param[in]    in  buufer chiffré et encodé en base64
 * @param[in]    key clé 
 * @param[in]    key_len longueur de la clé 
 * @param[out]   out pointeur sur le pointeur de la chaîne de sortie en base64 
 * @param[out]   out_len  longueur de du buffer de sortie
 * @return int 
 */
static int decrypt_aes(EVP_CIPHER_CTX *ctx, EVP_CIPHER * evp, char const *in_b64, char const *key, size_t key_len, char **out, size_t *out_len)
{
    assert(ctx);
    assert(in_b64);
    assert(key);
    assert(out);
    assert(out_len);
    int ret = -1;

    // _log_dump_info_in(in_b64, strlen(in_b64), "decodage de ");

    int len;
    size_t plaintext_len = 0;
    char *data; // buffer pour iv + tag (en gcm) + ciphertext
    size_t data_len;
    base64_decode(in_b64, &data, &data_len);
    if(!data) {
        _log_err("base64_decode");
        return ret;
    }

    char *iv = data;
    
    size_t iv_size = _get_ivi_size(evp);
    // printf("iv = ");
    // print_hex(iv, iv_size);
    data_len -= iv_size;
    char *ciphertext = data + iv_size;
    char *tag = NULL;
  
    // préparation du contexte de déchiffrement et initialisation du vecteur d'initialisation et du tag (en gcm)
    switch(EVP_CIPHER_mode(evp)) {
        case EVP_CIPH_CTR_MODE:
            assert(iv_size == 16);
            if (EVP_DecryptInit_ex(ctx, evp, NULL, (const unsigned char *)key, (const unsigned char *)iv) != 1)
            {
                _log_err("EVP_DecryptInit_ex");
                goto free_data;
            }
            break;
        case EVP_CIPH_GCM_MODE:
            // choix algo gcm
            if (!EVP_DecryptInit_ex(ctx, evp, NULL, NULL, NULL)) {
                _log_err("EVP_DecryptInit_ex");
                goto free_data;
            }
            // taille de l'IV (appel utile si différent de valeur par défaut 12)
            if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_size, NULL)) {
                _log_err("EVP_CIPHER_CTX_ctrl");
                goto free_data;
            }

            // initialisation du contexte de déchiffrement avec la clé et l'IV
            if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) {
                _log_err("EVP_DecryptInit_ex");
                goto free_data;
            }
            tag = data + iv_size;
            data_len -= TAG_SIZE;
            ciphertext += TAG_SIZE;

            break;
        default:
            _log_err("EVP_CIPHER_mode");
            goto free_data;
    }
    // printf("ciphertext = ");
    // print_hex(ciphertext, data_len - iv_size);

    // Déchiffrement
    *out = malloc(data_len /* EVP_MAX_BLOCK_LENGTH le padding est inutile dans une opération de déchiffrement en CTR ou GCM*/ + 1/* pour ajout fin de chaine*/);
    if (!*out) {
        _log_err("malloc");
        goto free_data;
    }

    if (!EVP_DecryptUpdate(ctx, (unsigned char *)*out, &len, (const unsigned char *)ciphertext, data_len)) {
        _log_err("EVP_DecryptUpdate");
        goto free_out_data;
    }

            // initialisation du tag
            if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_SIZE, tag)) {
                _log_err("EVP_CIPHER_CTX_ctrl");
                goto free_data;
            }

    int unused;
    if (!EVP_DecryptFinal_ex(ctx, NULL, &unused)) {
        unsigned long err_code = ERR_get_error();
        _log_debug("Error EVP_DecryptFinal_ex"); // niveau debug pour éviter de polluer les logs avec les challenges (ping encodé avec nouvelle clé)
        if(err_code != 0) {
            const char* err_msg = ERR_reason_error_string(err_code);
            const char* lib_name = ERR_lib_error_string(err_code);
            const char* func_name = ERR_func_error_string(err_code);
            _log_err("error code: %lu\n", err_code);
            _log_err("error ptxt: %s\n", err_msg);
            _log_err("lib name: %s\n", lib_name);
            _log_err("func name: %s\n", func_name);
        }
        goto free_out_data;
    }

    plaintext_len += len; 
    *out_len = plaintext_len;
    (*out)[plaintext_len] = '\0';

    ret = 0;

    // on vérifie la cohérence des tailles de buffer
    // assert(plaintext_len == (data_len - iv_size)); // CTR
free_out_data:
    if (ret != 0) {
        free(*out);
    }
free_data:
    free(data);

    return ret;
}


/**
 * @brief Génération d'une clé
 * 
 * \param[in]  in     source de dérivation de la clé
 * \param[in]  in_len longueur de la sourced e dérivation de la clé
 * \param[in]  iter   nombre d'itération pour générer la clé
 * \param[in] key_len longueur de la clé
 * \param[out] key    la clé
 * \return ==0 ok, !=0 échec
 */
static int cipher_aes_key_gen(char const *in, size_t in_len, int iterations, size_t key_len, char **key)
{
    assert(key_len == 16||key_len == 32);
    return cipher_key_gen(in, in_len, iterations, key_len, key);
}

/**
 * @brief Initialisation d'un environnement de cipher
 * 
 * \param[out] context pointeur qui contiendra l'adresse du context alloué
 * \return ==0 ok, !=0 échec
 */
static int cipher_aes_context_new(void **context)
{
    if (context)
    {
        *context = EVP_CIPHER_CTX_new();
        return (*context)?0:-1;
    }
    return -1;
}

/**
 * @brief libération d'un environnement de cipher
 * 
 * \param[out] context pointeur qui contiendra l'adresse du context alloué
 * \return ==0 ok, !=0 échec
 */
static int cipher_aes_context_free(void **context)
{
    if (context)
    {
        EVP_CIPHER_CTX_free(*context);
    }
    return 0;
}


/**
 * @defgroup aes-128-ctr
 * @{
 */

/*

       db         88888888888  ad88888ba             88   ad888888b,   ad88888ba
      d88b        88          d8"     "8b          ,d88  d8"     "88  d8"     "8b
     d8'`8b       88          Y8,                888888          a8P  Y8a     a8P
    d8'  `8b      88aaaaa     `Y8aaaaa,              88       ,d8P"    "Y8aaa8P"
   d8YaaaaY8b     88"""""       `"""""8b,  aaaaaaaa  88     a8P"       ,d8"""8b,
  d8""""""""8b    88                  `8b  """"""""  88   a8P'        d8"     "8b
 d8'        `8b   88          Y8a     a8P            88  d8"          Y8a     a8P
d8'          `8b  88888888888  "Y88888P"             88  88888888888   "Y88888P"


*/


/**
 * @brief Encodage d'un bloc
 * 
 * Encodage d'un bloc avec mise en base64 du résultat.
 * 
 * \param[inout] context context d'exécution du cipher
 * \param[in]    in pointeur sur la zone à encoder
 * \param[in]    in_len longueur de la zone à encoder
 * \param[in]    key  clé associée
 * \param[out]   out  pointeur sur l'adresse du conteneur de la zone chiffré et encodée en base64
 * \return ==0 en cas de succès, !=0 en cas d'échec
 */
static int cipher_aes128_ctr_encrypt(void *context, char  const *in, size_t in_len, char const *key, size_t key_len, char **out)
{
    return encrypt_aes(context, EVP_aes_128_ctr(), in, in_len, key, 16, out);
}

/**
 * @brief Décodage d'un bloc
 * 
 * Encodage d'un bloc avec mise en base64 du résultat.
 * 
 * \param[inout] context context d'exécution du cipher
 * \param[in]    in pointeur sur la zone à décodée et en base64
 * \param[in]    key  clé associée
 * \param[out]   out  pointeur sur l'adresse du conteneur de la zone en clair
 * \param[out]   ou_len longueur de la zone en clair
 * \return ==0 en cas de succès, !=0 en cas d'échec
 */
static int cipher_aes128_ctr_decrypt(void *context, char const *in, char const *key, size_t key_len, char **out, size_t *out_len)
{
    return decrypt_aes(context, EVP_aes_128_ctr(), in, key, key_len, out, out_len);
}


int cipher_key_gen(char const *in, size_t in_len, int iterations, size_t key_len, char **key);

/**
 * @brief Génération d'une clé
 * 
 * \param[in]  in     source de dérivation de la clé
 * \param[in]  in_len longueur de la sourced e dérivation de la clé
 * \param[in]  iter   nombre d'itération pour générer la clé
 * \param[out] key    la clé
 * \param[out] key_len longueur de la clé
 * \return ==0 ok, !=0 échec
 */
static int cipher_aes128_ctr_key_gen(char const *in, size_t in_len, int iterations, char **key, size_t *key_len)
{
    *key_len = 16;
    int ret = cipher_aes_key_gen(in, in_len, iterations, *key_len, key);

    return ret;
}
cipher_t _cipher_aes128_ctr =
{ 
    .lib          = _cipher_aes_128_ctr_lib_,
    .encrypt      = cipher_aes128_ctr_encrypt,
    .decrypt      = cipher_aes128_ctr_decrypt,
    .context_new  = cipher_aes_context_new,
    .context_free = cipher_aes_context_free,
    .key_gen      = cipher_aes128_ctr_key_gen,
    .enabled      = true,
};


/** @}*/ // aes-128-ctr

/**
 * @defgroup aes-256-ctr
 * @{
 */

/*

       db         88888888888  ad88888ba             ad888888b,  8888888888     ad8888ba,
      d88b        88          d8"     "8b           d8"     "88  88            8P'    "Y8
     d8'`8b       88          Y8,                           a8P  88  ____     d8
    d8'  `8b      88aaaaa     `Y8aaaaa,                  ,d8P"   88a8PPPP8b,  88,dd888bb,
   d8YaaaaY8b     88"""""       `"""""8b,  aaaaaaaa    a8P"      PP"     `8b  88P'    `8b
  d8""""""""8b    88                  `8b  """"""""  a8P'                 d8  88       d8
 d8'        `8b   88          Y8a     a8P           d8"          Y8a     a8P  88a     a8P
d8'          `8b  88888888888  "Y88888P"            88888888888   "Y88888P"    "Y88888P"


*/

/**
 * @brief Encodage d'un bloc
 * 
 * Encodage d'un bloc avec mise en base64 du résultat.
 * 
 * \param[inout] context context d'exécution du cipher
 * \param[in]    in pointeur sur la zone à encoder
 * \param[in]    in_len longueur de la zone à encoder
 * \param[in]    key  clé associée
 * \param[out]   out  pointeur sur l'adresse du conteneur de la zone chiffré et encodée en base64
 * \return ==0 en cas de succès, !=0 en cas d'échec
 */
static int cipher_aes256_ctr_encrypt(void *context, char  const *in, size_t in_len, char const *key, size_t key_len, char **out)
{
    return encrypt_aes(context, EVP_aes_256_ctr(), in, in_len, key, key_len, out);
}

/**
 * @brief Décodage d'un bloc
 * 
 * Encodage d'un bloc avec mise en base64 du résultat.
 * 
 * \param[inout] context context d'exécution du cipher
 * \param[in]    in pointeur sur la zone à décodée et en base64
 * \param[in]    key  clé associée
 * \param[out]   out  pointeur sur l'adresse du conteneur de la zone en clair
 * \param[out]   ou_len longueur de la zone en clair
 * \return ==0 en cas de succès, !=0 en cas d'échec
 */
static int cipher_aes256_ctr_decrypt(void *context, char const *in, char const *key, size_t key_len, char **out, size_t *out_len){
    
    return decrypt_aes(context, EVP_aes_256_ctr(), in, key, key_len, out, out_len);
}

/**
 * @brief Génération d'une clé
 * 
 * \param[in]  in     source de dérivation de la clé
 * \param[in]  in_len longueur de la sourced e dérivation de la clé
 * \param[in]  iter   nombre d'itération pour générer la clé
 * \param[out] key    la clé
 * \param[out] key_len longueur de la clé
 * \return ==0 ok, !=0 échec
 */
static int cipher_aes256_ctr_key_gen(char const *in, size_t in_len, int iterations, char **key, size_t *key_len)
{
    *key_len = 32;
    int ret = cipher_aes_key_gen(in, in_len, iterations, *key_len, key);

    return ret;
}

cipher_t _cipher_aes256_ctr =
{ 
    .lib          = _cipher_aes_256_ctr_lib_,
    .encrypt      = cipher_aes256_ctr_encrypt,
    .decrypt      = cipher_aes256_ctr_decrypt,
    .context_new  = cipher_aes_context_new,
    .context_free = cipher_aes_context_free,
    .key_gen      = cipher_aes256_ctr_key_gen,
    .enabled      = true,
};

/**@}*/ //-- aes-256-ctr

/*


:::'###::::'########::'######:::'#######::'########::'#######::::::::::::'######::::'######::'##::::'##:
::'## ##::: ##.....::'##... ##:'##.... ##: ##.....::'##.... ##::::::::::'##... ##::'##... ##: ###::'###:
:'##:. ##:: ##::::::: ##:::..::..::::: ##: ##::::::: ##::::..::::::::::: ##:::..::: ##:::..:: ####'####:
'##:::. ##: ######:::. ######:::'#######:: #######:: ########::'#######: ##::'####: ##::::::: ## ### ##:
 #########: ##...:::::..... ##:'##::::::::...... ##: ##.... ##:........: ##::: ##:: ##::::::: ##. #: ##:
 ##.... ##: ##:::::::'##::: ##: ##::::::::'##::: ##: ##:::: ##:::::::::: ##::: ##:: ##::: ##: ##:.:: ##:
 ##:::: ##: ########:. ######:: #########:. ######::. #######:::::::::::. ######:::. ######:: ##:::: ##:
..:::::..::........:::......:::.........:::......::::.......:::::::::::::......:::::......:::..:::::..::
https://patorjk.com/software/taag/#p=display&f=Banner3-D&t=aes256-GCM
*/

/**
 * @brief Encodage d'un bloc
 * 
 * Encodage d'un bloc avec mise en base64 du résultat.
 * 
 * \param[inout] context context d'exécution du cipher
 * \param[in]    in pointeur sur la zone à encoder
 * \param[in]    in_len longueur de la zone à encoder
 * \param[in]    key  clé associée
 * \param[out]   out  pointeur sur l'adresse du conteneur de la zone chiffré et encodée en base64
 * \return ==0 en cas de succès, !=0 en cas d'échec
 */
static int cipher_aes256_gcm_encrypt(void *context, char  const *in, size_t in_len, char const *key, size_t key_len, char **out)
{
    return encrypt_aes(context, EVP_aes_256_gcm(), in, in_len, key, key_len, out);
}

/**
 * @brief Décodage d'un bloc
 * 
 * Encodage d'un bloc avec mise en base64 du résultat.
 * 
 * \param[inout] context context d'exécution du cipher
 * \param[in]    in pointeur sur la zone à décodée et en base64
 * \param[in]    key  clé associée
 * \param[out]   out  pointeur sur l'adresse du conteneur de la zone en clair
 * \param[out]   ou_len longueur de la zone en clair
 * \return ==0 en cas de succès, !=0 en cas d'échec
 */
static int cipher_aes256_gcm_decrypt(void *context, char const *in, char const *key, size_t key_len, char **out, size_t *out_len){
    
    return decrypt_aes(context, EVP_aes_256_gcm(), in, key, key_len, out, out_len);
}

/**
 * @brief Génération d'une clé
 * 
 * \param[in]  in     source de dérivation de la clé
 * \param[in]  in_len longueur de la sourced e dérivation de la clé
 * \param[in]  iter   nombre d'itération pour générer la clé
 * \param[out] key    la clé
 * \param[out] key_len longueur de la clé
 * \return ==0 ok, !=0 échec
 */
static int cipher_aes256_gcm_key_gen(char const *in, size_t in_len, int iterations, char **key, size_t *key_len)
{
    *key_len = 32;
    int ret = cipher_aes_key_gen(in, in_len, iterations, *key_len, key);

    return ret;
}

cipher_t _cipher_aes256_gcm =
{ 
    .lib          = _cipher_aes_256_gcm_lib_,
    .encrypt      = cipher_aes256_gcm_encrypt,
    .decrypt      = cipher_aes256_gcm_decrypt,
    .context_new  = cipher_aes_context_new,
    .context_free = cipher_aes_context_free,
    .key_gen      = cipher_aes256_gcm_key_gen,
    .enabled      = true,
};

/**@}*/ //-- aes-256-gcm

#ifdef TEST_ENABLED
cipher_t _cipher_disabled =
    {
        .lib = _cipher_disabled_lib_,
        .encrypt = NULL,
        .decrypt = NULL,
        .context_new = NULL,
        .context_free = NULL,
        .key_gen = NULL,
        .enabled = false,
};
#endif

static int _get_ivi_size(EVP_CIPHER *evp)
{
        switch(EVP_CIPHER_mode(evp)) {
        case EVP_CIPH_CTR_MODE:
            return 16;
        case EVP_CIPH_GCM_MODE:
            return 12;
        default:
            _log_err("EVP_CIPHER_mode=%d", EVP_CIPHER_mode(evp));
            fflush(stdout);
            fflush(stderr);
            sleep(1);
            assert(0);
        }
}

/**@}*/