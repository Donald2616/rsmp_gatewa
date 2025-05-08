#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "disp_m31.h"
#include "disp_json_generic.h"
#include "disp_m40.h"
#include "disp_m41.h"
#include <string.h>
#include "disp_s20.h"
#include "disp_s10.h"
#include "disp_s40.h"



/**
 * @brief Encode une réponse DISP/S30 (StatusResponse) en JSON.
 *
 * Cette fonction convertit une structure `DispS30StatusResponse` en un objet JSON
 * contenant la liste des identifiants de messages disponibles.
 *
 * @param res Pointeur vers la structure à encoder
 * @return Un pointeur vers un objet JSON (à libérer avec `json_object_put`), ou NULL en cas d’erreur
 */
struct json_object* encode_disp_s30_status_response(const DispS30StatusResponse* res) {
    if (!res) return NULL;

    // Création de l'objet racine JSON
    struct json_object* root = json_object_new_object();

    // Création du tableau JSON msgIdList
    struct json_object* msg_array = json_object_new_array();

    // Parcourt les identifiants de messages et les ajoute au tableau
    for (size_t i = 0; i < res->msgIdCount; ++i) {
        json_object_array_add(msg_array, json_object_new_string(res->msgIdList[i]));
    }

    // Ajout du tableau au champ "msgIdList"
    json_object_object_add(root, "msgIdList", msg_array);

    return root;
}


/**
 * @brief Décode une réponse JSON DISP/S30 (StatusResponse) en structure C.
 *
 * Cette fonction parse un JSON contenant un champ `msgIdList` et remplit la structure
 * `DispS30StatusResponse` avec les chaînes de caractères correspondantes.
 *
 * @param json_str Chaîne JSON à décoder
 * @param res Pointeur vers la structure à remplir
 * @return 0 en cas de succès, -1 en cas d’erreur
 */
int decode_disp_s30_status_response(const char* json_str, DispS30StatusResponse* res) {
    if (!json_str || !res) return -1;

    // Parse la chaîne JSON
    struct json_object* root = json_tokener_parse(json_str);
    if (!root) return -1;

    // Récupère le tableau "msgIdList"
    struct json_object* j_list;
    if (!json_object_object_get_ex(root, "msgIdList", &j_list)) {
        json_object_put(root);
        return -1;
    }

    // Alloue la mémoire pour le tableau de chaînes
    size_t count = json_object_array_length(j_list);
    res->msgIdCount = count;
    res->msgIdList = calloc(count, sizeof(char*));

    // Copie chaque chaîne du tableau JSON
    for (size_t i = 0; i < count; ++i) {
        const char* str = json_object_get_string(json_object_array_get_idx(j_list, i));
        res->msgIdList[i] = strdup(str);  // alloue une nouvelle chaîne copiée
    }

    json_object_put(root);
    return 0;
}



/**
 * @brief Encode une commande DISP/M31 (CommandRequest) en JSON
 *
 * @param request Structure contenant les messages à encoder
 * @return json_object* Objet JSON à envoyer (à libérer avec json_object_put)
 */
 struct json_object* encode_disp_m31_command_request(const DispM31CommandRequest* request) {
    if (!request) return NULL;

    struct json_object* j_messages = json_object_new_array();

    for (int i = 0; i < request->messageCount; ++i) {
        const MessageObject* msg = &request->messages[i];
        struct json_object* j_msg = json_object_new_object();

        json_object_object_add(j_msg, "msgId", json_object_new_string(msg->msgId));
        json_object_object_add(j_msg, "repeatCount", json_object_new_int(msg->repeatCount));

        // Séquences
        struct json_object* j_sequences = json_object_new_array();
        for (int j = 0; j < msg->sequenceCount; ++j) {
            const Sequence* seq = &msg->sequences[j];
            struct json_object* j_seq = json_object_new_object();

            json_object_object_add(j_seq, "templateId", json_object_new_string(seq->templateId));
            json_object_object_add(j_seq, "end", json_object_new_string(seq->end));

            // Animations
            struct json_object* j_anims = json_object_new_array();
            for (int k = 0; k < seq->animationCount; ++k) {
                const Animation* anim = &seq->animations[k];
                struct json_object* j_anim = json_object_new_object();

                json_object_object_add(j_anim, "zoneId", json_object_new_string(anim->zoneId));
                json_object_object_add(j_anim, "animType", json_object_new_string(anim->animType));

                // Contenus
                struct json_object* j_contents = json_object_new_array();
                for (int c = 0; c < anim->contentCount; ++c) {
                    const Content* content = &anim->contents[c];
                    struct json_object* j_content = json_object_new_object();

                    json_object_object_add(j_content, "contentType", json_object_new_string(content->contentType));
                    json_object_object_add(j_content, "contextText", json_object_new_string(content->contextText));

                    struct json_object* j_duration = json_object_new_object();
                    json_object_object_add(j_duration, "onDuration", json_object_new_int(content->duration.onDuration));
                    json_object_object_add(j_content, "duration", j_duration);

                    json_object_array_add(j_contents, j_content);
                }

                json_object_object_add(j_anim, "contents", j_contents);
                json_object_array_add(j_anims, j_anim);
            }

            json_object_object_add(j_seq, "animations", j_anims);
            json_object_array_add(j_sequences, j_seq);
        }

        json_object_object_add(j_msg, "sequences", j_sequences);
        json_object_array_add(j_messages, j_msg);
    }

    struct json_object* wrapper = json_object_new_object();
    json_object_object_add(wrapper, "messages", j_messages);
    return wrapper;
}

/**
 * @brief Décode une réponse JSON DISP/M31 (CommandResponse) en structure C
 *
 * @param json_str Chaîne JSON à décoder
 * @param response Pointeur vers la structure à remplir
 * @return int 0 si succès, -1 si erreur
 */
int decode_disp_m31_command_response(const char* json_str, DispM31CommandResponse* response) {
    if (!json_str || !response) return -1;

    struct json_object* root = json_tokener_parse(json_str);
    if (!root) return -1;

    struct json_object* retMessages;
    if (!json_object_object_get_ex(root, "retMessages", &retMessages)) {
        json_object_put(root);
        return -1;
    }

    int count = json_object_array_length(retMessages);
    response->retMessageCount = count;
    response->retMessages = calloc(count, sizeof(RetMessageObject));

    for (int i = 0; i < count; ++i) {
        struct json_object* j_ret = json_object_array_get_idx(retMessages, i);
        struct json_object *j_msgId, *j_retStatus, *j_code, *j_rea;

        if (json_object_object_get_ex(j_ret, "msgId", &j_msgId))
            strncpy(response->retMessages[i].msgId, json_object_get_string(j_msgId), sizeof(response->retMessages[i].msgId));

        if (json_object_object_get_ex(j_ret, "retStatus", &j_retStatus)) {
            if (json_object_object_get_ex(j_retStatus, "code", &j_code))
                strncpy(response->retMessages[i].retStatus.code, json_object_get_string(j_code), sizeof(response->retMessages[i].retStatus.code));

            if (json_object_object_get_ex(j_retStatus, "rea", &j_rea))
                strncpy(response->retMessages[i].retStatus.rea, json_object_get_string(j_rea), sizeof(response->retMessages[i].retStatus.rea));
        }
    }

    json_object_put(root);
    return 0;
}


/**
 * @brief Encode une commande DISP/M32 (CommandRequest) en JSON
 *
 * @param req Structure contenant les IDs à supprimer
 * @return json_object* Objet JSON à envoyer (à libérer avec json_object_put)
 */
 struct json_object* encode_disp_m32_command_request(const DispM32CommandRequest* req) {
    if (!req || req->msgIdCount == 0 || !req->msgIdList) return NULL;

    struct json_object* j_array = json_object_new_array();
    for (int i = 0; i < req->msgIdCount; ++i) {
        json_object_array_add(j_array, json_object_new_string(req->msgIdList[i]));
    }

    struct json_object* wrapper = json_object_new_object();
    json_object_object_add(wrapper, "msgIdList", j_array);
    return wrapper;
}

/**
 * @brief Décode une réponse JSON DISP/M32 (CommandResponse) en structure C
 *
 * @param json_str Chaîne JSON à décoder
 * @param res Pointeur vers la structure à remplir
 * @return int 0 si succès, -1 si erreur
 */
int decode_disp_m32_command_response(const char* json_str, DispM32CommandResponse* res) {
    if (!json_str || !res) return -1;

    struct json_object* root = json_tokener_parse(json_str);
    if (!root) return -1;

    struct json_object* retMessages;
    if (!json_object_object_get_ex(root, "retMessages", &retMessages)) {
        json_object_put(root);
        return -1;
    }

    int count = json_object_array_length(retMessages);
    res->retMessageCount = count;
    res->retMessages = calloc(count, sizeof(RetMessageObject));

    for (int i = 0; i < count; ++i) {
        struct json_object* j_ret = json_object_array_get_idx(retMessages, i);
        struct json_object *j_msgId, *j_retStatus, *j_code, *j_rea;

        if (json_object_object_get_ex(j_ret, "msgId", &j_msgId))
            strncpy(res->retMessages[i].msgId, json_object_get_string(j_msgId), sizeof(res->retMessages[i].msgId));

        if (json_object_object_get_ex(j_ret, "retStatus", &j_retStatus)) {
            if (json_object_object_get_ex(j_retStatus, "code", &j_code))
                strncpy(res->retMessages[i].retStatus.code, json_object_get_string(j_code), sizeof(res->retMessages[i].retStatus.code));

            if (json_object_object_get_ex(j_retStatus, "rea", &j_rea))
                strncpy(res->retMessages[i].retStatus.rea, json_object_get_string(j_rea), sizeof(res->retMessages[i].retStatus.rea));
        }
    }

    json_object_put(root);
    return 0;
}


/**
 * @brief Encode une commande DISP/M40 (CommandRequest) en objet JSON.
 *
 * @param req Pointeur vers la structure contenant la commande DISP/M40 à encoder
 * @return json_object* Objet JSON à envoyer (à libérer avec json_object_put)
 */
 struct json_object* encode_disp_m40_command_request(const DispM40CommandRequest* req) {
    if (!req) return NULL;

    struct json_object* j_obj = json_object_new_object();

    // Ajout des champs simples
    json_object_object_add(j_obj, "msgId", json_object_new_string(req->msgId));
    json_object_object_add(j_obj, "displayBehavior", json_object_new_string(req->displayBehavior));
    json_object_object_add(j_obj, "maxWaitTime", json_object_new_int(req->maxWaitTime));
    json_object_object_add(j_obj, "displayDuration", json_object_new_int(req->displayDuration));
    json_object_object_add(j_obj, "displayPriority", json_object_new_int(req->displayPriority));

    // Ajout de dynamicData uniquement si non vide
    if (strlen(req->dynamicData) > 0) {
        json_object_object_add(j_obj, "dynamicData", json_object_new_string(req->dynamicData));
    }

    return j_obj;
}


/**
 * @brief Décode une réponse JSON DISP/M40 (CommandResponse) en structure C.
 *
 * @param json_str Chaîne JSON à décoder
 * @param res Structure C à remplir
 * @return 0 si succès, -1 si échec
 */
 int decode_disp_m40_command_response(const char* json_str, DispM40CommandResponse* res) {
    if (!json_str || !res) return -1;

    struct json_object* root = json_tokener_parse(json_str);
    if (!root) return -1;

    struct json_object *j_msgId, *j_behaviorUsed, *j_retStatus, *j_code, *j_rea;

    if (json_object_object_get_ex(root, "msgId", &j_msgId))
        strncpy(res->msgId, json_object_get_string(j_msgId), sizeof(res->msgId));

    if (json_object_object_get_ex(root, "behaviorUsed", &j_behaviorUsed)) {
        const char* behavior = json_object_get_string(j_behaviorUsed);
        strncpy(res->behaviorUsed, behavior ? behavior : "", sizeof(res->behaviorUsed));
    }

    if (json_object_object_get_ex(root, "retStatus", &j_retStatus)) {
        if (json_object_object_get_ex(j_retStatus, "code", &j_code))
            strncpy(res->retStatus.code, json_object_get_string(j_code), sizeof(res->retStatus.code));

        if (json_object_object_get_ex(j_retStatus, "rea", &j_rea))
            strncpy(res->retStatus.rea, json_object_get_string(j_rea), sizeof(res->retStatus.rea));
    }

    json_object_put(root);
    return 0;
}



/**
 * @brief Encode une commande DISP/M41 (CommandRequest) en JSON
 *
 * @param req Structure contenant les IDs à supprimer
 * @return json_object* Objet JSON à envoyer (à libérer avec json_object_put)
 */
struct json_object* encode_disp_m41_command_request(const DispM41CommandRequest* req) {
    if (!req) return NULL;

    struct json_object* j_obj = json_object_new_object();
    json_object_object_add(j_obj, "msgId", json_object_new_string(req->msgId));
    json_object_object_add(j_obj, "displayPriority", json_object_new_int(req->displayPriority));

    return j_obj;
}

int decode_disp_m41_command_response(const char* json_str, DispM41CommandResponse* res) {
    if (!json_str || !res) return -1;

    struct json_object* root = json_tokener_parse(json_str);
    if (!root) return -1;

    struct json_object *j_msgId, *j_priority, *j_retStatus, *j_code, *j_rea;

    if (json_object_object_get_ex(root, "msgId", &j_msgId))
        strncpy(res->msgId, json_object_get_string(j_msgId), sizeof(res->msgId));

    if (json_object_object_get_ex(root, "displayPriority", &j_priority))
        res->displayPriority = json_object_get_int(j_priority);

    if (json_object_object_get_ex(root, "retStatus", &j_retStatus)) {
        if (json_object_object_get_ex(j_retStatus, "code", &j_code))
            strncpy(res->retStatus.code, json_object_get_string(j_code), sizeof(res->retStatus.code));
        if (json_object_object_get_ex(j_retStatus, "rea", &j_rea))
            strncpy(res->retStatus.rea, json_object_get_string(j_rea), sizeof(res->retStatus.rea));
    }

    json_object_put(root);
    return 0;
}


/**
 * @brief Encode une réponse DISP/S20 (StatusResponse) en JSON
 * @param res Pointeur vers la structure à encoder
 * @return Un objet JSON représentant la réponse (à libérer avec json_object_put), ou NULL en cas d'erreur
 */
struct json_object* encode_disp_s20_status_response(const DispS20StatusResponse* res) {
    if (!res) return NULL;

    struct json_object* root = json_object_new_object();
    struct json_object* templates_array = json_object_new_array();

    for (int i = 0; i < res->templateCount; ++i) {
        const TemplateObject* tpl = &res->templates[i];
        struct json_object* tpl_obj = json_object_new_object();

        json_object_object_add(tpl_obj, "templateId", json_object_new_string(tpl->templateId));
        json_object_object_add(tpl_obj, "readonly", json_object_new_boolean(tpl->readonly));

        struct json_object* zones_array = json_object_new_array();
        for (int j = 0; j < tpl->zoneCount; ++j) {
            const ZoneObject* zone = &tpl->zones[j];
            struct json_object* zone_obj = json_object_new_object();

            json_object_object_add(zone_obj, "zoneId", json_object_new_string(zone->zoneId));

            struct json_object* colors_array = json_object_new_array();
            for (int c = 0; c < zone->colorCount; ++c) {
                json_object_array_add(colors_array, json_object_new_string(zone->supportedColors[c]));
            }
            json_object_object_add(zone_obj, "supportedColors", colors_array);

            struct json_object* pos = json_object_new_object();
            json_object_object_add(pos, "x", json_object_new_int(zone->position.x));
            json_object_object_add(pos, "y", json_object_new_int(zone->position.y));
            json_object_object_add(pos, "z", json_object_new_int(zone->position.z));
            json_object_object_add(zone_obj, "position", pos);

            json_object_object_add(zone_obj, "zoneType", json_object_new_string(zone->zoneType));

            // Options conditionnelles
            if (zone->textOptions) {
                struct json_object* text = json_object_new_object();
                json_object_object_add(text, "type", json_object_new_string(zone->textOptions->type));
                json_object_object_add(text, "textColor", json_object_new_string(zone->textOptions->textColor));
                json_object_object_add(text, "backgroundColor", json_object_new_string(zone->textOptions->backgroundColor));
                json_object_object_add(text, "font", json_object_new_string(zone->textOptions->font));
                json_object_object_add(text, "horizontalAlign", json_object_new_string(zone->textOptions->horizontalAlign));
                json_object_object_add(text, "verticalAlign", json_object_new_string(zone->textOptions->verticalAlign));
                json_object_object_add(text, "orientation", json_object_new_string(zone->textOptions->orientation));
                json_object_object_add(zone_obj, "textOptions", text);
            } else {
                json_object_object_add(zone_obj, "textOptions", NULL);
            }

            if (zone->graphicOptions) {
                struct json_object* g = json_object_new_object();
                json_object_object_add(g, "sizeNegotiation", json_object_new_string(zone->graphicOptions->negotiation));
                struct json_object* size = json_object_new_object();
                json_object_object_add(size, "w", json_object_new_int(zone->graphicOptions->size.w));
                json_object_object_add(size, "h", json_object_new_int(zone->graphicOptions->size.h));
                json_object_object_add(g, "size", size);
                json_object_object_add(zone_obj, "graphicOptions", g);
            } else {
                json_object_object_add(zone_obj, "graphicOptions", NULL);
            }

            if (zone->predefinedOptions) {
                struct json_object* p = json_object_new_object();
                json_object_object_add(p, "predefinedGroupId", json_object_new_string(zone->predefinedOptions->predefinedGroupId));
                struct json_object* size = json_object_new_object();
                json_object_object_add(size, "w", json_object_new_int(zone->predefinedOptions->size.w));
                json_object_object_add(size, "h", json_object_new_int(zone->predefinedOptions->size.h));
                json_object_object_add(p, "size", size);
                json_object_object_add(zone_obj, "predefinedOptions", p);
            } else {
                json_object_object_add(zone_obj, "predefinedOptions", NULL);
            }

            if (zone->customOptions) {
                json_object_object_add(zone_obj, "customOptions", json_object_new_object());
            } else {
                json_object_object_add(zone_obj, "customOptions", NULL);
            }

            json_object_array_add(zones_array, zone_obj);
        }

        json_object_object_add(tpl_obj, "zones", zones_array);
        json_object_array_add(templates_array, tpl_obj);
    }

    json_object_object_add(root, "templates", templates_array);
    return root;
}



int decode_disp_s20_status_response(const char* json_str, DispS20StatusResponse* res) {
    if (json_str == NULL || res == NULL) {
        fprintf(stderr, "Erreur : Paramètres NULL dans decode_disp_s20_status_response\n");
        return -1;
    }

    // Créer un objet JSON à partir de la chaîne
    struct json_object* root = json_tokener_parse(json_str);
    if (root == NULL) {
        fprintf(stderr, "Erreur : Impossible de parser la chaîne JSON\n");
        return -1;
    }

    // Récupérer le tableau de templates
    struct json_object* templates_array;
    if (!json_object_object_get_ex(root, "templates", &templates_array) || !json_object_is_type(templates_array, json_type_array)) {
        fprintf(stderr, "Erreur : Champ 'templates' manquant ou invalide\n");
        json_object_put(root);
        return -1;
    }

    // Nombre de templates
    int template_count = json_object_array_length(templates_array);
    res->templateCount = template_count;
    res->templates = calloc(template_count, sizeof(TemplateObject));

    if (res->templates == NULL) {
        fprintf(stderr, "Erreur : Allocation mémoire pour les templates\n");
        json_object_put(root);
        return -1;
    }

    // Boucle pour lire chaque template
    for (int i = 0; i < template_count; i++) {
        struct json_object* template_obj = json_object_array_get_idx(templates_array, i);
        TemplateObject* tpl = &res->templates[i];

        // Récupérer l'ID du template
        struct json_object* template_id;
        if (json_object_object_get_ex(template_obj, "templateId", &template_id)) {
            strncpy(tpl->templateId, json_object_get_string(template_id), sizeof(tpl->templateId) - 1);
        }

        // Lire le champ readonly
        struct json_object* readonly;
        if (json_object_object_get_ex(template_obj, "readonly", &readonly)) {
            tpl->readonly = json_object_get_boolean(readonly);
        }

        // Lire les zones
        struct json_object* zones_array;
        if (json_object_object_get_ex(template_obj, "zones", &zones_array) && json_object_is_type(zones_array, json_type_array)) {
            tpl->zoneCount = json_object_array_length(zones_array);
            tpl->zones = calloc(tpl->zoneCount, sizeof(ZoneObject));

            for (int j = 0; j < tpl->zoneCount; j++) {
                struct json_object* zone_obj = json_object_array_get_idx(zones_array, j);
                ZoneObject* zone = &tpl->zones[j];

                // Récupérer l'ID de la zone
                struct json_object* zone_id;
                if (json_object_object_get_ex(zone_obj, "zoneId", &zone_id)) {
                    strncpy(zone->zoneId, json_object_get_string(zone_id), sizeof(zone->zoneId) - 1);
                }

                // Lire le type de zone
                struct json_object* zone_type;
                if (json_object_object_get_ex(zone_obj, "zoneType", &zone_type)) {
                    strncpy(zone->zoneType, json_object_get_string(zone_type), sizeof(zone->zoneType) - 1);
                }

                // Lire le nombre de couleurs supportées
                struct json_object* color_count;
                if (json_object_object_get_ex(zone_obj, "colorCount", &color_count)) {
                    zone->colorCount = json_object_get_int(color_count);
                }
            }
        }
    }

    // Libérer la mémoire JSON
    json_object_put(root);
    return 0;
}


/**
* @brief Encode une réponse DISP/S10 (StatusResponse) en JSON.
*
* Cette fonction convertit une structure C DispS10StatusResponse en un objet JSON
* conforme au format attendu du protocole RSMP.
*
* @param res Pointeur vers la structure à encoder
* @return Un objet JSON représentant la réponse (à libérer avec json_object_put), ou NULL en cas d'erreur
*/
struct json_object* encode_disp_s10_status_response(const DispS10StatusResponse* res) {
   if (!res) return NULL;

   struct json_object* root = json_object_new_object();

   // Champs simples
   json_object_object_add(root, "displayType", json_object_new_string(res->displayType));
   json_object_object_add(root, "templateType", json_object_new_string(res->templateType));
   json_object_object_add(root, "minDisplayTime", json_object_new_int(res->minDisplayTime));
   json_object_object_add(root, "maxObjectIdSize", json_object_new_int(res->maxObjectIdSize));
   json_object_object_add(root, "maxSupportedTemplate", json_object_new_int(res->maxSupportedTemplate));
   json_object_object_add(root, "maxSupportedZone", json_object_new_int(res->maxSupportedZone));
   json_object_object_add(root, "maxSupportedSequence", json_object_new_int(res->maxSupportedSequence));
   json_object_object_add(root, "maxSupportedAnimation", json_object_new_int(res->maxSupportedAnimation));
   json_object_object_add(root, "maxSupportedAvailableMsg", json_object_new_int(res->maxSupportedAvailableMsg));
   json_object_object_add(root, "maxSupportedDisplayPool", json_object_new_int(res->maxSupportedDisplayPool));
   json_object_object_add(root, "maxScrollSpeed", json_object_new_int(res->maxScrollSpeed));

   // Objet "size"
   struct json_object* size = json_object_new_object();
   json_object_object_add(size, "width", json_object_new_int(res->size.width));
   json_object_object_add(size, "height", json_object_new_int(res->size.height));
   json_object_object_add(root, "size", size);

   // Affichage texte autorisé
   json_object_object_add(root, "textDisplayable", json_object_new_boolean(res->textDisplayable));

   // Groupes prédéfinis
   if (res->predefinedGroupCount > 0 && res->predefinedGroups) {
       struct json_object* groups = json_object_new_array();

       for (unsigned int i = 0; i < res->predefinedGroupCount; ++i) {
           struct json_object* group = json_object_new_object();
           json_object_object_add(group, "predefinedGroupId",
                                  json_object_new_string(res->predefinedGroups[i].predefinedGroupId));

           struct json_object* enum_array = json_object_new_array();
           for (unsigned int j = 0; j < res->predefinedGroups[i].enumCount; ++j) {
               json_object_array_add(enum_array,
                                     json_object_new_string(res->predefinedGroups[i].supportedEnum[j]));
           }

           json_object_object_add(group, "supportedPredefined", enum_array);
           json_object_array_add(groups, group);
       }

       json_object_object_add(root, "predefinedGroups", groups);
   } else {
       json_object_object_add(root, "predefinedGroups", NULL);
   }

   return root;
}
  

/**
 * @brief Décode une réponse JSON DISP/S10 en une structure DispS10StatusResponse.
 *
 * @param json_str Chaîne JSON à décoder.
 * @param res Pointeur vers la structure de réponse à remplir.
 * @return int Code d'erreur (0 si succès).
 */
int decode_disp_s10_status_response(const char* json_str, DispS10StatusResponse* res) {
    if (json_str == NULL || res == NULL) {
        fprintf(stderr, "[DISP/S10] Erreur : chaîne JSON ou structure résultat NULL.\n");
        return -1;
    }

    // Parser la chaîne JSON
    struct json_object* json_obj = json_tokener_parse(json_str);
    if (json_obj == NULL) {
        fprintf(stderr, "[DISP/S10] Erreur de parsing JSON.\n");
        return -1;
    }

    // Initialiser la structure de réponse
    memset(res, 0, sizeof(DispS10StatusResponse));

    // Décoder displayType
    struct json_object* displayType_obj;
    if (json_object_object_get_ex(json_obj, "displayType", &displayType_obj)) {
        const char* displayType = json_object_get_string(displayType_obj);
        strncpy(res->displayType, displayType, sizeof(res->displayType) - 1);
    } else {
        fprintf(stderr, "[DISP/S10] Champ 'displayType' manquant.\n");
    }

    // Décoder templateType
    struct json_object* templateType_obj;
    if (json_object_object_get_ex(json_obj, "templateType", &templateType_obj)) {
        const char* templateType = json_object_get_string(templateType_obj);
        strncpy(res->templateType, templateType, sizeof(res->templateType) - 1);
    } else {
        fprintf(stderr, "[DISP/S10] Champ 'templateType' manquant.\n");
    }

    // Décoder les champs numériques
    json_object_object_get_ex(json_obj, "minDisplayTime", &displayType_obj);
    res->minDisplayTime = json_object_get_int(displayType_obj);

    json_object_object_get_ex(json_obj, "maxObjectIdSize", &displayType_obj);
    res->maxObjectIdSize = json_object_get_int(displayType_obj);

    json_object_object_get_ex(json_obj, "maxSupportedTemplate", &displayType_obj);
    res->maxSupportedTemplate = json_object_get_int(displayType_obj);

    json_object_object_get_ex(json_obj, "maxSupportedZone", &displayType_obj);
    res->maxSupportedZone = json_object_get_int(displayType_obj);

    json_object_object_get_ex(json_obj, "maxSupportedSequence", &displayType_obj);
    res->maxSupportedSequence = json_object_get_int(displayType_obj);

    json_object_object_get_ex(json_obj, "maxSupportedAnimation", &displayType_obj);
    res->maxSupportedAnimation = json_object_get_int(displayType_obj);

    json_object_object_get_ex(json_obj, "maxSupportedAvailableMsg", &displayType_obj);
    res->maxSupportedAvailableMsg = json_object_get_int(displayType_obj);

    json_object_object_get_ex(json_obj, "maxSupportedDisplayPool", &displayType_obj);
    res->maxSupportedDisplayPool = json_object_get_int(displayType_obj);

    json_object_object_get_ex(json_obj, "maxScrollSpeed", &displayType_obj);
    res->maxScrollSpeed = json_object_get_int(displayType_obj);

    // Libérer les ressources JSON
    json_object_put(json_obj);

    return 0;
}


 /**
 * @brief Encode une réponse DISP/S40 en JSON.
 */
struct json_object* encode_disp_s40_status_response(const DispS40StatusResponse* res) {
    if (!res) return NULL;

    struct json_object* j_root = json_object_new_object();
    struct json_object* j_list = json_object_new_array();

    for (unsigned int i = 0; i < res->displayPoolCount; ++i) {
        const DisplayPoolElementObject* elem = &res->displayPoolList[i];
        struct json_object* j_elem = json_object_new_object();

        json_object_object_add(j_elem, "msgId", json_object_new_string(elem->msgId));
        json_object_object_add(j_elem, "maxWaitTime", json_object_new_int(elem->maxWaitTime));
        json_object_object_add(j_elem, "maxWaitTimeRemain", json_object_new_int(elem->maxWaitTimeRemain));
        json_object_object_add(j_elem, "displayDuration", json_object_new_int(elem->displayDuration));
        json_object_object_add(j_elem, "displayDurationRemain", json_object_new_int(elem->displayDurationRemain));
        json_object_object_add(j_elem, "displayPriority", json_object_new_int(elem->displayPriority));
        json_object_object_add(j_elem, "dynamicData", json_object_new_string(elem->dynamicData));

        json_object_array_add(j_list, j_elem);
    }

    json_object_object_add(j_root, "displayPoolList", j_list);
    return j_root;
}

/**
 * @brief Décode une réponse DISP/S40 depuis un JSON.
 */
int decode_disp_s40_status_response(const char* json_str, DispS40StatusResponse* res) {
    if (!json_str || !res) return -1;

    struct json_object* root = json_tokener_parse(json_str);
    if (!root) return -1;

    struct json_object* j_list;
    if (!json_object_object_get_ex(root, "displayPoolList", &j_list)) {
        json_object_put(root);
        return -1;
    }

    unsigned int count = json_object_array_length(j_list);
    res->displayPoolList = calloc(count, sizeof(DisplayPoolElementObject));
    res->displayPoolCount = count;

    for (unsigned int i = 0; i < count; ++i) {
        struct json_object* j_elem = json_object_array_get_idx(j_list, i);
        DisplayPoolElementObject* e = &res->displayPoolList[i];

        struct json_object* tmp;
        if (json_object_object_get_ex(j_elem, "msgId", &tmp))
            strncpy(e->msgId, json_object_get_string(tmp), sizeof(e->msgId));

        if (json_object_object_get_ex(j_elem, "maxWaitTime", &tmp))
            e->maxWaitTime = json_object_get_int(tmp);

        if (json_object_object_get_ex(j_elem, "maxWaitTimeRemain", &tmp))
            e->maxWaitTimeRemain = json_object_get_int(tmp);

        if (json_object_object_get_ex(j_elem, "displayDuration", &tmp))
            e->displayDuration = json_object_get_int(tmp);

        if (json_object_object_get_ex(j_elem, "displayDurationRemain", &tmp))
            e->displayDurationRemain = json_object_get_int(tmp);

        if (json_object_object_get_ex(j_elem, "displayPriority", &tmp))
            e->displayPriority = json_object_get_int(tmp);

        if (json_object_object_get_ex(j_elem, "dynamicData", &tmp))
            strncpy(e->dynamicData, json_object_get_string(tmp), sizeof(e->dynamicData));
    }

    json_object_put(root);
    return 0;
}
/**
 * @brief Décode une réponse JSON DISP/S31 en structure C
 *
 * @param json_str Chaîne JSON à décoder
 * @param res Pointeur vers la structure `DispS31StatusResponse` à remplir
 * @return int 0 si succès, -1 en cas d’erreur
 */
 /*
int decode_disp_s31_status_response(const char* json_str, DispS31StatusResponse* res) {
    if (!json_str || !res) return -1;

    struct json_object* root = json_tokener_parse(json_str);
    if (!root) return -1;

    struct json_object* j_messages;
    if (!json_object_object_get_ex(root, "messages", &j_messages)) {
        json_object_put(root);
        return -1;
    }

    res->messageCount = json_object_array_length(j_messages);
    res->messages = calloc(res->messageCount, sizeof(MessageObject));

    for (unsigned int i = 0; i < res->messageCount; ++i) {
        struct json_object* j_msg = json_object_array_get_idx(j_messages, i);
        MessageObject* msg = &res->messages[i];

        struct json_object* j_val;

        if (json_object_object_get_ex(j_msg, "msgId", &j_val))
            strncpy(msg->msgId, json_object_get_string(j_val), sizeof(msg->msgId));

        if (json_object_object_get_ex(j_msg, "repeatCount", &j_val))
            msg->repeatCount = json_object_get_int(j_val);

        if (json_object_object_get_ex(j_msg, "autoDestruct", &j_val))
            msg->autoDestruct = json_object_get_boolean(j_val);

        // --- Séquences ---
        struct json_object* j_seqs;
        if (json_object_object_get_ex(j_msg, "sequences", &j_seqs)) {
            msg->sequenceCount = json_object_array_length(j_seqs);
            msg->sequences = calloc(msg->sequenceCount, sizeof(SequenceObject));

            for (unsigned int j = 0; j < msg->sequenceCount; ++j) {
                struct json_object* j_seq = json_object_array_get_idx(j_seqs, j);
                SequenceObject* seq = &msg->sequences[j];

                if (json_object_object_get_ex(j_seq, "templateId", &j_val))
                    strncpy(seq->templateId, json_object_get_string(j_val), sizeof(seq->templateId));

                struct json_object* j_end;
                if (json_object_object_get_ex(j_seq, "end", &j_end)) {
                    struct json_object* j_end_val;
                    if (json_object_object_get_ex(j_end, "endType", &j_end_val))
                        strncpy(seq->end.endType, json_object_get_string(j_end_val), sizeof(seq->end.endType));

                    if (json_object_object_get_ex(j_end, "zoneIdFinisher", &j_end_val))
                        strncpy(seq->end.zoneIdFinisher, json_object_get_string(j_end_val), sizeof(seq->end.zoneIdFinisher));

                    if (json_object_object_get_ex(j_end, "seqDuration", &j_end_val))
                        seq->end.seqDuration = json_object_get_int(j_end_val);
                }

                // --- Animations ---
                struct json_object* j_anims;
                if (json_object_object_get_ex(j_seq, "animations", &j_anims)) {
                    seq->animationCount = json_object_array_length(j_anims);
                    seq->animations = calloc(seq->animationCount, sizeof(SequenceAnimationObject));

                    for (unsigned int k = 0; k < seq->animationCount; ++k) {
                        struct json_object* j_anim = json_object_array_get_idx(j_anims, k);
                        SequenceAnimationObject* anim = &seq->animations[k];

                        if (json_object_object_get_ex(j_anim, "zoneId", &j_val))
                            strncpy(anim->zoneId, json_object_get_string(j_val), sizeof(anim->zoneId));

                        if (json_object_object_get_ex(j_anim, "animType", &j_val))
                            strncpy(anim->animType, json_object_get_string(j_val), sizeof(anim->animType));

                        // --- Contenus ---
                        struct json_object* j_contents;
                        if (json_object_object_get_ex(j_anim, "contents", &j_contents)) {
                            anim->contentCount = json_object_array_length(j_contents);
                            anim->contents = calloc(anim->contentCount, sizeof(ContentObject));

                            for (unsigned int c = 0; c < anim->contentCount; ++c) {
                                struct json_object* j_content = json_object_array_get_idx(j_contents, c);
                                ContentObject* content = &anim->contents[c];

                                struct json_object* j_type;
                                if (json_object_object_get_ex(j_content, "contentType", &j_type)) {
                                    strncpy(content->contentType, json_object_get_string(j_type), sizeof(content->contentType));
                                }

                                struct json_object* j_dur;
                                if (json_object_object_get_ex(j_content, "duration", &j_dur)) {
                                    struct json_object* val;
                                    if (json_object_object_get_ex(j_dur, "firstPhase", &val))
                                        strncpy(content->duration.firstPhase, json_object_get_string(val), sizeof(content->duration.firstPhase));
                                    if (json_object_object_get_ex(j_dur, "onDuration", &val))
                                        content->duration.onDuration = json_object_get_int(val);
                                    if (json_object_object_get_ex(j_dur, "offDuration", &val))
                                        content->duration.offDuration = json_object_get_int(val);
                                }

                                // Text / Predefined / Custom
                                if (strcmp(content->contentType, "Text") == 0) {
                                    struct json_object* text;
                                    if (json_object_object_get_ex(j_content, "contentText", &text)) {
                                        struct json_object* chars;
                                        if (json_object_object_get_ex(text, "characters", &chars))
                                            strncpy(content->contentText.characters, json_object_get_string(chars), sizeof(content->contentText.characters));
                                    }
                                } else if (strcmp(content->contentType, "Predefined") == 0) {
                                    struct json_object* pred;
                                    if (json_object_object_get_ex(j_content, "contentPredefined", &pred)) {
                                        struct json_object* pid;
                                        if (json_object_object_get_ex(pred, "predefinedId", &pid))
                                            strncpy(content->contentPredefined.predefinedId, json_object_get_string(pid), sizeof(content->contentPredefined.predefinedId));
                                    }
                                } else if (strcmp(content->contentType, "Custom") == 0) {
                                    struct json_object* cust;
                                    if (json_object_object_get_ex(j_content, "contentCustom", &cust)) {
                                        struct json_object* id;
                                        if (json_object_object_get_ex(cust, "customAnimId", &id))
                                            strncpy(content->contentCustom.customAnimId, json_object_get_string(id), sizeof(content->contentCustom.customAnimId));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    json_object_put(root);
    return 0;
}
*/