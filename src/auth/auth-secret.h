/**
* @file auth-secret.h
* @author your name (you@domain.com)
* @brief Gestion du secret de l'authentification
* @version 0.1
* @date 2024-03-10
* 
* @copyright Copyright (c) 2024
* 
*/
#ifndef AUTH_AUTH_SECRET_H
#define AUTH_AUTH_SECRET_H

/**
 * @defgroup auth-secret 
 * 
 * @brief Gestion du secret
 * 
 * @{
 */

typedef struct auth_secret_t
{
    size_t len;
    char *buf;
} auth_secret_t;

void auth_secret_swap(auth_secret_t *s1, auth_secret_t *s2);
auth_secret_t const *auth_secret_cpy(auth_secret_t *dest, auth_secret_t const *src);
auth_secret_t const *auth_secret_release(auth_secret_t *item);

/**@}*/
#endif //AUTH_AUTH-SECRET_H