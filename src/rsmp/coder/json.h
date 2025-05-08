/**
* @file json.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_CODER_JSON_H
#define RSMP_CODER_JSON_H
#include <stdbool.h>
#include "rsmp/coder/vars.h"
#include "rsmp/messages/messages.h"
#include <json-c/json.h> // \todo reflexion separation json par rapport à verrue rsmp_json_msg2payload_with_inserting_jobj
/**
 * @defgroup json 
 * 
 * @brief 
 * 
 * @{
 */

#ifdef RSMP_CODER_JSON_C
#define JSON_EXTERN
#define JSON_PRIVATE
#endif

#ifndef JSON_EXTERN
#define JSON_EXTERN extern
#endif

#ifndef JSON_PRIVATE
#define JSON_PRIVATE const
#endif

#ifdef YCO
typedef void rsmp_json_t;
typedef enum rsmp_json_value_type
{
    _rsmp_json_value_str_,
    _rsmp_json_value_json_,
    _rsmp_json_value_array_,
} rsmp_json_value_type_t;

typedef struct rsmp_json_value_str
{
    JSON_PRIVATE rsmp_json_value_type_t type;
    const char *value;
} rsmp_json_value_str_t;

typedef struct rsmp_json_value_json
{
    JSON_PRIVATE rsmp_json_value_type_t type;
    rsmp_json_t const *value;
} rsmp_json_value_json_t;

typedef struct rsmp_json_value_array
{
    JSON_PRIVATE rsmp_json_value_type_t type;
    rsmp_json_t const *value;
} rsmp_json_value_array_t;

typedef union rsmp_json_value
{
    rsmp_json_value_type_t const *t;
    rsmp_json_value_json_t       *j;
    rsmp_json_value_array_t      *a;
    rsmp_json_value_str_t        *s;
} rsmp_json_value_t __attribute__((__transparent_union__));


rsmp_json_t *rsmp_str_to_json(char const *str);
char *rsmp_json_to_str(rsmp_json_t const *json);
void rmp_json_free(rsmp_json_t *json);
bool rsmp_json_field_exist(rsmp_json_t *json, char const *field);
bool rsmp_json_field_get(rsmp_json_t *json, char const *field, rsmp_json_value_t *value);
#endif

char const *rsmp_json_msg2payload(rsmp_messages_ptr msg);
rsmp_messages_ptr rsmp_json_payload2msg(rsmp_context_t const *context, char const *payload);
char const *rsmp_json_msg2payload_with_inserting_jobj(rsmp_messages_ptr msg, char *key, json_object *jobj_to_insert); // \todo reflexion separation json, verrue pour inserer le resultat des sxl GEN

static int rsmp_engine_send_msg(rsmp_engine_t *engine, rsmp_messages_ptr msg, const void *attrib)
{
    const char *payload = rsmp_json_msg2payload(msg);
    return engine->send(engine, payload, strlen(payload), attrib);
}


// pour traitement stat-response GEN \todo reflexion separation modules
void json_obj_free(json_object *jobj);

/**@}*/
#endif //RSMP_CODER_JSON_H
