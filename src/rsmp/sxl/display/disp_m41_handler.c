/**
 * @file disp_m41_handler.c
 * @brief Handler pour le message DISP/M41 (CommandRequest).
 * 
 * Ce fichier contient l'implémentation du handler pour gérer les commandes 
 * associées au message DISP/M41 dans le RSMP-Gateway.
 * 
 * @details Le handler traite la réception d'un message de demande d'affichage, 
 * décode les données reçues et prépare une réponse appropriée.
 * 
 * @version 1.0
 * @date 2025-04-02
 * 
 */

#include "rsmp/sxl/display/disp_m41.h"  ///< Header spécifique pour DISP/M41
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
  void free_disp_m41_command_request(DispM41CommandRequest* req) {
    if (!req) return;
    memset(req->msgId, 0, sizeof(req->msgId));
}

/**
 * @brief Handler pour le message DISP/M41.
 */
 int handle_disp_m41(void *context, void *rsmp_context, args const *request, rvs *response) {
    (void)context;
    (void)rsmp_context;

    // Vérification des paramètres d'entrée
    if (request == NULL || request->v.len == 0) { 
        fprintf(stderr, "[DISP/M41] Erreur : Requête invalide ou vide.\n");
        return _rsmp_errcod_gen_generic_;
    }

    // Décodage du message JSON reçu
    DispM41CommandResponse parsed;
    const char *json_str = (const char*)request->v.items;

    int decode_status = decode_disp_m41_command_response(json_str, &parsed);
    if (decode_status != 0) {
        fprintf(stderr, "[DISP/M41] Erreur de décodage du message : %s\n", json_str);
        return _rsmp_errcod_gen_generic_;
    }


// Vérification du champ msgId
if (strlen(parsed.msgId) == 0) {
    fprintf(stderr, "[DISP/M41] msgId vide ou invalide.\n");
    return _rsmp_errcod_gen_generic_;
}

// Log du message reçu
printf("[DISP/M41] Message reçu : %s\n", parsed.msgId);
printf("[DISP/M41] Statut du message : %s\n", parsed.retStatus.rea);

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
    fprintf(stderr, "[DISP/M41] Réponse vide.\n");
    return  _rsmp_errcod_gen_no_error_;
}

// Affecter l'âge de la réponse
response->age = _rsmp_age_no_;

// Libération des ressources
free_disp_m41_command_request((DispM41CommandRequest *)&parsed);
return _rsmp_no_error_;


return 0;
}

