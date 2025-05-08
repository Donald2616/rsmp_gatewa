/**
 * @file disp_m31_handler.c
 * @brief Handler pour le message DISP/M31 (CommandRequest).
 * 
 * Ce fichier contient l'implémentation du handler pour gérer les commandes 
 * associées au message DISP/M31 dans le RSMP-Gateway.
 * 
 * @details Le handler traite la réception d'un message de demande d'affichage, 
 * décode les données reçues et prépare une réponse appropriée.
 * 
 * @version 1.0
 * @date 2025-04-02
 * 
 * @note Ce fichier contient également la fonction utilitaire pour libérer la mémoire.
 */

 #include "rsmp/sxl/display/disp_m31.h"  ///< Header spécifique pour DISP/M31
 #include "rsmp/sxl/display/disp_m31_handler.h"  ///< Header spécifique pour DISP/M31
 #include "rsmp/fields/arg.h"                    ///< Gestion des arguments
 #include "rsmp/fields/rvs.h"                    ///< Gestion des réponses
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include "errcode/errcode.h"
 #include "rsmp/sxl/display/disp_json_generic.h"


/**
 * @brief Libère toute la mémoire allouée dynamiquement dans un DispM31CommandRequest
 */

 /*
 void free_disp_m31_command_request(DispM31CommandRequest* req) {
    if (!req || !req->messages) return;

    for (int i = 0; i < req->messageCount; ++i) {
        MessageObject* msg = &req->messages[i];

        for (int j = 0; j < msg->sequenceCount; ++j) {
            Sequence* seq = &msg->sequences[j];

            for (int k = 0; k < seq->animationCount; ++k) {
                Animation* anim = &seq->animations[k];
                if (anim->contents)
                    free(anim->contents);
            }

            if (seq->animations)
                free(seq->animations);
        }

        if (msg->sequences)
            free(msg->sequences);
    }

    free(req->messages);
    req->messages = NULL;
    req->messageCount = 0;
}
*/

/**
  * @brief Handler pour le message DISP/M31.
  * 
  * Ce handler traite la réception d'un message DISP/M31, décode le contenu 
  * JSON, vérifie les informations et prépare une réponse standardisée.
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
  int handle_disp_m31(void *context, void *rsmp_context, args const *request, rvs *response) {
    (void)context;
    (void)rsmp_context;
    (void)response;
    // Vérification des paramètres d'entrée
    if (request == NULL || request->v.len == 0) { 

        fprintf(stderr, "[DISP/M31] Erreur : Requête invalide ou vide.\n");
        return _rsmp_errcod_gen_generic_;
    }

// Décodage du message JSON reçu
DispM31CommandResponse parsed;
const char *json_str = (const char*)request->v.items;  // Déclaration et initialisation de json_str

int decode_status = decode_disp_m31_command_response(json_str, &parsed);
if (decode_status != 0) {
    fprintf(stderr, "[DISP/M31] Erreur de décodage du message : %s\n", json_str);
    return _rsmp_errcod_gen_generic_;
}


// Vérification du champ msgId
if (strlen(parsed.retMessages[0].msgId) == 0) {
    fprintf(stderr, "[DISP/M31] msgId vide ou invalide.\n");
    return _rsmp_errcod_gen_generic_;
}


    // Log du message reçu
    printf("[DISP/M31] Message reçu : %s\n", (parsed.retMessages[0].msgId));
    printf("[DISP/M31] Statut du message : %s\n", parsed.retMessages[0].retStatus.rea);

// Préparation de la réponse
/*
if (rsmp_array_char_insert(&response->v, 1) != 0) {
    fprintf(stderr, "[DISP/M31] Erreur d'initialisation du tableau de caractères.\n");
    return  _rsmp_errcod_gen_no_error_;
}
*/
// Préparation de la réponse
rsmp_array_char_t *array = rsmp_array_char_from_str_len(&response->v, "ACK", strlen("ACK"));
if (array == NULL) {
    fprintf(stderr, "[DISP/M31] Erreur d'initialisation du tableau de caractères.\n");
    return _rsmp_errcod_gen_no_error_;
}



// Affecter la valeur "ACK" dans le tableau de caractères
if (rsmp_array_char_set(&response->v, 0, "ACK") != 0) {
    fprintf(stderr, "[DISP/M31] Erreur lors de l'affectation de la valeur.\n");
    return _rsmp_errcod_gen_generic_;
}

// Vérification que la réponse est bien initialisée
if (response->v.len == 0) {
    fprintf(stderr, "[DISP/M31] Réponse vide.\n");
    return  _rsmp_errcod_gen_no_error_;
}

// Affecter l'âge de la réponse
response->age = _rsmp_age_no_;

// Libération des ressources
free_disp_m31_command_request((DispM31CommandRequest *)&parsed);
//free_disp_m31_command_request(&parsed);

return _rsmp_no_error_;
    return 0;
}
 