/**
* @file cipher.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-01-24
* 
* @copyright Copyright (c) 2024
* 
*/
#ifndef CIPHER_CIPHER_H
#define CIPHER_CIPHER_H
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <uuid/uuid.h>
/**
 * @defgroup cipher 
 * 
 * @brief Abstraction des ciphers disponibles
 * 
 * @{
 */

#ifdef CIPHER_CIPHER_C
#define CIPHER_EXTERN
#define CIPHER_PRIVATE
#endif

#ifndef CIPHER_EXTERN
#define CIPHER_EXTERN extern
#endif

#ifndef CIPHER_PRIVATE
#define CIPHER_PRIVATE const
#endif

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
typedef int(*cipher_encrypt_t)(void *context, char  const *in, size_t in_len, char const *key, size_t key_len, char **out);

/**
 * @brief Décodage d'un bloc
 * 
 * Encodage d'un bloc avec mise en base64 du résultat.
 * 
 * \param[inout] context context d'exécution du cipher
 * \param[in]    in pointeur sur la zone à décodée et en base64
 * \param[in]    key  clé associée
 * \param[out]   out  pointeur sur l'adresse du conteneur de la zone en clair
 * \param[out]   out_len longueur de la zone en clair
 * \return ==0 en cas de succès, !=0 en cas d'échec
 */
typedef int(*cipher_decrypt_t)(void *context, char const *in, char const *key, size_t key_len, char **out, size_t *out_len);

/**
 * @brief Initialisation d'un environnement de cipher
 * 
 * \param[out] context pointeur qui contiendra l'adresse du context alloué
 * \return ==0 ok, !=0 échec
 */
typedef int(*cipher_context_new_t)(void **context);

/**
 * @brief libération d'un environnement de cipher
 * 
 * \param[out] context pointeur qui contiendra l'adresse du context alloué
 * \return ==0 ok, !=0 échec
 */
typedef int(*cipher_context_free_t)(void **context);

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
typedef int(*cipher_key_gen_t)(char const *in, size_t in_len, int iteration, char **key, size_t *key_len);

/**
 * @brief Description d'un cipher
 * 
 * C'est un objet statique en singleton...
 */
typedef struct cipher
{
    char const *                         lib;          /*!< \brief libellé du cipher il doit être normalisé */
    CIPHER_PRIVATE cipher_context_new_t  context_new;  /*!< \brief Création d'un context d'exécution du cipher */
    CIPHER_PRIVATE cipher_context_free_t context_free; /*!< \brief libération du contexte d'exécution du cipher */
    CIPHER_PRIVATE cipher_encrypt_t      encrypt;      /*!< \brief encodage chiffrement + mise en base64 */
    CIPHER_PRIVATE cipher_decrypt_t      decrypt;      /*!< \brief decodage extraction base 64 puis déchiffrement */
    CIPHER_PRIVATE cipher_key_gen_t      key_gen;      /*!< \brief génération de la clé pour une source donnée */ 
    CIPHER_PRIVATE bool                  enabled;      /*!< \brief cipher autorisé ou non */ 
} cipher_t;

/** liste des identifiant de cipher */
typedef enum {
    __cipher_aes_128_ctr__ = 0,
    __cipher_aes_256_ctr__,
    __cipher_aes_256_gcm__,
    __CIPHER_MAX__,
    #ifdef TEST_ENABLED
    __CIPHER_DISABLED__
    #endif
} cipher_enum_t;

#define _cipher_aes_128_ctr_lib_ "AES128CTR"
#define _cipher_aes_256_ctr_lib_ "AES256CTR"
#define _cipher_aes_256_gcm_lib_ "AES256GCM"
#define _cipher_disabled_lib_ "DISABLED"

/**
 * @brief Type de callback pour le parcours des ciphers
 * 
 * \param context context d'éxécution
 * \param cipher description du cipher
 */
typedef int(*cipher_foreach_cb_t)(void *context, const cipher_t *cipher);

/**
 * @brief Boucle sur la liste des cipher
 * 
 * 
 * @param callback callback sur chaque item
 * @param context  context d'exécution du callback
 * @return int 
 */
int cipher_foreach(cipher_foreach_cb_t callback, void *context);

/**
 * @brief récupération d'un cipher
 * 
 * @param cipher     numéro du cipher
 * @return cipher_t* pointeur sur le cipher choisi
 */
cipher_t const *cipher_get(cipher_enum_t cipher);


#ifdef TEST_ENABLED
/**
 * @brief récupération du fake cipher désactivant le chiffrement
*/
cipher_t const *cipher_get_fake_one_disabling_encryption(void);
#endif
/**
 * @brief récupération d'un cipher à partir de son nom
 * 
 * @param name       nom du cipher
 * @return cipher_t* pointeur sur le cipher choisi
 */
cipher_t  const *cipher_get_by_str(char const *name);

bool cipher_let_enabled_by_str(char const *name, bool is_enabled);

int cipher_list(int fd);


#define SALT_SIZE 16
#define ITERATIONS 1000



int cipher_key_gen(char const *in, size_t in_len, int iterations, size_t key_len, char **key);

typedef void rsmp_cipher_t;

int rsmp_cipher_pwd_check(uuid_t id, char const* pwd, char const *pwd_footprint);

/**@}*/
#endif //CIPHER_CIPHER_H