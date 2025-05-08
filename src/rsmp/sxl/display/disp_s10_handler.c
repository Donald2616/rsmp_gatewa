#include "disp_s10.h"
#include "rsmp/fields/arg.h"
#include "rsmp/fields/rvs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errcode/errcode.h"
#include "rsmp/sxl/display/disp_json_generic.h"

/**
 * @brief Handler pour le message S10 (StatusRequest).
 * 
 * @param context Pointeur vers le contexte utilisateur.
 * @param rsmp_context Contexte RSMP.
 * @param request Arguments de la requête (message reçu).
 * @param response Structure de la réponse à remplir.
 * @return int Code de retour RSMP (0 si succès).
 */
int handle_disp_s10(void *context, void *rsmp_context, args const *request, rvs *response) {
    (void)context;
    (void)rsmp_context;
    (void)response;

    // Vérification des paramètres d'entrée
    if (request == NULL || request->v.len == 0) {
        fprintf(stderr, "[DISP/S10] Erreur : Requête invalide ou vide.\n");
        return _rsmp_errcod_gen_generic_;
    }

    // Décodage du message JSON reçu
    DispS10StatusResponse parsed;
    const char *json_str = (const char*)request->v.items;
    int decode_status = decode_disp_s10_status_response(json_str, &parsed);
    if (decode_status != 0) {
        fprintf(stderr, "[DISP/S10] Erreur de décodage du message : %s\n", json_str);
        return _rsmp_errcod_gen_generic_;
    }

// Vérification du champ displayType
if (strlen(parsed.displayType) == 0) {
    fprintf(stderr, "[DISP/S10] displayType vide ou invalide.\n");
    return _rsmp_errcod_gen_generic_;
}

// Log des informations reçues
printf("[DISP/S10] Type d'affichage : %s\n", parsed.displayType);
printf("[DISP/S10] Type de template : %s\n", parsed.templateType);
printf("[DISP/S10] Temps d'affichage minimum : %u\n", parsed.minDisplayTime);
printf("[DISP/S10] Taille max de l'ID d'objet : %u\n", parsed.maxObjectIdSize);
printf("[DISP/S10] Nombre max de templates supportés : %u\n", parsed.maxSupportedTemplate);
printf("[DISP/S10] Nombre max de zones supportées : %u\n", parsed.maxSupportedZone);
printf("[DISP/S10] Nombre max de séquences supportées : %u\n", parsed.maxSupportedSequence);
printf("[DISP/S10] Nombre max d'animations supportées : %u\n", parsed.maxSupportedAnimation);
printf("[DISP/S10] Nombre max de messages disponibles : %u\n", parsed.maxSupportedAvailableMsg);
printf("[DISP/S10] Taille max de la pool d'affichage : %u\n", parsed.maxSupportedDisplayPool);
printf("[DISP/S10] Vitesse max de défilement : %u\n", parsed.maxScrollSpeed);
printf("[DISP/S10] Taille de l'objet Web : Largeur=%u, Hauteur=%u\n", parsed.size.width, parsed.size.height);
printf("[DISP/S10] Texte affichable : %s\n", parsed.textDisplayable ? "Oui" : "Non");
printf("[DISP/S10] Nombre de groupes prédéfinis : %u\n", parsed.predefinedGroupCount);

// Boucle pour afficher les groupes prédéfinis s'ils existent
if (parsed.predefinedGroupCount > 0 && parsed.predefinedGroups) {
    for (unsigned int i = 0; i < parsed.predefinedGroupCount; ++i) {
      //  printf("[DISP/S10] Groupe prédéfini %u : %s\n", i, parsed.predefinedGroups[i].groupName);
    }
}



    // Préparation de la réponse
    response->age = _rsmp_age_no_;

    return _rsmp_no_error_;
    return 0;

}
