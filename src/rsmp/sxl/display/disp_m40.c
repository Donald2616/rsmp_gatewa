/**
 * @file disp_m40.c
 * @brief Implémentation des fonctions du signal DISP/M40 - Activation d’un message
 */

 #include <stdlib.h>
 #include <string.h>
 #include "disp_m40.h"
 
 /**
  * @brief Libère les champs dynamiques de la requête M40
  * (Actuellement vide, mais utile si future utilisation de malloc)
  */
 void free_disp_m40_command_request(DispM40CommandRequest* req) {
     // Rien à libérer pour le moment
     (void)req;
 }
 
 /**
  * @brief Libère les champs dynamiques de la réponse M40
  */
 void free_disp_m40_command_response(DispM40CommandResponse* res) {
     // Rien à libérer ici non plus
     (void)res;
 }
 