/**
 * @file disp_s30_handler.c
 * @brief Handler pour le message DISP/S30 (StatusResponse).
 * 
 * Ce fichier contient l'implémentation du handler pour gérer les réponses 
 * associées au message DISP/S30 dans le RSMP-Gateway.
 * 
 * @version 1.0
 * @date 2025-04-02
 */

 #include "rsmp/sxl/display/disp_s30.h"  ///< Header spécifique pour DISP/S30
 #include "rsmp/fields/arg.h"            ///< Gestion des arguments
 #include "rsmp/fields/rvs.h"            ///< Gestion des réponses
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "errcode/errcode.h"
 #include "rsmp/sxl/display/disp_json_generic.h"
 
 /**
  * @brief Libère toute la mémoire allouée dynamiquement dans un DispS30StatusResponse
  */
  /*
 void free_disp_s30_status_response(DispS30StatusResponse* res) {
     if (res == NULL) return;
 
     for (size_t i = 0; i < res->msgIdCount; ++i) {
         if (res->msgIdList[i]) {
             free(res->msgIdList[i]);
         }
     }
 
     free(res->msgIdList);
     res->msgIdList = NULL;
     res->msgIdCount = 0;
 }
 */
 /**
  * @brief Handler pour le message DISP/S30.
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
 int handle_disp_s30(void *context, void *rsmp_context, args const *request, rvs *response) {
     (void)context;
     (void)rsmp_context;
 
     // Vérification des paramètres d'entrée
     if (request == NULL || request->v.len == 0) {
         fprintf(stderr, "[DISP/S30] Erreur : Requête invalide ou vide.\n");
         return _rsmp_errcod_gen_no_error_;
     }
 
     // Décodage du message JSON reçu
     DispS30StatusResponse parsed;
     const char *json_str = (const char*)request->v.items;
 
     int decode_status = decode_disp_s30_status_response(json_str, &parsed);
     if (decode_status != 0) {
         fprintf(stderr, "[DISP/S30] Erreur de décodage du message : %s\n", json_str);
         return _rsmp_errcod_gen_no_error_;
     }
 
     // Vérification des msgId
     if (parsed.msgIdCount == 0 || parsed.msgIdList == NULL) {
         fprintf(stderr, "[DISP/S30] Liste de msgId vide ou invalide.\n");
         free_disp_s30_status_response(&parsed);
         return _rsmp_errcod_gen_no_error_;
     }
 
     // Log des messages reçus
     printf("[DISP/S30] Nombre d'identifiants reçus : %zu\n", parsed.msgIdCount);
     for (size_t i = 0; i < parsed.msgIdCount; ++i) {
         printf("[DISP/S30] msgId[%zu] : %s\n", i, parsed.msgIdList[i]);
     }
 
    // Préparation de la réponse
    rsmp_array_char_new(&response->v); // Initialiser la structure v
    //response->v = rsmp_array_char_new(1); // Initialiser avec une taille de 1

    if (rsmp_array_char_from_str_len(&response->v, "ACK", strlen("ACK")) == NULL) {
        fprintf(stderr, "[DISP/S30] Erreur d'initialisation du tableau de caractères.\n");
        free_disp_s30_status_response(&parsed);
        return RSMP_ERRCODE_ERRCODE_H;
    }

 
     response->age = _rsmp_age_no_;
 
     // Libération des ressources
     free_disp_s30_status_response(&parsed);
     return _rsmp_no_error_;
 }
 