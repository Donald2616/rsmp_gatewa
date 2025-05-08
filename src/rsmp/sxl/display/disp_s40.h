#ifndef DISP_S40_H
#define DISP_S40_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @file disp_s40.h
 * @brief Structures C pour le message DISP/S40 - Lecture des messages actifs.
 */

/**
 * @struct DisplayPoolElementObject
 * @brief Représente une entrée dans le display pool (un message actif).
 */
typedef struct {
    char msgId[64];                 ///< ID du message affiché
    uint32_t maxWaitTime;          ///< Temps d’attente max (ms)
    uint32_t maxWaitTimeRemain;    ///< Temps d’attente restant (ms)
    uint32_t displayDuration;      ///< Durée d’affichage max (ms)
    uint32_t displayDurationRemain;///< Durée d’affichage restante (ms)
    uint32_t displayPriority;      ///< Priorité d’affichage (0-255)
    char dynamicData[128];         ///< Données dynamiques (wildcards, etc.)
} DisplayPoolElementObject;

/**
 * @struct DispS40StatusResponse
 * @brief Réponse complète au message DISP/S40.
 */
typedef struct {
    DisplayPoolElementObject* displayPoolList; ///< Liste des éléments du display pool
    unsigned int displayPoolCount;             ///< Taille de la liste
} DispS40StatusResponse;

#endif // DISP_S40_H
