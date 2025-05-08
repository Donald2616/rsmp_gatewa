/**
 * @file base64.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CRYPT_BASE64_C
#include "base64.h"

/**
 * @ingroup base64
 * @{
 */

const unsigned char _base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_encode(const unsigned char *input, size_t length, char **out)
{
    size_t output_length = 4 * ((length + 2) / 3); // Taille de sortie approximative
    char *encoded_string = (char *)calloc(output_length + 1, sizeof(encoded_string));
    if (encoded_string == NULL)
    {
        _log_err("Erreur dans calloc");
        return -1;
    }

    size_t i, j;
    for (i = 0, j = 0; i < length; i += 3, j += 4)
    {
        unsigned char octet1 = input[i];
        unsigned char octet2 = (i + 1 < length) ? input[i + 1] : 0;
        unsigned char octet3 = (i + 2 < length) ? input[i + 2] : 0;

        encoded_string[j] = _base64_chars[octet1 >> 2];
        encoded_string[j + 1] = _base64_chars[((octet1 & 0x03) << 4) | (octet2 >> 4)];
        encoded_string[j + 2] = (i + 1 < length) ? _base64_chars[((octet2 & 0x0F) << 2) | (octet3 >> 6)] : '=';
        encoded_string[j + 3] = (i + 2 < length) ? _base64_chars[octet3 & 0x3F] : '=';
    }

    encoded_string[j] = '\0';
    *out = encoded_string;
    return 0;
}

int base64_decode(const char *input, char **out, size_t *output_length)
{
    size_t input_length = strlen(input);
    if (input_length % 4 != 0)
    {
        _log_err("La longueur de l'entrée base64 n'est pas valide.");
        return -1;
    }

    size_t decoded_length = (input_length / 4) * 3;
    if (input[input_length - 1] == '=')
    {
        decoded_length--;
        if (input[input_length - 2] == '=')
        {
            decoded_length--;
        }
    }

    unsigned char *decoded_data = (unsigned char *)calloc(decoded_length, sizeof(*decoded_data));
    if (decoded_data == NULL)
    {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        return -1;
    }

    size_t i, j;
    for (i = 0, j = 0; i < input_length; i += 4, j += 3)
    {
        unsigned char char1 = input[i];
        unsigned char char2 = input[i + 1];
        unsigned char char3 = input[i + 2];
        unsigned char char4 = input[i + 3];

        unsigned char index1 = 0, index2 = 0, index3 = 0, index4 = 0;
        size_t k;
        for (k = 0; k < 64; k++)
        {
            if (_base64_chars[k] == char1)
            {
                index1 = (unsigned char)k;
            }
            if (_base64_chars[k] == char2)
            {
                index2 = (unsigned char)k;
            }
            if (_base64_chars[k] == char3)
            {
                index3 = (unsigned char)k;
            }
            if (_base64_chars[k] == char4)
            {
                index4 = (unsigned char)k;
            }
        }

        decoded_data[j] = (index1 << 2) | (index2 >> 4);
        if (char3 != '=')
        {
            decoded_data[j + 1] = ((index2 & 0x0F) << 4) | (index3 >> 2);
        }
        if (char4 != '=')
        {
            decoded_data[j + 2] = ((index3 & 0x03) << 6) | index4;
        }
    }
    *out = (char *)decoded_data;
    *output_length = decoded_length;
    return 0;
}

/**@}*/