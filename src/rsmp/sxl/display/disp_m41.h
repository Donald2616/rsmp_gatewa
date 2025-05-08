#ifndef DISP_M41_H
#define DISP_M41_H

#include "disp_m31.h"  // Pour RetStatusObject


/**
 * @brief Structure pour la commande DISP/M41 - CommandRequest
 */
typedef struct {
    char msgId[64];              ///< Identifiant du message à désactiver
    unsigned int displayPriority;///< Priorité d'affichage du message
} DispM41CommandRequest;

/**
 * @brief Structure pour la réponse DISP/M41 - CommandResponse
 */
typedef struct {
    char msgId[64];              ///< Identifiant du message traité
    unsigned int displayPriority;///< Priorité du message traité
    RetStatusObject retStatus;   ///< Statut de la désactivation
} DispM41CommandResponse;

#endif // DISP_M41_H
