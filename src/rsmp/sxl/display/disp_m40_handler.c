/**
 * @file disp_m40_handler.c
 * @brief Handler pour le message DISP/M40 (CommandRequest).
 * 
 * Ce fichier contient l'implémentation du handler pour gérer les commandes 
 * associées au message DISP/M40 dans le RSMP-Gateway.
 * 
 * @details Le handler traite la réception d'un message de demande d'affichage, 
 * décode les données reçues et prépare une réponse appropriée.
 * 
 * @version 1.0
 * @date 2025-04-02
 * 
 * @note Ce fichier contient également la fonction utilitaire pour libérer la mémoire.
 */

 #include "rsmp/sxl/display/disp_m40.h"  ///< Header spécifique pour DISP/M40
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
// void free_disp_m40_command_request(DispM40CommandRequest* req) {
  //   if (!req) return;
  //   memset(req->msgId, 0, sizeof(req->msgId));
// }
 
 /**
  * @brief Handler pour le message DISP/M40.
  * 
  * Ce handler traite la réception d'un message DISP/M40, décode le contenu 
  * JSON, vérifie les informations et prépare une réponse standardisée.
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
 int handle_disp_m40(void *context, void *rsmp_context, args const *request, rvs *response) {
     (void)context;
     (void)rsmp_context;
 
     // Vérification des paramètres d'entrée
     if (request == NULL || request->v.len == 0) {
         fprintf(stderr, "[DISP/M40] Erreur : Requête invalide ou vide.\n");
         return _rsmp_errcod_gen_generic_;
     }
 
     // Décodage du message JSON reçu
     DispM40CommandResponse parsed;
     const char *json_str = (const char*)request->v.items;
     int decode_status = decode_disp_m40_command_response(json_str, &parsed);
     if (decode_status != 0) {
         fprintf(stderr, "[DISP/M40] Erreur de décodage du message : %s\n", json_str);
         return _rsmp_errcod_gen_generic_;
     }
 

     // Vérification du champ msgId
     if (strlen(parsed.msgId) == 0) {
         fprintf(stderr, "[DISP/M40] msgId vide ou invalide.\n");
         return _rsmp_errcod_gen_generic_;
     }
 
     // Log du message reçu
     printf("[DISP/M40] Message reçu : %s\n", parsed.msgId);
  //   printf("[DISP/M31] Statut du message : %s\n", parsed.retMessages[0].retStatus.rea);
     printf("[DISP/M40] Statut du message : %s\n", parsed.retStatus.rea);
 
     // Préparation de la réponse
     /*
     response->v = rsmp_array_char_new(1);
     if (!response->v.items) {
         fprintf(stderr, "[DISP/M40] Erreur d'allocation mémoire pour la réponse.\n");
         return _rsmp_errcod_gen_no_error_;
     }
         */
 
   //  rsmp_array_char_set(&response->v, 0, "ACK");

        // Préparation de la réponse
    rsmp_array_char_t *array = rsmp_array_char_from_str_len(&response->v, "ACK", strlen("ACK"));
    if (array == NULL) {
        fprintf(stderr, "[DISP/M40] Erreur d'initialisation du tableau de caractères.\n");
        return _rsmp_errcod_gen_no_error_;
    }

     
 
     // Vérification que la réponse est bien initialisée
     if (response->v.len == 0) {
         fprintf(stderr, "[DISP/M40] Réponse vide.\n");
         return _rsmp_errcod_gen_generic_;
     }
 
     // Affecter l'âge de la réponse
     response->age = _rsmp_age_no_;
 

    // Libération des ressources
    free_disp_m40_command_request((DispM40CommandRequest *)&parsed);
    return _rsmp_no_error_;
    return 0;
}
 

     
 
 
 