/**
* @file base64.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-06-27
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef CRYPT_BASE64_H
#define CRYPT_BASE64_H
#include <stdio.h>
/**
 * @defgroup base64 
 * @ingroup cipher
 * @brief encodage/décodage base64
 * 
 * @{
 */

/**
 * @brief Encode un buffer en base64
 * 
 * Le buffer de sortie est alloué dynamiquement, c'est à 
 * l'appellant de le libérer
 * 
 * @param[in]  input  pointeur sur le buffer de caractère
 * @param[in]  length longueur du buffer d'entrée
 * @param[out] out    pointeur sur le buffer de sortie
 * @return int ==0 Ok, !=0 KO
 */
int base64_encode(const unsigned char* input, size_t length, char **out);
/**
 * @brief decode un buffer base64
 * 
 * Le buffer de sortie est alloué dynamiquement, c'est à 
 * l'appellant de le libérer
 * 
 * @param[in]  input           pointeur sur le buffer de caractère
 * @param[out] out             pointeur sur le buffer de sortie
 * @param[out] output_length   longueur de la chaîne décodée
 * @return int  ==0 Ok, !=0 KO
 */
int base64_decode(const char* input, char **out, size_t* output_length);

/**@}*/
#endif //CRYPT_BASE64_H