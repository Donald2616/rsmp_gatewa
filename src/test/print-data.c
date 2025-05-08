
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/// @brief affichage de l'ascii des valeurs d'un tableau
/// @param ascii 
/// @param dataLen 
void print_ascii(char* data, size_t dataLen) {
    
    for (size_t i = 0; i < dataLen; i++) {
        if (isprint(data[i])) {
            printf("%c", data[i]);
        } else {
            printf(".");
        }
    }
    printf("\n");
}

/// @brief affichage de l'hex des valeurs d'un tableau
/// @param ascii 
/// @param dataLen 
void print_hex(char* data, size_t dataLen) {
    // printf("Hex values: ");
    for (size_t i = 0; i < dataLen; i++) {
        printf("%02x", (unsigned char)data[i]);
    }
    printf("\n");
}



// affichage des caractères ascci à partir d'une chaine hexadécimale (où chaque octet est représenté par 2 caractères hexadécimaux, terminée par '\0')
// exemple: "48656c6c6f20576f726c6421" -> "Hello World!"
int print_ascii_from_hex(const char *hexString) {
    size_t hexLen = strlen(hexString);
    if (hexLen % 2 != 0) {
        printf("Invalid hexadecimal string\n");
        return -1;
    }

    size_t dataLen = hexLen / 2;
    char data[dataLen + 1];
    data[dataLen] = '\0';

    // convertion hex en ascii
    for (size_t i = 0; i < dataLen; i++) {
        char hex[3] = {hexString[i * 2], hexString[i * 2 + 1], '\0'};
        int value;
        sscanf(hex, "%x", &value);
        data[i] = (char)value;
    }

    // affichage
    print_ascii(data, dataLen);

    return 0;
}