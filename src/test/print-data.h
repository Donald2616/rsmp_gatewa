#ifndef PRINT_DATA_H
#define PRINT_DATA_H

#include <stddef.h>

/// @brief affichage de l'ascii des valeurs d'un tableau
/// @param data 
/// @param dataLen 
void print_ascii(char* data, size_t dataLen);

/// @brief affichage de l'hex des valeurs d'un tableau
/// @param data 
/// @param dataLen 
void print_hex(char* data, size_t dataLen);

/// @brief affichage des caractères ascci à partir d'une chaine hexadécimale
/// @param hexString 
int print_ascii_from_hex(const char *hexString);

#endif // PRINT_DATA_H