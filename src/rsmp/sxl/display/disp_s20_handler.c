#include "rsmp/sxl/display/disp_s20_handler.h"
#include "rsmp/fields/arg.h"
#include "rsmp/fields/rvs.h"
#include "errcode/errcode.h"
#include "rsmp/sxl/display/disp_json_generic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Handler pour le message DISP/S20.
 */
int handle_disp_s20(void *context, void *rsmp_context, args const *request, rvs *response) {
    (void)context;
    (void)rsmp_context;
    (void)response;

    // Vérification des paramètres d'entrée
    if (request == NULL || request->v.len == 0) { 
        fprintf(stderr, "[DISP/S20] Erreur : Requête invalide ou vide.\n");
        return _rsmp_errcod_gen_generic_;
    }

    // Décodage du message JSON reçu
    DispS20StatusResponse parsed;
    const char *json_str = (const char*)request->v.items;
    int decode_status = decode_disp_s20_status_response(json_str, &parsed);

    if (decode_status != 0) {
        fprintf(stderr, "[DISP/S20] Erreur de décodage du message : %s\n", json_str);
        return _rsmp_errcod_gen_generic_;
    }

    // Vérification du champ templateCount
    if (parsed.templateCount == 0) {
        fprintf(stderr, "[DISP/S20] Aucune template reçue.\n");
        return _rsmp_errcod_gen_generic_;
    }

    // Log des templates reçus
    for (int i = 0; i < parsed.templateCount; i++) {
        printf("[DISP/S20] Template %d : %s\n", i, parsed.templates[i].templateId);
    }

    // Préparation de la réponse
    response->age = _rsmp_age_no_;
    return _rsmp_no_error_;
    return 0;
}
