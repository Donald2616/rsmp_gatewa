/**
 * @file disp_m32_handler.c
 * @brief Handler pour le message DISP/M32 (CommandRequest).
 *
 * Ce fichier contient l'implémentation du handler pour gérer les commandes 
 * associées au message DISP/M32 dans le RSMP-Gateway.
 *
 * @details Le handler traite la réception d'un message de demande d'affichage, 
 * décode les données reçues et prépare une réponse appropriée.
 *
 * @version 1.0
 * @date 2025-04-02
 */

 #include "rsmp/sxl/display/disp_m32.h"  ///< Header spécifique pour DISP/M32
 #include "rsmp/fields/arg.h"            ///< Gestion des arguments
 #include "rsmp/fields/rvs.h"            ///< Gestion des réponses
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include "errcode/errcode.h"
 #include "rsmp/sxl/display/disp_json_generic.h"
 
 /**
  * @brief Libère toute la mémoire allouée dynamiquement dans un DispM40CommandRequest
  */
/*
  void free_disp_m32_command_request(DispM32CommandRequest* req) {
    if (!req || !req->msgIdList) return;

    for (int i = 0; i < req->msgIdCount; ++i) {
        if (req->msgIdList[i]) {
            free(req->msgIdList[i]);
        }
    }
    free(req->msgIdList);
    req->msgIdList = NULL;
    req->msgIdCount = 0;
}
    */
 
 /**
  * @brief Handler pour le message DISP/M32.
  * 
  * Ce handler traite la réception d'un message DISP/M32, décode le contenu 
  * JSON, vérifie les informations et prépare une réponse standardisée.
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
 int handle_disp_m32(void *context, void *rsmp_context, args const *request, rvs *response) {
     (void)context;
     (void)rsmp_context;
     (void)response;
 
     if (request == NULL || request->v.len == 0) {
         fprintf(stderr, "[DISP/M32] Erreur : Requête invalide ou vide.\n");
         return _rsmp_errcod_gen_generic_;
     }
 
     // Décodage du message JSON reçu
     DispM32CommandRequest parsed;
     const char *json_str = (const char*)request->v.items;
 
     int decode_status = decode_disp_m32_command_response(json_str, (DispM32CommandResponse*)&parsed);
     if (decode_status != 0) {
         fprintf(stderr, "[DISP/M32] Erreur de décodage du message : %s\n", json_str);
         return _rsmp_errcod_gen_generic_;
     }
 
// Vérification du champ msgIdList
if (parsed.msgIdCount == 0 || parsed.msgIdList == NULL) {
    fprintf(stderr, "[DISP/M32] Liste d'identifiants vide ou invalide.\n");
    return _rsmp_errcod_gen_generic_;
}

// Affichage des identifiants
for (int i = 0; i < parsed.msgIdCount; i++) {
    if (parsed.msgIdList[i] == NULL || strlen(parsed.msgIdList[i]) == 0) {
        fprintf(stderr, "[DISP/M32] msgId[%d] vide ou invalide.\n", i);
        return _rsmp_errcod_gen_generic_;
    }
    printf("[DISP/M32] Texte à afficher : %s\n", parsed.msgIdList[i]);
}

 
     // Préparation de la réponse
     rsmp_array_char_t *array = rsmp_array_char_from_str_len(&response->v, "ACK", strlen("ACK"));
     if (array == NULL) {
         fprintf(stderr, "[DISP/M32] Erreur d'initialisation du tableau de caractères.\n");
         return _rsmp_errcod_gen_no_error_;
     }
 
     if (rsmp_array_char_set(&response->v, 0, "ACK") != 0) {
         fprintf(stderr, "[DISP/M32] Erreur lors de l'affectation de la valeur.\n");
         return _rsmp_errcod_gen_no_error_;
     }
 
     response->age = _rsmp_age_no_;
 
     // Libération des ressources
     free_disp_m32_command_request(&parsed);
     return _rsmp_no_error_;
 }
 