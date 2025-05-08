/**
* @file cipher.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_FIELDS_CIPHER_H
#define RSMP_FIELDS_CIPHER_H

#include "rsmp/vars/array.h"
#include "rsmp/vars/str.h"
/**
 * @defgroup cipher 
 * 
 * @brief 
 * 
 * @ingroup fields
 * @{
 */

#ifdef RSMP_FIELDS_CIPHER_C
#define CIPHER_EXTERN
#define CIPHER_PRIVATE
#endif

#ifndef CIPHER_EXTERN
#define CIPHER_EXTERN extern
#endif

#ifndef CIPHER_PRIVATE
#define CIPHER_PRIVATE const
#endif

typedef str_t cipher;

rsmp_array_declare(cipher);

// #define rsmp_cipher_set    rsmp_str_set 
// #define rsmp_cipher_free   rsmp_str_free 


#define __rsmp_field_cipher_encode __rsmp_field_str_encode 
#define __rsmp_field_cipher_decode __rsmp_field_str_decode


/**@}*/
#endif //FIELDS_CIPHER_H
