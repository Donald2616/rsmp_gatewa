/**
* @file json.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <assert.h>
#include <errno.h>

#include "log/log.h"
#include "rsmp/rsmp.h"
#include "errcode/errcode.h"
#include "rsmp/vars/array.h"
// #include "rsmp/vars/arraystr.h"
#include "rsmp/fields/mType.h"

#define RSMP_CODER_JSON_C
#include "json.h"

/**
 * @ingroup json
 * @{
 */

/*static \todo suppression de "static" car utilisé dans stat-request ; decoupage modulaire eventuellement a revoir */void json_obj_free(json_object *jobj)
{
    if (!jobj) return;

    while(1) if (json_object_put(jobj)==1) break;
}

/*

88888888888  888b      88    ,ad8888ba,    ,ad8888ba,    88888888ba,    88888888888  88888888ba
88           8888b     88   d8"'    `"8b  d8"'    `"8b   88      `"8b   88           88      "8b
88           88 `8b    88  d8'           d8'        `8b  88        `8b  88           88      ,8P
88aaaaa      88  `8b   88  88            88          88  88         88  88aaaaa      88aaaaaa8P'
88"""""      88   `8b  88  88            88          88  88         88  88"""""      88""""88'
88           88    `8b 88  Y8,           Y8,        ,8P  88         8P  88           88    `8b
88           88     `8888   Y8a.    .a8P  Y8a.    .a8P   88      .a8P   88           88     `8b
88888888888  88      `888    `"Y8888Y"'    `"Y8888Y"'    88888888Y"'    88888888888  88      `8b


*/
static inline json_object *json_object_from_var(rsmp_var_t const *var)
{
    json_object *obj = NULL;
    switch(var->type)
    {
        case _rsmp_var_bool_ :
            obj = json_object_new_boolean(var->data.b);
            break;
        case _rsmp_var_string_ :
            obj = json_object_new_string_len(var->data.s.t, var->data.s.l);
            break;
        case _rsmp_var_int_ :
            obj = json_object_new_int64(var->data.i);
            break;
        case _rsmp_var_uint_ :
#if JSON_C_MAJOR_VERSION >= 0 && JSON_C_MINOR_VERSION >= 15
            obj = json_object_new_uint64(var->data.u);
#else
            obj = json_object_new_int64(var->data.u);
#endif
            break;
        case _rsmp_var_double_ :
            obj = json_object_new_double(var->data.b);
            break;
        default:
            break;
    }
    return obj;
}


static inline int json_object_var_add(json_object *jobj, const char *lib, rsmp_var_t const *var)
{
    assert(var != NULL);
    int ret = 1;
    json_object *obj = json_object_from_var(var);
    if (obj) ret = json_object_object_add(jobj, lib, obj);
    return ret;
}


static inline int json_add_field(json_object *jobj, void *msg, rsmp_field_desc_t const *field)
{
    assert(field != NULL);
    assert(msg != NULL);
    assert(field->ext.convert.encode != NULL);
    rsmp_var_t *var_ptr=NULL;    
    int ret = field->ext.convert.encode(msg + field->offset, &var_ptr);
    if (!ret)
    { 
        ret = json_object_var_add(jobj, field->lib, var_ptr);
    }
    rsmp_var_free(var_ptr);
    return ret;
}

static int json_add_fields(json_object *jobj, rsmp_messages_ptr msg, rsmp_field_desc_t const *fields);


static inline int json_add_array(json_object *jobj, void * msg, rsmp_field_desc_t const *field)
{
    int ret = _rsmp_no_error_;
    //-- Ajout d'un tableau en Json
    json_object *jarray = json_object_new_array();
    if (jarray)
    {
        rsmp_array_void_t *tab = msg + field->offset;
        //-- On va jouer sur un array sans connaître son type, mais avec la taille de l'offset...
        void *item = tab->items;
        for (size_t i=0; i< tab->len && !ret; i++, item += tab->item_len)
        {
            json_object *jitem = json_object_new_object();
            if (jitem)
            {
                ret = json_add_fields(jitem, (type_t *)item, field->ext.array);
                if (!ret)
                {
                    json_object_array_add(jarray, jitem);   
                }
                else
                {
                    json_obj_free(jitem);
                }
            }
            else
            {

            }
        }
        if (!ret)
        {
            json_object_object_add(jobj, field->lib, jarray);
        }
        else
        {
            json_obj_free(jarray);
        }
    }
    else 
    {


    }
    return ret;
}

static inline int json_add_table(json_object *jobj, void * msg, rsmp_field_desc_t const *field)
{
    int ret = 0;
    //-- Ajout d'un tableau en Json d'éléments unitaires et non object
    json_object *jarray = json_object_new_array();
    if (jarray)
    {
        rsmp_array_void_t *tab = msg + field->offset;
        //-- On va jouer sur un array sans connaître son type, mais avec la taille de l'offset...
        void *item = tab->items;
        rsmp_var_t *var=NULL;
        for (size_t i=0; i< tab->len && !ret; i++, item += tab->item_len)
        {
            ret = field->ext.convert.encode(item, &var);
            if (ret) break;
            json_object *obj = json_object_from_var(var);
            if (obj) json_object_array_add(jarray, obj);  
            else
            {
                ret = 1;
            }
        }
        rsmp_var_free(var);
        if (!ret)
        {
            json_object_object_add(jobj, field->lib, jarray);
        }
        else
        {
            json_obj_free(jarray);
        }
    }
    else 
    {


    }
    return ret;
}


static int json_add_fields(json_object *jobj, rsmp_messages_ptr msg, rsmp_field_desc_t const *fields)
{
    int ret = 0;
    uint i=0;
    while(fields[i].type != _rsmp_field_desc_end_mark_)
    {
        switch(fields[i].type)
        {
            case _rsmp_field_desc_field_mark_ :
                ret = json_add_field(jobj, msg.type, fields + i);
                break;
            case _rsmp_field_desc_array_mark_ :
                ret = json_add_array(jobj, msg.type, fields + i);
                break;
            case _rsmp_field_desc_table_mark_ :
                ret = json_add_table(jobj, msg.type, fields + i);
                break;
            default:
                break;
        }
        if (ret) break;
        i++; //-- On passe au suivant...
    }
    return ret;
}

typedef struct {
    const char *sg;
    const char *vers;
} def_eSXLG_t;

def_eSXLG_t eSXLG_def[] = {
    { "GEN", "1.0.0" },
    { "SLT", "0.5.2" },
    { "IO", "0.3.0" },
    { "VAR", "0.1.0" }
};

char* add_eSXLG_to_json(const char *json_str) {
    // Deserialize the JSON string into a JSON object
    struct json_object *root_obj = json_tokener_parse(json_str);
    if (!root_obj) {
        fprintf(stderr, "Failed to parse JSON string\n");
        return NULL;
    }

    // Create a new JSON array for eSXLG
    struct json_object *eSXLG_array = json_object_new_array();
    if (!eSXLG_array) {
        fprintf(stderr, "Failed to create JSON array\n");
        json_object_put(root_obj);
        return NULL;
    }

    // Populate the eSXLG array with the required objects
    for (size_t i = 0; i < sizeof(eSXLG_def) / sizeof(eSXLG_def[0]); i++) {
        struct json_object *item = json_object_new_object();
        json_object_object_add(item, "sg", json_object_new_string(eSXLG_def[i].sg));
        json_object_object_add(item, "vers", json_object_new_string(eSXLG_def[i].vers));
        json_object_array_add(eSXLG_array, item);
    }

    // Add the eSXLG array to the root JSON object
    json_object_object_add(root_obj, "SXLG", eSXLG_array);

    // Serialize the updated JSON object back to a string
    const char *updated_json_str = json_object_to_json_string_ext(root_obj, JSON_C_TO_STRING_PLAIN);
    char *result = strdup(updated_json_str);

    // Free the JSON objects
    json_object_put(root_obj);

    return result;
}

char const *rsmp_json_msg2payload(rsmp_messages_ptr msg)
{
    assert(msg.type != NULL);
    char const *ret=NULL;
    if (rsmp_message_is_active(msg))
    {
        rsmp_msg_desc_t const *msg_desc = rsmp_message_desc_get(msg);
        rsmp_field_desc_t const * fields = msg_desc->fields;
        if (fields)
        {
            //-- Création de l'objet json...
            json_object * jobj = json_object_new_object();
            if (jobj)
            {
                if (!json_add_fields(jobj, msg, fields))
                {
                    ret = json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PLAIN);
                    if (ret) ret = strdup(ret);
                    // todo en json-c, prend ret, déserialize et ajoute l'objet :
                     /* "eSXLG": [
{
"sg": "GEN", "vers": "1.0.0"
},{
"sg": "SLT", "vers": "0.5.2"
},{
"sg": "IO", "vers": "0.3.0"
},{
"sg": "VAR", "vers": "0.1.0"
}
]
a la racine. Enfin serialize vers ret */
                    // if (msg.type == _rsmp_msg_Version_)
                    if(ret && strstr(ret, "\"Version\"") != NULL){
                        char *updated_json = add_eSXLG_to_json(ret); // \todo grosse verrue à revoir
                        if (updated_json)
                        {
                            ret = strdup(updated_json);
                            printf("$$$$$$$$$$$$$$$$ Updated JSON (insert eSXLG): %s\n", updated_json);
                            // sleep(1);
                            fflush(stdout);
                            // sleep(1);
                            free(updated_json);
                        }
                    }
                }
                json_obj_free(jobj);
            }
        }
    }
    else
    {
        _rsmp_log_err(_rsmp_errcod_json_unknown_msg_, "message type %s is not active!", rsmp_type_to_string(*msg.type));
    }
    return ret;
}


char const *rsmp_json_msg2payload_with_inserting_jobj(rsmp_messages_ptr msg, char *key, json_object *jobj_to_insert)
{
    assert(msg.type != NULL);
    char const *ret=NULL;
    if (rsmp_message_is_active(msg))
    {
        rsmp_msg_desc_t const *msg_desc = rsmp_message_desc_get(msg);
        rsmp_field_desc_t const * fields = msg_desc->fields;
        if (fields)
        {
            //-- Création de l'objet json...
            json_object * jobj = json_object_new_object();
            if (jobj)
            {
                if (!json_add_fields(jobj, msg, fields))
                {
                    if(key && jobj_to_insert){
                        // on commence par faire un raz de l'élément si existant
                        json_object_object_del(jobj, key);

                        json_object_object_add(jobj, key, jobj_to_insert);

                    } else {
                        _log_err("cannot insert jobj element since key or jobj is null (key = %p, jobj = %p)", key, jobj_to_insert);
                    }
                    ret = json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PLAIN);
                    if (ret) ret = strdup(ret);
                }
                json_obj_free(jobj);
            }
        }
    }
    else
    {
        _rsmp_log_err(_rsmp_errcod_json_unknown_msg_, "message type %s is not active!", rsmp_type_to_string(*msg.type));
    }
    return ret;
}
/*

88888888ba,    88888888888  ,ad8888ba,    ,ad8888ba,    88888888ba,    88888888888  88888888ba
88      `"8b   88          d8"'    `"8b  d8"'    `"8b   88      `"8b   88           88      "8b
88        `8b  88         d8'           d8'        `8b  88        `8b  88           88      ,8P
88         88  88aaaaa    88            88          88  88         88  88aaaaa      88aaaaaa8P'
88         88  88"""""    88            88          88  88         88  88"""""      88""""88'
88         8P  88         Y8,           Y8,        ,8P  88         8P  88           88    `8b
88      .a8P   88          Y8a.    .a8P  Y8a.    .a8P   88      .a8P   88           88     `8b
88888888Y"'    88888888888  `"Y8888Y"'    `"Y8888Y"'    88888888Y"'    88888888888  88      `8b


*/

static int json_extract_fields(json_object *jobj, void * msg, rsmp_field_desc_t const *fields);




static inline int rsmp_json_object_get(json_object * jobj, char const * key, json_type *type, json_object **value)
{
    int ret = _rsmp_no_error_;
    if (json_object_object_get_ex(jobj, key, value))
    {
        *type = json_object_get_type(*value);
    }
    else
    {
        ret = _rsmp_errcod_json_notfound_;
        _rsmp_log_err(ret, ": %s", key);
        *value = NULL;
    }
    return ret;
}




/**
 * @brief 
 * 
 * @param jobj 
 * @param key 
 * @param tofval 
 * @param value 
 * @return bool 
 */
static bool rsmp_json_key_value_check(json_object * jobj, char const * key, enum json_type tofval, json_object **value )
{
    json_type type_of_object;
    int ret = false;
    if (!rsmp_json_object_get(jobj, key, &type_of_object, value))
    {
        //-- Check le type, extraction de la valeur, test du truc...
        ret = (type_of_object == tofval);
    }
    return ret;
}


static bool rsmp_json_mType_check(json_object * jobj)
{
    static char const *key=NULL;
    json_object * value;
    bool ret = false;
    if (!key) key = _rsmp_field_lib_get(_rsmp_field_mType_);
    if (rsmp_json_key_value_check(jobj, key, json_type_string, &value))
    {
        ret = rsmp_mType_check(json_object_get_string(value));
    }
    return ret;
}

static type_t rsmp_json_extract_type(json_object * jobj, char **str)
{
    static char const *type_lib=NULL;
    if (!type_lib) type_lib = _rsmp_field_lib_get(_rsmp_field_type_);
    type_t ret = _rsmp_msg_no_;
    json_object * value;

    if (rsmp_json_key_value_check(jobj, type_lib, json_type_string, &value ))
    {
        //-- Récupération de la valeur pour décodage...
        *str = (char *)json_object_get_string(value);
        ret = rsmp_type_from_string(*str);
    }
    return ret;
}



static int json2vars(json_type type, json_object *obj, rsmp_var_t **var)
{
    int ret=_rsmp_no_error_;

    switch (type)
    {
	case json_type_null :
        *var = rsmp_var_from_null();
        break;

	case json_type_boolean :
        *var = rsmp_var_from_bool(json_object_get_boolean(obj) != 0);
        break;

	case json_type_double :
        *var = rsmp_var_from_double(json_object_get_double(obj));
        break;

	case json_type_int :
        *var = rsmp_var_from_int(json_object_get_int64(obj));
        break;

	case json_type_object :
	case json_type_array :
        ret=_rsmp_errcod_json_decode_;
        break;

	case json_type_string :
        *var = rsmp_var_from_string_len(json_object_get_string(obj), json_object_get_string_len(obj));
        break;
    }
    return ret;
}

int json_extract_field(json_type jtype, json_object *obj, rsmp_field_decode_t decode, void *msg)
{
    int ret = _rsmp_no_error_;
    rsmp_var_t *var=NULL;
    json2vars(jtype, obj, &var);
    if (var)
    {
        ret = decode(var, msg);
        rsmp_var_free(var);
    }
    else
    {
        ret = _rsmp_errcod_json_decode_;
    }
    return ret;
}



static inline int json_extract_array(json_object *jobj, void *msg, rsmp_field_desc_t const *field)
{
    int ret = _rsmp_no_error_;
    rsmp_array_void_t *array = msg ;
    //-- Décodage d'un tableau d'object (pas un type simple)
    //-- Création des éléments du tableau d destination
    int len = json_object_array_length(jobj);

    rsmp_array_initialize(array, len);

    //-- Parcours de l'objet jobj
    //-- Pour chaque élément déclenchement de l'extractions des champs...
    void *item=array->items;
    for(size_t i=0; i<array->len; i++, item+=array->item_len)
    {   
        json_object *value = json_object_array_get_idx(jobj, i);
        ret = json_extract_fields(value, item, field->ext.array);
        if (ret) {
            _rsmp_log_err(ret, "fail decoding %s as an array", field->lib);            
            break;
        }
    }
    return ret;
}

static inline int json_extract_table(json_object *jobj, void * msg, rsmp_field_desc_t const *field)
{
   int ret = _rsmp_no_error_;
    rsmp_array_void_t *array = msg ;
    //-- Décodage d'un tableau d'object (pas un type simple)
    //-- Création des éléments du tableau d destination
    int len = json_object_array_length(jobj);

    rsmp_array_initialize(array, len);

    //-- Parcours de l'objet jobj
    //-- Pour chaque élément déclenchement de l'extractions des champs...
    void *item=array->items;
    for(size_t i=0; i<array->len; i++, item+=array->item_len)
    {   
        json_object *value = json_object_array_get_idx(jobj, i);
        ret = json_extract_field(json_object_get_type(value), value, field->ext.convert.decode, item);
        if (ret) {
            _rsmp_log_err(ret, "fail decoding %s as a table", field->lib);
            break;
        }
    }
    return ret;
}


static int json_extract_fields(json_object *jobj, void * msg, rsmp_field_desc_t const *fields)
{
    int ret = _rsmp_no_error_;
    uint i=0;
    while(fields[i].type != _rsmp_field_desc_end_mark_)
    {
        json_object *obj=NULL;
        json_type jtype;
        rsmp_json_object_get(jobj, fields[i].lib, &jtype, &obj);       
        //-- Vérification cohérence type 
        if (obj)
        {
            if ((jtype == json_type_array || jtype == json_type_object) && fields[i].type == _rsmp_field_desc_field_mark_)
            {
                ret = _rsmp_errcod_json_decode_;
                _rsmp_log_err(ret, "try to decode an json %s into a fields variable %s", json_type_to_name(jtype), fields[i].lib);
            }
            else
            {
                // fprintf(stdout, "decoding a json %s into a fields variable %s\n", json_type_to_name(jtype), fields[i].lib);
                _log_info("decoding a json %s into a fields variable %s\n", json_type_to_name(jtype), fields[i].lib);
                
                //-- Récup de l'objet en fonction du champs
                switch(fields[i].type)
                {
                    case _rsmp_field_desc_field_mark_ :
                        ret = json_extract_field(jtype, obj, fields[i].ext.convert.decode, msg+fields[i].offset);
                        break;
                    case _rsmp_field_desc_array_mark_ :
                        ret = json_extract_array(obj, msg+fields[i].offset, fields + i);
                        break;
                    case _rsmp_field_desc_table_mark_ :
                        ret = json_extract_table(obj, msg+fields[i].offset, fields + i);
                        break;
                    default:
                        break;
                }
            }
        }
        else
        {
            ret = _rsmp_errcod_json_decode_;
            _rsmp_log_err(ret, "no json object for field: %s", fields[i].lib);
        }
        if (ret) break;
        i++; //-- On passe au suivant...
    }

    return ret;
}



rsmp_messages_ptr rsmp_json_payload2msg(rsmp_context_t const *context, char const *payload)
{
    assert(payload != NULL);
    assert(context != NULL);
    void *msg=NULL;
    //-- Transformation de la chaîne en object json
    json_object * jobj = json_tokener_parse(payload);
    if (jobj)
    {
        type_t type_of_message;
        //-- Vérification de la présence du type et récupération pour la suite...
        if (rsmp_json_mType_check(jobj))
        {
            char *str_msg=NULL;
            if ((type_of_message = rsmp_json_extract_type(jobj, &str_msg)) != _rsmp_msg_no_)
            {
                //-- Récupération de la description du message
                rsmp_msg_desc_t const *msg_desc = rsmp_message_desc_get(&type_of_message);
                rsmp_field_desc_t const * fields = msg_desc->fields;
                //-- Deux solutions : on cherche les champs attendus, ou on vérifie que les champs 
                //-- du json sont bien attendus...
                //-- Pour faire "vite" on gere que les champs attendu...

                //-- Création du conteneur du message
                msg = msg_desc->_new(context);
                if (msg)
                {
                    //-- Parcours des champs pour les decoder
                    if (json_extract_fields(jobj, (type_t *)msg, fields)!=0)
                    {
                        //-- Echec...
                        msg_desc->_free(msg);
                        _rsmp_log_err(_rsmp_errcod_json_decode_, "message: %s, payoad: '%s'", str_msg, payload);
                    }
                }
                else
                {
                    _rsmp_log_err(_rsmp_errcod_json_generic_, "allocation message %s failure", str_msg);
                }
            }
            else
            {
                _rsmp_log_err(_rsmp_errcod_json_unknown_msg_, "msg type:%s, payload: '%s'", str_msg, payload);
            }
        }
        else
        {
            _rsmp_log_err(_rsmp_errcod_json_no_header_, "mType failure, payload: '%s'", payload);
        }
        json_obj_free(jobj);
    }
    else
    {
        _rsmp_log_err(_rsmp_errcod_json_parse_, "payload: '%s'", payload);
    }
    return (rsmp_messages_ptr)(type_t *)msg;
}

/**@}*/
