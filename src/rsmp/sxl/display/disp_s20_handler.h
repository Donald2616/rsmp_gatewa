#ifndef DISP_S20_HANDLER_H
#define DISP_S20_HANDLER_H

#include "rsmp/fields/arg.h"
#include "rsmp/fields/rvs.h"
#include <stdio.h>

/**
 * @brief Handler pour le message DISP/S20.
 * 
 * Ce handler traite la réception d'un message DISP/S20, décode le contenu 
 * JSON, vérifie les informations et prépare une réponse standardisée.
 * 
 * @param context Pointeur vers le contexte utilisateur.
 * @param rsmp_context Contexte RSMP.
 * @param request Arguments de la requête (message reçu).
 * @param response Structure de la réponse à remplir.
 * @return int Code de retour RSMP (0 si succès).
 */
int handle_disp_s20(void *context, void *rsmp_context, args const *request, rvs *response);

#endif // DISP_S20_HANDLER_H
