#ifndef DISP_M40_H
#define DISP_M40_H

#include "disp_m31.h"  // Pour RetStatusObject

/**
 * @file disp_m40.h
 * @brief Structures et prototypes pour le signal DISP/M40 - Demande d'activation d'un message
 */

/// Constantes de comportement d'affichage
#define DISPLAY_BEHAVIOR_DROP "DROP"
#define DISPLAY_BEHAVIOR_STORE "STORE"

/**
 * @brief Requête DISP/M40 : activation d'un message
 */
typedef struct {
    char msgId[64];
    char displayBehavior[16];
    unsigned int maxWaitTime;
    unsigned int displayDuration;
    unsigned int displayPriority;
    char dynamicData[256];
} DispM40CommandRequest;

/**
 * @brief Réponse DISP/M40 après traitement de l'activation
 */
typedef struct {
    char msgId[64];
    char behaviorUsed[16];
    RetStatusObject retStatus;
} DispM40CommandResponse;

// Fonctions de libération (placeholder)
void free_disp_m40_command_request(DispM40CommandRequest* req);
void free_disp_m40_command_response(DispM40CommandResponse* res);

#endif // DISP_M40_H
