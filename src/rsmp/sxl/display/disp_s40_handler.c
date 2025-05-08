#include "rsmp/sxl/display/disp_s40_handler.h"
#include "rsmp/fields/arg.h"
#include "rsmp/fields/rvs.h"
#include "errcode/errcode.h"
#include "rsmp/sxl/display/disp_json_generic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Handler pour le message DISP/S40.
 */
int handle_disp_s40(void *context, void *rsmp_context, args const *request, rvs *response) {
    (void)context;
    (void)rsmp_context;
    (void)response;

    // Vérification des paramètres d'entrée
    if (request == NULL || request->v.len == 0) { 
        fprintf(stderr, "[DISP/S40] Erreur : Requête invalide ou vide.\n");
        return _rsmp_errcod_gen_generic_;
    }

    // Décodage du message JSON reçu
    DispS40StatusResponse parsed;
    const char *json_str = (const char*)request->v.items;
    int decode_status = decode_disp_s40_status_response(json_str, &parsed);

    if (decode_status != 0) {
        fprintf(stderr, "[DISP/S40] Erreur de décodage du message : %s\n", json_str);
        return _rsmp_errcod_gen_generic_;
    }

    // Vérification du champ displayPoolCount
    if (parsed.displayPoolCount == 0) {
        fprintf(stderr, "[DISP/S40] Aucune donnée de display pool reçue.\n");
        return _rsmp_errcod_gen_generic_;
    }

    // Log des éléments du display pool
    for (unsigned int i = 0; i < parsed.displayPoolCount; i++) {
        printf("[DISP/S40] Display Pool Element %u : %s\n", i, parsed.displayPoolList[i].msgId);

    }

    // Préparation de la réponse
    response->age = _rsmp_age_no_;
    return _rsmp_no_error_;
    return 0;
}
