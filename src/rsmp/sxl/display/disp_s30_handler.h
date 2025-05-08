#ifndef DISP_S30_HANDLER_H
#define DISP_S30_HANDLER_H

#include "rsmp/fields/arg.h"
#include "rsmp/fields/rvs.h"

/**
 * @brief Handler pour le message DISP/S30.
 * 
 * @param context Pointeur vers le contexte utilisateur.
 * @param rsmp_context Contexte RSMP.
 * @param request Arguments de la requête (message reçu).
 * @param response Structure de la réponse à remplir.
 * @return int Code de retour RSMP (0 si succès).
 */
int handle_disp_s30(void *context, void *rsmp_context, args const *request, rvs *response);

#endif // DISP_S30_HANDLER_H
