/**
 * @file disp_display.h
 * @brief Header central pour les handlers des messages DISP/.
 * 
 * Ce fichier contient les déclarations de toutes les fonctions handlers 
 * associées aux messages DISP/Mxx et DISP/Sxx dans le projet RSMP-Gateway.
 * 
 * @details Les fonctions déclarées ici permettent de gérer les commandes 
 * et les statuts du module DISP/ en assurant l'enregistrement centralisé.
 * 
 * @version 1.0
 * @date 2025-03-29
 * 
 * @note Ce fichier est essentiel pour centraliser les handlers DISP/.
 */

 #ifndef DISP_DISPLAY_H
 #define DISP_DISPLAY_H
 
 #include "rsmp/fields/arg.h"    ///< Inclusion des définitions des arguments
 #include "rsmp/fields/rvs.h"    ///< Inclusion des définitions des réponses
 
 /**
  * @brief Fonction pour enregistrer tous les handlers DISP/.
  * 
  * Appelée lors de l'initialisation pour lier chaque message DISP/ 
  * au handler approprié.
  */
 void register_disp_handlers(void);

 void register_disp_status_handlers(void);

 void register_disp_command_handlers(void);

 
 /** 
  * @brief Handler pour le message DISP/M31 (CommandRequest).
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
 int handle_disp_m31(void *context, void *rsmp_context, args const *request, rvs *response);
 
 /** 
  * @brief Handler pour le message DISP/M32 (CommandRequest).
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
 int handle_disp_m32(void *context, void *rsmp_context, args const *request, rvs *response);
 
 /** 
  * @brief Handler pour le message DISP/M40 (CommandRequest).
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
 int handle_disp_m40(void *context, void *rsmp_context, args const *request, rvs *response);
 
 /** 
  * @brief Handler pour le message DISP/M41 (CommandRequest).
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
 int handle_disp_m41(void *context, void *rsmp_context, args const *request, rvs *response);
 
 /** 
  * @brief Handler pour le message DISP/S10 (StatusRequest).
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
 int handle_disp_s10(void *context, void *rsmp_context, args const *request, rvs *response);
 
 /** 
  * @brief Handler pour le message DISP/S20 (StatusRequest).
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
 int handle_disp_s20(void *context, void *rsmp_context, args const *request, rvs *response);
 
 /** 
  * @brief Handler pour le message DISP/S30 (StatusRequest).
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
 int handle_disp_s30(void *context, void *rsmp_context, args const *request, rvs *response);


  /** 
  * @brief Handler pour le message DISP/S31 (StatusRequest).
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
  int handle_disp_s31(void *context, void *rsmp_context, args const *request, rvs *response);
 
 /** 
  * @brief Handler pour le message DISP/S40 (StatusRequest).
  * 
  * @param context Pointeur vers le contexte utilisateur.
  * @param rsmp_context Contexte RSMP.
  * @param request Arguments de la requête (message reçu).
  * @param response Structure de la réponse à remplir.
  * @return int Code de retour RSMP (0 si succès).
  */
 int handle_disp_s40(void *context, void *rsmp_context, args const *request, rvs *response);
 
 #endif // DISP_DISPLAY_H
 