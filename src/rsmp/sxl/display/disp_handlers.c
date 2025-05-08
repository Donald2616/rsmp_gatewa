/**
 * @file disp_handlers.c
 * @brief Gestion centralisée des handlers RSMP pour les messages DISP/.
 * 
 * Ce fichier contient les fonctions nécessaires pour enregistrer 
 * les handlers des commandes et des statuts liés aux messages DISP/.
 * 
 * @details Les handlers sont enregistrés au démarrage pour lier chaque 
 * message DISP/Mxx ou DISP/Sxx à la fonction de gestion appropriée.
 * 
 * @version 1.0
 * @date 2025-03-29
 * 
 * @note Ce fichier est essentiel pour la gestion centralisée des handlers RSMP.
 */

 #include "rsmp/sxl/display/disp_display.h"
 //#include "rsmp/sxl/display/disp_commands.h"  ///< Handlers pour les CommandRequest
 //#include "rsmp/sxl/display/status/disp_status.h"  ///< Handlers pour les CommandRequest
 #include "rsmp/sxl/sxl.h"     ///< API RSMP pour l'enregistrement des handlers
 
 /**
  * @brief Enregistre les handlers pour les commandes DISP/Mxx.
  * 
  * Cette fonction enregistre chaque handler de commande en utilisant 
  * la fonction RSMP `rsmp_sxl_cmd_jsut_add`.
  * 
  * @note Appelée lors de l'initialisation du RSMP-Gateway.
  */
 void register_disp_command_handlers(void) {
     rsmp_sxl_cmd_jsut_add("DISP/M31", handle_disp_m31, NULL);  ///< Enregistrement du handler pour DISP/M31
     rsmp_sxl_cmd_jsut_add("DISP/M32", handle_disp_m32, NULL);  ///< Enregistrement du handler pour DISP/M32
     rsmp_sxl_cmd_jsut_add("DISP/M40", handle_disp_m40, NULL);  ///< Enregistrement du handler pour DISP/M40
     rsmp_sxl_cmd_jsut_add("DISP/M41", handle_disp_m41, NULL);  ///< Enregistrement du handler pour DISP/M41
 }
 
 /**
  * @brief Enregistre les handlers pour les statuts DISP/Sxx.
  * 
  * Cette fonction enregistre chaque handler de statut en utilisant 
  * la fonction RSMP `rsmp_sxl_cmd_jsut_add`.
  * 
  * @note Appelée lors de l'initialisation du RSMP-Gateway.
  */
 void register_disp_status_handlers(void) {
     rsmp_sxl_cmd_jsut_add("DISP/S10", handle_disp_s10, NULL);  ///< Enregistrement du handler pour DISP/S10
     rsmp_sxl_cmd_jsut_add("DISP/S20", handle_disp_s20, NULL);  ///< Enregistrement du handler pour DISP/S20
     rsmp_sxl_cmd_jsut_add("DISP/S30", handle_disp_s30, NULL);  ///< Enregistrement du handler pour DISP/S30
//     rsmp_sxl_cmd_jsut_add("DISP/S31", handle_disp_s31, NULL);  ///< Enregistrement du handler pour DISP/S31
     rsmp_sxl_cmd_jsut_add("DISP/S40", handle_disp_s40, NULL);  ///< Enregistrement du handler pour DISP/S40
 }
 
 /**
  * @brief Enregistre tous les handlers DISP.
  * 
  * Fonction globale pour centraliser l'enregistrement de tous les handlers
  * DISP (commandes et statuts). Appelée lors de l'initialisation.
  */
 void register_disp_handlers(void) {
     register_disp_command_handlers();  ///< Enregistrer les commandes DISP
     register_disp_status_handlers();   ///< Enregistrer les statuts DISP
 }
 