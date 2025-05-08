#ifndef DISP_M32_H
#define DISP_M32_H

/**
 * @file disp_m32.h
 * @brief Structures pour le signal DISP/M32 (Suppression des messages disponibles)
 */

#include <stdint.h>

// Réutilisation des types de retour de M31
#include "disp_m31.h"

/**
 * @brief Requête DISP/M32 : suppression de messages
 */
typedef struct {
    char** msgIdList;     /**< Tableau dynamique de chaînes à supprimer */
    int msgIdCount;       /**< Nombre total d'identifiants */
} DispM32CommandRequest;

/**
 * @brief Réponse DISP/M32 : liste des statuts par message supprimé
 */
typedef struct {
    RetMessageObject* retMessages; /**< Liste de statuts (réutilise M31) */
    int retMessageCount;
} DispM32CommandResponse;

// Déclarations de fonctions utilitaires (optionnelles)
extern void free_disp_m32_command_request(DispM32CommandRequest* req);
void free_disp_m32_command_response(DispM32CommandResponse* res);

#endif // DISP_M32_H
