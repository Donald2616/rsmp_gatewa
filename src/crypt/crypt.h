/**
* @file aes.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2024-01-24
* 
* @copyright Copyright (c) 2024
* 
*/

#include "cipher/cipher.h"

#ifndef CRYPT_AES_H
#define CRYPT_AES_H
/**
 * @defgroup aes 
 * 
 * @brief Abstraction des encodages et décodages AES
 * 
 * @{
 */

#ifdef CRYPT_CRYPT_C
#define AES_EXTERN
#define AES_PRIVATE
#endif

#ifndef AES_EXTERN
#define AES_EXTERN extern
#endif

#ifndef AES_PRIVATE
#define AES_PRIVATE const
#endif

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


extern cipher_t /*const*/ _cipher_aes128_ctr;
// #define KEY_SIZE 16
// #define IV_SIZE 16 // 128 bits en ctr (ciphers depreciated)
#define IV_SIZE 12

// IV utilisé pour le chiffrement dans l'example test-0.txt
#define IV_EXAMPLE_TEST_0 "6ee54432e65f3722455732d4eb389b42"
#define IV_EXAMPLE IV_EXAMPLE_TEST_0
// \todo voir besoins TU sur le dechiffrement et nettoyer le code

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


extern cipher_t /*const*/ _cipher_aes256_ctr;

///////////////////////////////////////////////////////////////////////
extern cipher_t /*const*/ _cipher_aes256_gcm;


///////////////////////////////////////////////////////////////////////
extern cipher_t /*const*/ _cipher_disabled;

/**@}*/
#endif //CRYPT_AES_H