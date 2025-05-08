#ifndef DISP_JSON_GENERIC_H
#define DISP_JSON_GENERIC_H

#include <json-c/json.h>
#include "disp_m31.h"
#include "disp_m32.h"
#include "disp_m40.h"
#include "disp_m41.h"
#include "disp_s20.h"
#include "disp_s10.h"
#include "disp_s40.h"
#include "disp_s30.h"
#include "rsmp/sxl/display/disp_m31_handler.h"
#include "rsmp/sxl/display/disp_m31.h"


/**
 * @brief Encode une réponse DISP/S30 (StatusResponse) en JSON
 * @param res Pointeur vers la structure DISP/S30 à encoder
 * @return Objet JSON à libérer avec json_object_put
 */
 struct json_object* encode_disp_s30_status_response(const DispS30StatusResponse* res);

 /**
  * @brief Décode un JSON DISP/S30 en structure C
  * @param json_str Chaîne JSON à décoder
  * @param res Structure à remplir
  * @return 0 si succès, -1 sinon
  */
 int decode_disp_s30_status_response(const char* json_str, DispS30StatusResponse* res);

/**
 * @file disp_json_generic.h
 * @brief Interface des encodeurs/décodeurs JSON pour les signaux DISP (RSMP)
 *
 * Ce header déclare les fonctions utilisées pour sérialiser et désérialiser
 * les structures SXL DISPLAY (ex: DISP/M31, DISP/M32) via la bibliothèque json-c.
 */
/**
 * @brief Encode une réponse DISP/S40 (StatusResponse) en JSON.
 *
 * @param res Pointeur vers la structure DISP/S40 à encoder
 * @return Objet JSON représentant la réponse (à libérer avec json_object_put), ou NULL en cas d'erreur
 */
 struct json_object* encode_disp_s40_status_response(const DispS40StatusResponse* res);
// === DISP/S31 ===

/**
 * @brief Décode une réponse JSON DISP/S40 (StatusResponse)
 * @param json_str Chaîne JSON source
 * @param res Pointeur vers la structure à remplir
 * @return 0 si succès, -1 sinon
 */
 int decode_disp_s40_status_response(const char* json_str, DispS40StatusResponse* res);

 /**
  * @brief Libère la mémoire allouée dynamiquement dans un StatusResponse S40
  * @param res Pointeur vers la structure à libérer
  */
 void free_disp_s40_status_response(DispS40StatusResponse* res);
 

/**
 * @brief Encode une réponse DISP/S31 (StatusResponse) en JSON.
 *
 * @param res Pointeur vers la structure DISP/S31 à encoder
 * @return Objet JSON représentant la réponse (à libérer avec json_object_put), ou NULL en cas d'erreur
 */
 //struct json_object* encode_disp_s31_status_response(const DispS31StatusResponse* res);

// int decode_disp_s31_status_response(const char* json_str, DispS31StatusResponse* res);

 // === DISP/M40 ===
 struct json_object* encode_disp_m40_command_request(const DispM40CommandRequest* req);
 int decode_disp_m40_command_response(const char* json_str, DispM40CommandResponse* res);

 // === DISP/M41 ===

 struct json_object* encode_disp_m41_command_request(const DispM41CommandRequest* req);
int decode_disp_m41_command_response(const char* json_str, DispM41CommandResponse* res);

 // === DISP/S10 ===
 struct json_object* encode_disp_s10_status_response(const DispS10StatusResponse* res);
 int decode_disp_s10_status_response(const char* json_str, DispS10StatusResponse* res);

 // === DISP/S20 ===
struct json_object* encode_disp_s20_status_response(const DispS20StatusResponse* res);
int decode_disp_s20_status_response(const char* json_str, DispS20StatusResponse* res);


// === DISP/M31 ===

/**
 * @brief Encode une requête DISP/M31 en JSON
 * @param request Pointeur vers la structure de requête
 * @return Objet JSON généré (à libérer après usage)
 */
struct json_object* encode_disp_m31_command_request(const DispM31CommandRequest* request);

/**
 * @brief Décode une réponse JSON DISP/M31
 * @param json_str Chaîne JSON
 * @param response Structure cible à remplir
 * @return 0 si succès, -1 si erreur
 */
 int decode_disp_m31_command_response(const char* json_str, DispM31CommandResponse* response);


// === DISP/M32 ===

/**
 * @brief Encode une requête DISP/M32 en JSON
 * @param req Pointeur vers la requête DISP/M32
 * @return Objet JSON généré
 */
struct json_object* encode_disp_m32_command_request(const DispM32CommandRequest* req);

/**
 * @brief Décode une réponse JSON DISP/M32
 * @param json_str Chaîne JSON
 * @param res Pointeur vers la réponse à remplir
 * @return 0 si succès, -1 si erreur
 */
int decode_disp_m32_command_response(const char* json_str, DispM32CommandResponse* res);

// === DISP/M40 ===

/**
 * @brief Encode une commande DISP/M40 (activation d'un message) au format JSON
 *
 * Cette fonction convertit une structure C `DispM40CommandRequest` en objet JSON conforme au protocole RSMP.
 *
 * @param req Pointeur vers la structure de la commande à encoder
 * @return struct json_object* Objet JSON (à libérer avec json_object_put), ou NULL en cas d'erreur
 */
 struct json_object* encode_disp_m40_command_request(const DispM40CommandRequest* req);

 /**
  * @brief Décode une réponse JSON DISP/M40 en structure C
  *
  * Cette fonction extrait les données d'une réponse JSON au message DISP/M40 (`CommandResponse`)
  * et remplit une structure `DispM40CommandResponse` correspondante.
  *
  * @param json_str Chaîne JSON à parser
  * @param res Pointeur vers la structure de réponse à remplir
  * @return int 0 si succès, -1 en cas d'erreur
  */
 int decode_disp_m40_command_response(const char* json_str, DispM40CommandResponse* res);
 

#endif // DISP_JSON_GENERIC_H
