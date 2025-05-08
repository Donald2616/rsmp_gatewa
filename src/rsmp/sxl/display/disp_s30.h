#ifndef DISP_S30_H
#define DISP_S30_H

#include <stddef.h>

#define MAX_STRING_SIZE 128

/**
 * @brief Structure représentant une réponse DISP/S30 contenant une liste de msgId.
 */
typedef struct {
    char** msgIdList;       ///< Liste des identifiants de messages disponibles
    size_t msgIdCount;      ///< Nombre d'identifiants
} DispS30StatusResponse;

/**
 * @brief Libère la mémoire allouée dynamiquement dans une réponse DISP/S30
 * @param res Pointeur vers la structure à libérer
 */
//void free_disp_s30_status_response(DispS30StatusResponse* res);
extern void free_disp_s30_status_response(DispS30StatusResponse* res);

#endif // DISP_S30_H
