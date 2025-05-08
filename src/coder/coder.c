/**
* @file coder.c
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
#include <stdbool.h>
#include <assert.h>
#include <json-c/json.h>

#include "auth/auth-db-to.h"
#include "auth/auth-db-from.h"
#include "auth/auth-pwd.h"
#include "cipher/cipher.h"
#include "crypt/bcd.h"
#include "errcode/errcode.h"
#include "horodate/horodate.h"
#include "log/log.h"

#include "memory/checkmem.h"

#define RSMP_CODER_CODER_C
#include "coder.h"

/**
 * @ingroup coder
 * @{
 */

static int _coder_time_windows=-1; //milisecondes 
void coder_time_windows_set(int val)
{
    _coder_time_windows = val;
}



int ptxt_is_valid(ptxt_t *ptxt) {

    if(!ptxt || !ptxt->original || (ptxt->class != _cipher_class_internal_ && ptxt->class != _cipher_class_external_)) {
        return _rsmp_errcod_json_decode_;
    }

    //-- Vérification du timestamp
    HoroDate now;
    int delta = abs((int)diffHorodateMs(&ptxt->timestamp, setHorodate(&now)));
    if (_coder_time_windows > 0 && delta > _coder_time_windows) // paramètre actif et temps incorrect
    {
        _log_err("controle temps a echoué %dms", delta);
        return _rsmp_errcod_auth_outdated_;
    }

    //-- Recherche d'un marqueur canari sur le paylaod
    if (strstr(ptxt->original, "\"rSMsg\"") == NULL && strstr(ptxt->original, "\"eRMsg\"") == NULL)
    {
        _log_err("No canari rSMsg dans '%s'", ptxt->original);
        return _rsmp_errcod_cipher_decrypt_;
    }

    return _rsmp_errcod_gen_no_error_;
}


static int _coder_iterations = 1000;

static inline int coder_auth_get(const auth_login_t login, const auth_sId_t sId_remote, coder_enum_action_t action, auth_t **auth)
{
    assert(sId_remote);
    assert(auth);
    
    if (action ==_coder_decrypt_ && !login) return _rsmp_errcod_auth_failure_;

    int ret = _rsmp_no_error_;
    auth_key_t key = {0};
    if (login)
        *auth = (auth_t *)auth_find_by_key((action==_coder_decrypt_) ? auth_from_key_set(&key, login, sId_remote):auth_to_key_set(&key, login, sId_remote));
    else
        *auth = auth_get_first((action==_coder_decrypt_) ? auth_from_key_set(&key, login, sId_remote):auth_to_key_set(&key, login, sId_remote));
    if (!*auth)
    {
        ret = _rsmp_errcod_auth_unknown_;
        _rsmp_log_err(ret, "%s:%s:%s", key.dbId, key.login, key.sId);
    }
    else
    {
        _log_debug("Get Auth %s:%s:%s", key.dbId, (*auth)->key.login, key.sId);
    }
    
    if(*auth && !(*auth)->active){
        _log_err("auth=%p auth->active=%d", *auth, (*auth)->active);
        return _rsmp_errcod_cyber_generic_;
    } 

    return ret;
}



#define _ptxt_field_original_ "original"
#define _ptxt_field_class_ "class"
#define _ptxt_field_timestamp_ "timestamp"


static inline ptxt_t *ptxt_release(ptxt_t *ptxt)
{
    if (ptxt && ptxt->original)
    {
        printf("ptxt_release: %s\n", ptxt->original);
        fflush(stdout);
         _free(ptxt->original);
    }
    return ptxt;
}

int ptxt_set(ptxt_t *ptxt, HoroDate const *timestamp, char const *original)
{
    int ret = _rsmp_errcod_gen_no_error_;
    ptxt_release(ptxt);
    if (timestamp) ptxt->timestamp = *timestamp;
    else setHorodate(&ptxt->timestamp);
    
    if (ptxt->original)
    {
        free(ptxt->original);
        ptxt->original = NULL;
    }
    if (original)
    {
        ptxt->original = strdup(original);
        if (strstr(original,"\"rSMsg\"")!=NULL){
            ptxt->class = _cipher_class_external_;
        } 
        else if (strstr(original,"\"eRMsg\"")!=NULL)
        {
            ptxt->class = _cipher_class_internal_;
        } else {
            _log_err("mType canari values rSMsg not found. Message payload dropped.");
            free(ptxt->original);
            ptxt->original = NULL;

            ret = _rsmp_errcod_json_decode_;
        }
    }

    return ret;
}

static inline int ptxt_json_extract_field(json_object *jobj, const char *key, char const **str)
{
    int ret = _rsmp_errcod_json_parse_;
    json_object * item;
    if (json_object_object_get_ex(jobj, key, &item))
    {
        //-- test du type
        if (json_object_is_type(item, json_type_string))
        {
            *str = json_object_get_string(item);
            ret = _rsmp_no_error_;
        }
    }
    return ret;
}

#define TIMESTAMP_FORMAT_DEF "%Y-%m-%dT%H:%M:%S.#mlZ"
;

static HoroDate *str2ZuluHorodate(char const *str, char const *format, HoroDate *horodate)
{
    assert(horodate != NULL);
    char *sbis = NULL;
    char *sstr = (char *)str;
    char *dot = NULL;
    unsigned long ml = 0;
    
    if (!format) format="%Y-%m-%dT%H:%M:%SZ";
    if (strstr(format,".#ml") != NULL)
    {
        sstr = sbis = strdup(str);
        dot = strrchr(sbis, '.');
        if (dot)
        {
            errno=0;
            char*next;
            ml = strtoul(dot+1, &next, 10);
            ml *= 1000000; //-- Passage en nanoseconde
            if (errno) 
            {
                //Failure...
                ml = 0;
            }
            else 
            {
                strcpy(dot, next);
            }
        }
    }
    if (sstr)
    {
        struct tm tm_info;
        strptime(str, format, &tm_info);
        tm_info.tm_isdst = -1;
        setSecond(horodate, mktime(&tm_info));
        setNanoSecond(horodate, ml);
        localtime_r(&horodate->tv_sec, &tm_info);
    	horodate->tv_sec += tm_info.tm_gmtoff;
    }

    if (sbis) free(sbis);

    return horodate;
}


static inline int ptxt_extract_timestamp(json_object *jobj, HoroDate *ts)
{
    const char *str = NULL;
    int ret = ptxt_json_extract_field(jobj, _ptxt_field_timestamp_, &str);
    if (_rsmp_err_is_ok(ret))
    {
            assert(str != NULL);
            assert(ts != NULL);
            str2ZuluHorodate(str, TIMESTAMP_FORMAT_DEF, ts);
            
            if (ts->tv_sec <= 0)
            {
                ret = _rsmp_errcod_json_parse_;
                _rsmp_log_err(ret, "####### timestamp:%s", *ts);
        }
    } else {
        _log_err("timestamp not found");
    }
    return ret;
}

static inline int str_set(char **item, char const *value)
{
    assert(item != NULL);
    if (*item)
    {
        free(*item);
        *item = NULL;
    }
    if (value)
    {
        *item = strdup(value);
    }
    return _rsmp_no_error_;
}

static inline int ptxt_extract_original(json_object *jobj, char** original)
{
    const char *str = NULL;
    int ret = ptxt_json_extract_field(jobj, _ptxt_field_original_, &str);
    if (_rsmp_err_is_ok(ret))
    {
        asprintf(original, "%s", str);
    }
    return ret;
}

static inline int ptxt_extract_class(json_object *jobj, pt_enum_class_t *class)
{
    const char *str = NULL;
    int ret = ptxt_json_extract_field(jobj, _ptxt_field_class_, &str);
    if (_rsmp_err_is_ok(ret))
    {
        *class = _cipher_class_unknown_;
        if(strcmp(str, "INTERNAL")==0) {
            *class = _cipher_class_internal_;
        } else if(strcmp(str, "EXTERNAL")==0) {
            *class = _cipher_class_external_;
        } else ret = _rsmp_errcod_json_parse_;
        // pas de free à faire sur str. sera libérée avec l'objet json. 
    }
    return ret;
}

int ptxt_deserial(char *payload, ptxt_t *ptxt)
{
    assert(ptxt);
    assert(payload);
    int ret = _rsmp_no_error_;
    json_object * jobj = json_tokener_parse(payload);
    if (!jobj) {
        ret = _rsmp_errcod_json_decode_;
        return ret;
    }

    // _log_dump_info_in(payload, strlen(payload), "payload parsed");

    //-- Recherche des champs
    ret = ptxt_extract_timestamp(jobj, &ptxt->timestamp);
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("extract timestamp failed");
        json_object_put(jobj);
        return ret;
    }

    ret = ptxt_extract_class(jobj, &ptxt->class);
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("extract class failed");
        json_object_put(jobj);
        return ret;
    }

    ret = ptxt_extract_original(jobj, &ptxt->original);
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("extract original failed");
        json_object_put(jobj);
        return ret;
    }

    json_object_put(jobj);
    return ret;
}


char *_rsmp_ts_to_string(HoroDate *ts, char *str, size_t len)  
{
    assert(ts != NULL);
    if (!str)
    {   
        len = 50;
        str=calloc(len, sizeof(*str));
        if (!str)
        {
            _log_err("calloc failed");
            return NULL;
        }
    }
    return strGmtHorodate(TIMESTAMP_FORMAT_DEF, 0, str, len, ts);
}  

int ptxt_serialize(ptxt_t *ptxt, char **out)
{
    int ret = _rsmp_no_error_;
    json_object * jobj = json_object_new_object();
    if (jobj)
    { 
        char dt[50];
        json_object_object_add(jobj, _ptxt_field_timestamp_, json_object_new_string(_rsmp_ts_to_string(&ptxt->timestamp, dt, sizeof(dt))));
        json_object_object_add(jobj, _ptxt_field_original_, json_object_new_string(ptxt->original));
        if(ptxt->class == _cipher_class_internal_) {
            json_object_object_add(jobj, _ptxt_field_class_, json_object_new_string("INTERNAL"));
        } else if(ptxt->class == _cipher_class_external_) {
            json_object_object_add(jobj, _ptxt_field_class_, json_object_new_string("EXTERNAL"));
        } else {
            return _rsmp_errcod_json_encode_;
        }

        //-- Sérialisation
        *out = strdup(json_object_to_json_string(jobj));
        if (!*out)
        {
            ret = _rsmp_errcod_json_encode_;
        }

        json_object_put(jobj);
    }
    else
    {
        ret = _rsmp_errcod_json_encode_;
    }
    return ret;
}


static char* _getPassphraseForPwd(const char *sId, const auth_t *auth, int action, size_t *length, auth_pwd_t *pwd) {
    size_t len[3] = {strlen(sId), strlen(auth->key.login), strlen(auth->key.sId)};
    *length = len[0] + len[1] + len[2] + pwd->len;

#ifdef TEST_ENABLED
    _log_info("############################### sId local(%d): %s, login(%d): %s, pwd (%d), sId remote(%d): %s, action: %s", len[0], sId, len[1], auth->key.login, pwd->len, len[2], auth->key.sId, (action == _coder_decrypt_) ? "decrypt" : "encrypt");
#endif

    char *data = calloc(*length + 1, sizeof(*data));
    if (data) {
        char *d = data;
        if (action == _coder_decrypt_) {
            memcpy(d, auth->key.sId, len[2]);
            d += len[2];
            memcpy(d, auth->key.login, len[1]);
            d += len[1];
            memcpy(d, pwd->buf, pwd->len);
            d += pwd->len;
            memcpy(d, sId, len[0]);
        } else {
            memcpy(d, sId, len[0]);
            d += len[0];
            memcpy(d, auth->key.login, len[1]);
            d += len[1];
            memcpy(d, pwd->buf, pwd->len);
            d += pwd->len;
            memcpy(d, auth->key.sId, len[2]);
        }
    }
    return data;
}

static char* _getCurrentPassphrase(const char *sId, const auth_t *auth, int action, size_t *length) {
    return _getPassphraseForPwd(sId, auth, action, length, &auth->pwd);
}


static char* _getNewPassphrase(const char *sId, const auth_t *auth, int action, size_t *length) {
    return _getPassphraseForPwd(sId, auth, action, length, &auth->new_pwd);
}


static  _computeNewSecretAuthTo(const cipher_t *cipher, const char *sId_local, auth_t *auth, auth_secret_t *secret_new_pathphrase)
{
    size_t length = 0;
    char *data = _getNewPassphrase(sId_local, auth, _coder_encrypt_, &length);
    int ret = cipher->key_gen(data, length, _coder_iterations, &secret_new_pathphrase->buf, &secret_new_pathphrase->len);
    if(data) free(data);
    if(!_rsmp_err_is_ok(ret)) {
        _log_err("Failed to generate new secret");
    }
    return ret;
}


static inline int coder_secret_get(cipher_t const *cipher, auth_t *auth, const char * sId, coder_enum_action_t action, auth_secret_t **secret)
{
    int ret = _rsmp_no_error_;
    assert(secret);
    *secret = (auth_secret_t *)auth_secret_get(auth, cipher->lib);
    if (!*secret)
    {
        //-- Création de l'origine de la dérivation
        size_t length = 0;
        char *data = NULL;

        data = _getCurrentPassphrase(sId, auth, action, &length);
        if (data)
        {

#ifdef TEST_ENABLED
            char *data_str = bcd_encode(data, length);
            _log_info("data: %s, data.len: %u, data.buf:%s", data, length, data_str);
            if (data_str) free(data_str);
#endif
            //-- Dérivation de la clé
            auth_secret_t new_secret;
            if (cipher->key_gen(data, length, _coder_iterations, &new_secret.buf, &new_secret.len)==0)
            {
                *secret = (auth_secret_t *)auth_secret_add_or_update(auth, cipher->lib, &new_secret);
                if (*secret)
                {
#ifdef TEST_ENABLED
                    char *key_str = bcd_encode((*secret)->buf, (*secret)->len);
                    _log_info("!!!!!!!!!!!!!! secret.len: %u, secret.buf:%s", (*secret)->len, key_str);
                    if (key_str) free(key_str);
#endif
                }
                else
                {
                    ret = _rsmp_errcod_auth_no_secret_;
                }
            }
            else
            {
                ret = _rsmp_errcod_cipher_keygen_;
            }
        }
    }
    return ret;
}


static int coder_cipher_decrypt(cipher_t const *cipher, auth_secret_t const *secret, char *in, char **out)
{
    assert(cipher);
    assert(secret);
    assert(out);
    void *context;
    int ret = _rsmp_no_error_;
    if (cipher->context_new(&context)==0)
    {
        size_t out_len=0;
        // _log_dump_info_in(secret->buf, secret->len, "!!!!!!!!!!!!!!!!!!!!!! secret ");
        int ret_cipher = cipher->decrypt(context, in, secret->buf, secret->len, out, &out_len);
        if (ret_cipher==0)
        {
            *out = realloc(*out, out_len+1);
            if (*out) (*out)[out_len]='\0';
            ret = _rsmp_no_error_;
        }
        else
        {
            ret=_rsmp_errcod_cipher_decrypt_;
            // _rsmp_log_err(ret, "%s decrypt failed %d", cipher->lib, ret_cipher);
        }
        cipher->context_free(&context);
    }
    else
    {
        _rsmp_log_err(ret, "context new failed");
    }
    return ret;
}


int coder_decrypt(cipher_t const *cipher, const auth_sId_t sId_local, const auth_login_t login, const auth_sId_t sId_remote, char *in, char **out, auth_t **auth)
{
    int ret = _rsmp_no_error_;
    *auth = NULL;
    ret = coder_auth_get(login, sId_remote, _coder_decrypt_, auth);
    if (_rsmp_err_is_ok(ret))
    {
        //-- Récupération du secret
        auth_secret_t *secret = NULL;
        ret = coder_secret_get(cipher, *auth, sId_local, _coder_decrypt_, &secret);
        if (_rsmp_err_is_ok(ret))
        {
            char *payload;

            //-- décodage du contenu
            ret = coder_cipher_decrypt(cipher, secret, in, &payload);
            if (_rsmp_err_is_ok(ret))
            {
                // _log_dump_info_in(payload, strlen(payload), " ");
                char *log_message = NULL;

                // Utiliser asprintf pour construire la chaîne de caractères avec le login et le payload
                asprintf(&log_message, "Login: %s, Payload: %s", login, payload);

                // _log_dump_info_in(log_message, strlen(log_message), " "); 

                // Libérer la mémoire allouée
                free(log_message);

                *out = payload;

                return ret;

            }
            // else
            // {
            //     _rsmp_log_err(ret, "%s", cipher->lib);
            // }
        }
        else
        {
            _rsmp_log_err(ret, " #########");
        }
    }
    else
    {
        _rsmp_log_err(ret, " #########");
    }
    return ret;
}


static int coder_cipher_encrypt(cipher_t const *cipher, auth_secret_t const *secret, char const *in, char **out)
{
    assert(cipher);
    assert(secret);
    assert(out);
    void *context;
    int ret = _rsmp_errcod_cipher_encrypt_;
    if (cipher->context_new(&context)==0)
    {
        // _log_dump_info_out(secret->buf, secret->len, "!!!!!!!!!!!!!!!!!!!!!! secret ");
        if (cipher->encrypt(context, in, strlen(in), secret->buf, secret->len, out)==0)
        {
            ret = _rsmp_no_error_;
        }
        cipher->context_free(&context);
    }
    else
    {
        _rsmp_log_err(ret, "context new failed");
    }
    return ret;
}

int coder_encrypt(cipher_t const *cipher, const auth_sId_t sId_local, const auth_login_t login, const auth_sId_t sId_remote, char const *in, char **out, auth_t **auth)
{
    int ret = _rsmp_no_error_;
    ret = coder_auth_get(login, sId_remote, _coder_encrypt_, auth); 
    if (_rsmp_err_is_ok(ret))
    {
        //-- On récupère le secret
         //-- Récupération du secret
        auth_secret_t *secret = NULL;
        ret = coder_secret_get(cipher, *auth, sId_local, _coder_encrypt_, &secret);
        if (_rsmp_err_is_ok(ret))
        {
            ret = coder_cipher_encrypt(cipher, secret, in, out);
             _log_dump_info_out(in, strlen(in), " encoded %s with login %s", cipher->lib, login);
        }
   }
    return ret;
}


/*
88888888888  888b      88    ,ad8888ba,    ,ad8888ba,    88888888ba,    88888888888
88           8888b     88   d8"'    `"8b  d8"'    `"8b   88      `"8b   88
88           88 `8b    88  d8'           d8'        `8b  88        `8b  88
88aaaaa      88  `8b   88  88            88          88  88         88  88aaaaa
88"""""      88   `8b  88  88            88          88  88         88  88"""""
88           88    `8b 88  Y8,           Y8,        ,8P  88         8P  88
88           88     `8888   Y8a.    .a8P  Y8a.    .a8P   88      .a8P   88
88888888888  88      `888    `"Y8888Y"'    `"Y8888Y"'    88888888Y"'    88888888888
*/


// Fonction pour construire un message Encrypted
int Encrypted_encode(cipher_t const *cipher, char *sId_local, const char *login, const char *sId, ptxt_t *ptxt_list, size_t ptxt_count, char ** payload) {
    int ret = _rsmp_no_error_;

    if (!cipher || !sId_local || !sId || !ptxt_list || ptxt_count == 0) {
        return _rsmp_errcod_json_parse_;
    }

    struct json_object *json_obj = json_object_new_object();
    struct json_object *json_data_array = json_object_new_array();

    json_object_object_add(json_obj, "mType", json_object_new_string("eRMsg"));
    json_object_object_add(json_obj, "type", json_object_new_string("Encrypted"));

    char uuid_str[UUID_STR_LEN];
    uuid_t _uuid;
    uuid_generate(_uuid);
    uuid_unparse(_uuid, uuid_str);
    json_object_object_add(json_obj, "mId", json_object_new_string(uuid_str));
    json_object_object_add(json_obj, "sId", json_object_new_string(sId_local));
    json_object_object_add(json_obj, "data", json_data_array);

    

    auth_t *auth;
    for (size_t i = 0; i < ptxt_count; ++i) {
        char *encrypted = NULL;
        char *data_to_crypt = NULL;
        
        ret = ptxt_serialize(&ptxt_list[i], &data_to_crypt);

        free(ptxt_list[i].original); // \todo a voir a quel niveau on libere, La libération de ptxt_list doit elle se faire par l'appelant ?

        if (!_rsmp_err_is_ok(ret))
        {
            _log_err("Erreur de serialisation pour l'élément data %d", i);
            break; // \todo comportement en cas d'erreur ? 

        }
        ret = coder_encrypt(cipher, sId_local, login, sId, data_to_crypt, &encrypted, &auth);

        if (!_rsmp_err_is_ok(ret) || !encrypted) {
            _log_err("Erreur de chiffrement pour l'élément %d", i);
            break; // \todo comportement en cas d'erreur ? 
        }

        struct json_object *json_encrypted_data = json_object_new_string(encrypted);
        json_object_array_add(json_data_array, json_encrypted_data);

        free(encrypted);
    }

    if (_rsmp_err_is_ok(ret))
    {
        json_object_object_add(json_obj, "login", json_object_new_string(auth->key.login));

        const char *json_str = json_object_to_json_string(json_obj);
        *payload = strdup(json_str);
    }
    json_object_put(json_obj);

    return ret;
}

// Fonction pour construire un message CipherAgreement
int CipherAgreement_serial(const char* sId_local, char ** ciphers_list, size_t ciphers_len, char ** payload, size_t *len, char ** mId) { // \todo remettre du const pour const char * const * ciphers_list
    int ret = _rsmp_no_error_;

    // if (!cipher || !sId_local || !sId || !ptxt_list || ptxt_count == 0) {
    if(!sId_local || !ciphers_list || ciphers_len <= 0 || !payload || !len){
        _log_err("Invalid input parameters %p %p %zu %p %p", sId_local, ciphers_list, ciphers_len, payload, len);
        return _rsmp_errcod_json_encode_;
    }

    struct json_object *json_obj = json_object_new_object();
    struct json_object *json_cipher_array = json_object_new_array();

    json_object_object_add(json_obj, "mType", json_object_new_string("eRMsg"));
    json_object_object_add(json_obj, "type", json_object_new_string("CipherAgreement"));

    char uuid_str[UUID_STR_LEN];
    uuid_t _uuid;
    uuid_generate(_uuid);
    uuid_unparse(_uuid, uuid_str);
    json_object_object_add(json_obj, "mId", json_object_new_string(uuid_str));
    json_object_object_add(json_obj, "sId", json_object_new_string(sId_local));
    json_object_object_add(json_obj, "cipher", json_cipher_array);
    if(mId){
        *mId = strdup(uuid_str); 
    }

    
    for (size_t i = 0; i < ciphers_len; ++i) {
        struct json_object *json_cipher = json_object_new_string(ciphers_list[i]);
        json_object_array_add(json_cipher_array, json_cipher);
    }

    const char *json_str = json_object_to_json_string(json_obj);
    if (json_str == NULL) {
        _log_err("Erreur serialisation");
        return _rsmp_errcod_json_encode_;
    }

    *payload = strdup(json_str);
    if (*payload == NULL) {
        _log_err("Erreur strdup");
        return _rsmp_errcod_json_encode_;
    }

    *len = strlen(json_str);

    json_object_put(json_obj);

    return ret;
}



/** 
 * @brief Encodage d'un message MessageAck
 * 
 * @param mId [in] L'ID du message
 * @param payload [out] Le payload encodé
 * @param len [out] La longueur du payload
 * @return int _rsmp_no_error_ si tout va bien, enum errcod sinon
 */
int MessageAck_serial(const char *mId, char ** payload, size_t *len) {
    int ret = _rsmp_no_error_;

    if (!mId || !payload || !len) {
        return _rsmp_errcod_json_encode_;
    }

    struct json_object *json_obj = json_object_new_object();

    json_object_object_add(json_obj, "mType", json_object_new_string("eRMsg"));
    json_object_object_add(json_obj, "type", json_object_new_string("MessageAck"));
    json_object_object_add(json_obj, "oMId", json_object_new_string(mId));

    const char *json_str = json_object_to_json_string(json_obj);
    if (json_str == NULL) {
        _log_err("Erreur serialisation");
        return _rsmp_errcod_json_encode_;
    }

    *payload = strdup(json_str);
    if (*payload == NULL) {
        _log_err("Erreur strdup");
        return _rsmp_errcod_json_encode_;
    }

    *len = strlen(json_str);

    json_object_put(json_obj);

    return ret;
}



void freeMessage(Message **msg) {
    return; // \todo a rétablir

    if(!msg || !*msg) {
        return;
    }
    switch ((*msg)->messageType) {
        case MSG_TYPE_ENCRYPTED:
            _free((*msg)->fields.encrypted.sId);
            _free((*msg)->fields.encrypted.login);
            for(size_t i = 0; i < (*msg)->fields.encrypted.data_len; i++) {
                _free((*msg)->fields.encrypted.data[i]);
            }
            _free((*msg)->fields.encrypted.data);
            break;
        case MSG_TYPE_ALLCLEAR:
            break;
        case MSG_TYPE_CHANGE_PWD_REQUEST:
            _free((*msg)->fields.changePwdRequest.pwd);
            break;
        case MSG_TYPE_CHANGE_PWD_RESPONSE:
            break;
        case MSG_TYPE_PING:
            _free((*msg)->fields.ping.padding);
            break;
        case MSG_TYPE_PONG:
            _free((*msg)->fields.pong.padding);
            break;
        default:
            break;
    }

    free(*msg);
    *msg = NULL;
}

Message * initMessage(MessageType type) {
    int ret = _rsmp_no_error_;
    Message *msg = (Message *)calloc(1, sizeof(Message));
    if(!msg) {
        _log_err("Erreur calloc");
        return NULL;
    }
    uuid_t _uuid;
    uuid_generate(_uuid);
    uuid_unparse(_uuid, msg->common.mId);
    msg->messageType = type;

    switch (type) {
        case MSG_TYPE_ENCRYPTED:
        case MSG_TYPE_ALLCLEAR:
        case MSG_TYPE_CHANGE_PWD_REQUEST:
        case MSG_TYPE_CHANGE_PWD_RESPONSE:
        case MSG_TYPE_PING:
        case MSG_TYPE_PONG:
            break;
        default:
            _log_err("Invalid message type");
            ret = _rsmp_errcod_gen_encode_;
            goto cleanup;
    }
    cleanup:
    if(!_rsmp_err_is_ok(ret)) {
        freeMessage(&msg);
        return NULL;
    }

    return msg;
}


// retourne 0 si les champs du message sont valides, -1 sinon
int messageCheckFields(Message *msg) {
    if(!msg) {
        _log_err("Invalid input parameters %p", msg);
        return -1;
    }
    if(strlen(msg->common.mId) != UUID_STR_LEN - 1) {
        _log_err("Invalid mId length");
        return -1;
    }

    switch (msg->messageType) {
        case MSG_TYPE_ENCRYPTED:
            if(!msg->fields.encrypted.sId || strlen(msg->fields.encrypted.sId) == 0 || !msg->fields.encrypted.data || msg->fields.encrypted.data_len == 0) {
                _log_err("Missing sId field");
                return -1;
            }
            for(size_t i = 0; i < msg->fields.encrypted.data_len; i++) {
                if(!msg->fields.encrypted.data[i] || strlen(msg->fields.encrypted.data[i]) == 0) {
                    _log_err("Missing data field");
                    return -1;
                }
            }
            break;
        case MSG_TYPE_ALLCLEAR:
            break;
        case MSG_TYPE_CHANGE_PWD_REQUEST:
            if(!msg->fields.changePwdRequest.pwd || strlen(msg->fields.changePwdRequest.pwd) == 0) {
                _log_err("Missing password field");
                return -1;
            }
            int base64_length = strlen(msg->fields.changePwdRequest.pwd);
            int original_length = (base64_length * 3) / 4 - (msg->fields.changePwdRequest.pwd[base64_length - 1] == '=' ? (msg->fields.changePwdRequest.pwd[base64_length - 2] == '=' ? 2 : 1) : 0);
            if(original_length < AUTH_PWD_LEN_MIN) {
                 _log_err("password received too short (len = %d)", original_length); // \todo ne faire que du log pour l'instant attente de spec
            } else if(original_length >= AUTH_PWD_LEN_MAX) {
                 _log_err("password received too long (len = %d)", original_length); 
                 return -1;
            }
            break;
        case MSG_TYPE_CHANGE_PWD_RESPONSE:
            if(!msg->fields.changePwdResponse.oMId || strlen(msg->fields.changePwdResponse.oMId) == 0) {
                _log_err("Missing oMId field");
                return -1;
            }
            break;
        case MSG_TYPE_PING:
            if(!msg->fields.ping.padding|| strlen(msg->fields.ping.padding) == 0) {
                _log_err("Missing padding field");
                return -1;
            }
            break;
        case MSG_TYPE_PONG:
            if(!msg->fields.pong.padding|| strlen(msg->fields.pong.padding) == 0) {
                _log_err("Missing padding field");
                return -1;
            }
            if(!msg->fields.pong.oMId || strlen(msg->fields.pong.oMId) == 0) {
                _log_err("Missing oMId field");
                return -1;
            }
            break;
        default:
            _log_err("Invalid message type");
            return -1;
    }
    return 0;
}


int serializeMessage(Message *msg, char **payload, size_t *len) {
    int ret = _rsmp_no_error_;
    if(!msg || !payload || !len || 0!=messageCheckFields(msg)) {
        _log_err("Invalid input parameters or field value %p %p %p", msg, payload, len);
        return _rsmp_errcod_json_encode_;
    }
    struct json_object *json_obj = json_object_new_object();
    
    json_object_object_add(json_obj, "mType", json_object_new_string("eRMsg"));
    json_object_object_add(json_obj, "mId", json_object_new_string(msg->common.mId));

    switch (msg->messageType) {
        case MSG_TYPE_ENCRYPTED:
            json_object_object_add(json_obj, "type", json_object_new_string("Encrypted"));
            json_object_object_add(json_obj, "sId", json_object_new_string(msg->fields.encrypted.sId));
            struct json_object *json_data_array = json_object_new_array();
            for(size_t i = 0; i < msg->fields.encrypted.data_len; i++) {
                struct json_object *json_data = json_object_new_string(msg->fields.encrypted.data[i]);
                json_object_array_add(json_data_array, json_data);
            }
            json_object_object_add(json_obj, "data", json_data_array);
            json_object_object_add(json_obj, "login", json_object_new_string(msg->fields.encrypted.login));

            break;
        case MSG_TYPE_ALLCLEAR:
            json_object_object_add(json_obj, "type", json_object_new_string("AllClear"));
            break;
        case MSG_TYPE_CHANGE_PWD_REQUEST:
            json_object_object_add(json_obj, "type", json_object_new_string("ChangePwdRequest"));
            json_object_object_add(json_obj, "pwd", json_object_new_string(msg->fields.changePwdRequest.pwd));
            break;
        case MSG_TYPE_CHANGE_PWD_RESPONSE:
            json_object_object_add(json_obj, "type", json_object_new_string("ChangePwdResponse"));
            json_object_object_add(json_obj, "oMId", json_object_new_string(msg->fields.changePwdResponse.oMId));
            json_object_object_add(json_obj, "status", json_object_new_boolean(msg->fields.changePwdResponse.status));
            break;
        case MSG_TYPE_PING:
            json_object_object_add(json_obj, "type", json_object_new_string("Ping"));
            json_object_object_add(json_obj, "pad", json_object_new_string(msg->fields.ping.padding));
            break;
        case MSG_TYPE_PONG:
            json_object_object_add(json_obj, "type", json_object_new_string("Pong"));
            json_object_object_add(json_obj, "pad", json_object_new_string(msg->fields.pong.padding));
            json_object_object_add(json_obj, "oMId", json_object_new_string(msg->fields.pong.oMId));
            break;
        default:
            _log_err("Invalid message type");
            return NULL;
    }

    const char *json_str = json_object_to_json_string(json_obj);
    if (json_str == NULL) {
        _log_err("Erreur serialisation");
        ret = _rsmp_errcod_json_encode_;
        goto cleanup;
    }

    *payload = strdup(json_str);
    if (*payload == NULL) {
        _log_err("Erreur strdup");
        ret = _rsmp_errcod_json_encode_;
        goto cleanup;
    }

    *len = strlen(json_str);
    
    cleanup:
        if(json_obj) 
            json_object_put(json_obj);
        if(!_rsmp_err_is_ok(ret)) {
            _free(*payload);
            *len = 0;
        }

    return ret;
}

struct json_object *parse_json_with_length(const char *str, int len) {
    struct json_tokener *tok = json_tokener_new();
    if (!tok) return NULL;

    struct json_object *obj = json_tokener_parse_ex(tok, str, len);
    enum json_tokener_error jerr = json_tokener_get_error(tok);

    if (jerr != json_tokener_success) {
        json_tokener_free(tok);
        return NULL;
    }

    json_tokener_free(tok);
    return obj;
}

int deserializeMessage(const char *payload, size_t len, Message **msg) 
{
    int ret = _rsmp_no_error_;
    if(!payload || !msg) {
        _log_err("Invalid input parameters %p %p", payload, msg);
        return _rsmp_errcod_json_decode_;
    }

    // struct json_object *json_obj = json_tokener_parse(payload);
    struct json_object *json_obj = parse_json_with_length(payload, len);
    if (!json_obj) {
        _log_err("Erreur json_tokener_parse");
        return _rsmp_errcod_json_decode_;
    }

    struct json_object *json_mType = NULL;
    struct json_object *json_mId = NULL;
    struct json_object *json_type = NULL;
    struct json_object *json_pwd = NULL;
    struct json_object *json_pad = NULL;
    struct json_object *json_oMId = NULL;
    struct json_object *json_status = NULL;

    Message *message = (Message *)calloc(1, sizeof(Message));
    if(!message) {
        _log_err("Erreur calloc");
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

    if(json_object_object_get_ex(json_obj, "mType", &json_mType) && json_object_is_type(json_mType, json_type_string)) {
        const char *mType = json_object_get_string(json_mType);
        if(strcmp(mType, "eRMsg") != 0) {
            _log_err("Invalid mType value");
            ret = _rsmp_errcod_json_decode_;
            goto cleanup;
        }
    } else {
        _log_err("mType not found");
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

    if(json_object_object_get_ex(json_obj, "mId", &json_mId) && json_object_is_type(json_mId, json_type_string)) {
        const char *mId = json_object_get_string(json_mId);
        if(strlen(mId) != UUID_STR_LEN - 1) {
            _log_err("Invalid mId length");
            ret = _rsmp_errcod_json_decode_;
            goto cleanup;
        }
        strcpy(message->common.mId, mId);
    } else {
        _log_err("mId not found");
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

    if(json_object_object_get_ex(json_obj, "type", &json_type) && json_object_is_type(json_type, json_type_string)) {
        const char *type = json_object_get_string(json_type);
        if(strcmp(type, "AllClear") == 0) {
            message->messageType = MSG_TYPE_ALLCLEAR;
        } else if(strcmp(type, "ChangePwdRequest") == 0) {
            message->messageType = MSG_TYPE_CHANGE_PWD_REQUEST;
        } else if(strcmp(type, "ChangePwdResponse") == 0) {
            message->messageType = MSG_TYPE_CHANGE_PWD_RESPONSE;
        } else if(strcmp(type, "Ping") == 0) {
            message->messageType = MSG_TYPE_PING;
        } else if(strcmp(type, "Pong") == 0) {
            message->messageType = MSG_TYPE_PONG;
        } else {
            _log_err("Invalid type value");
            ret = _rsmp_errcod_json_decode_;
            goto cleanup;
        }
    } else {
        _log_err("type not found");
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

    switch (message->messageType)
    {
    case MSG_TYPE_ALLCLEAR:
        break;
    case MSG_TYPE_CHANGE_PWD_REQUEST:
        if(json_object_object_get_ex(json_obj, "pwd", &json_pwd) && json_object_is_type(json_pwd, json_type_string)) {
            const char *pwd = json_object_get_string(json_pwd);
            message->fields.changePwdRequest.pwd = strdup(pwd);
        } else {
            _log_err("pwd not found");
            ret = _rsmp_errcod_json_decode_;
            goto cleanup;
        }
        break;
    case MSG_TYPE_CHANGE_PWD_RESPONSE:
        if(json_object_object_get_ex(json_obj, "oMId", &json_oMId) && json_object_is_type(json_oMId, json_type_string)) {
            const char *oMId = json_object_get_string(json_oMId);
            strcpy(message->fields.changePwdResponse.oMId, oMId);
        } else {
            _log_err("oMId not found");
            ret = _rsmp_errcod_json_decode_;
            goto cleanup;
        }
        if(json_object_object_get_ex(json_obj, "status", &json_status) && json_object_is_type(json_status, json_type_boolean)) {
            message->fields.changePwdResponse.status = json_object_get_boolean(json_status);
        } else {
            _log_err("status not found");
            ret = _rsmp_errcod_json_decode_;
            goto cleanup;
        }
        break;
    case MSG_TYPE_PING:
        if(json_object_object_get_ex(json_obj, "pad", &json_pad) && json_object_is_type(json_pad, json_type_string)) {
            const char *pad = json_object_get_string(json_pad);
            message->fields.ping.padding = strdup(pad);
        } else {
            _log_err("pad not found");
            ret = _rsmp_errcod_json_decode_;
            goto cleanup;
        }
        break;
    case MSG_TYPE_PONG:
        // parse de pad et oMId
        if(json_object_object_get_ex(json_obj, "pad", &json_pad) && json_object_is_type(json_pad, json_type_string)) {
            const char *pad = json_object_get_string(json_pad);
            message->fields.pong.padding = strdup(pad);
        } else {
            _log_err("pad not found");
            ret = _rsmp_errcod_json_decode_;
            goto cleanup;
        }
        if(json_object_object_get_ex(json_obj, "oMId", &json_oMId) && json_object_is_type(json_oMId, json_type_string)) {
            const char *oMId = json_object_get_string(json_oMId);
            strcpy(message->fields.pong.oMId, oMId);
        } else {
            _log_err("oMId not found");
            ret = _rsmp_errcod_json_decode_;
            goto cleanup;
        }
        break;
    default:
        ;
    }

    *msg = message;
    
    cleanup:
        if(json_obj) {
            json_object_put(json_obj);
        }
        if(!_rsmp_err_is_ok(ret) || messageCheckFields(message) != 0) {
            freeMessage(&message);
            *msg = NULL;
            return -1;

        } else {
            return 0;
        }
}

static int coder_cipher_encrypt(cipher_t const *cipher, auth_secret_t const *secret, char const *in, char **out);

// Déclaration de la nouvelle fonction
//const char *sId_local, const char *login, const char *sId
static int Challenge_encode_data(const HoroDate *timestamp,  Message * msg, const cipher_t *cipher, const auth_secret_t *secret, char **encryptedData, size_t *len_out) {

    if(!timestamp || !msg || !cipher || !secret || !encryptedData || !len_out) {
        _log_err("Invalid input parameters %p %p %p %p %p", timestamp, msg, cipher, secret, encryptedData);
        return _rsmp_errcod_gen_encode_;
    }

    int ret = _rsmp_no_error_;
    ptxt_t ptxt = {0};
    char *data_to_crypt = NULL;

    char * payload;
    size_t _len = 0;
    ret = serializeMessage(msg, &payload, &_len);
    if(!_rsmp_err_is_ok(ret)) {
        _log_err("Erreur serializeMessage");
        goto cleanup;
    }

    // Transformer le payload en ptxt_t
    ret = ptxt_set(&ptxt, timestamp, payload);
    if(ret != _rsmp_no_error_) {
        _log_err("Erreur ptxt_set");
        ret = _rsmp_errcod_json_encode_;
        goto cleanup;
    }

    // Serialisation du ptxt_t
    ret = ptxt_serialize(&ptxt, &data_to_crypt);
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("Erreur de serialisation pour l'élément data");
        ret = _rsmp_errcod_json_encode_;
        goto cleanup;
    }

    // Chiffrement du ptxt_t
    if (!_rsmp_err_is_ok(ret = coder_cipher_encrypt(cipher, secret, data_to_crypt, encryptedData)) || !(*encryptedData)) {
        _log_err("Erreur de chiffrement pour l'élément ChangePwdRequest");
        ret = _rsmp_errcod_json_encode_;
        goto cleanup;
    }
    *len_out = strlen(*encryptedData);

cleanup:
    _free(data_to_crypt);
    if (ret != _rsmp_no_error_) {
        _free(*encryptedData);
    }

    return ret;
}

// Fonction pour construire un message Encrypted avec un Challenge (ChangePwdRequest + Ping)
int Challenge_encode(ChallengeContext_t * ctxt, cipher_t const *cipher, char *sId_local, const char *login, const char *sId_remote, char ** payload, size_t *len) {
    int ret = _rsmp_no_error_;

    if (!cipher || !sId_local || !sId_remote || !login || !payload || !len || !ctxt) {
        _log_err("Invalid input parameters %p %p %p %p %p %p %p", cipher, sId_local, sId_remote, login, payload, len, ctxt);
        return _rsmp_errcod_gen_encode_;
    }

    if(ctxt->running) {
        _log_err("Challenge already running for login %s", ctxt->auth->key.login);
        return _rsmp_errcod_gen_encode_;
    }
    freeChallengeContextMembers(ctxt);

    ret = coder_auth_get(login, sId_remote, _coder_encrypt_, &ctxt->auth); 
    if (!_rsmp_err_is_ok(ret))
    {
        _log_err("Erreur coder_auth_get");
        goto cleanup;
    }
    
    char *pwd_b64 = NULL;
    ret = auth_get_new_pwd_b64(ctxt->auth, &pwd_b64);
    if(!_rsmp_err_is_ok(ret))
    {
        _log_err("Erreur auth_get_new_pwd_b64");
        goto cleanup;
    }

     
    // message ChangePwdRequest
    char pad_len = strlen(pwd_b64) + 12;
    char * ChangePwdRequest_encrypted_data = NULL;
    char * Ping_encrypted_data = NULL;
    auth_secret_t *ChangePwdRequest_secret_current = NULL;
    auth_secret_t Ping_secret_new = {0}; 

    //-- On récupère le secret courant
    ret = coder_secret_get(cipher, ctxt->auth, sId_local, _coder_encrypt_, &ChangePwdRequest_secret_current);
    if (!_rsmp_err_is_ok(ret))
    {
        _log_err("Erreur coder_secret_get ChangePwdRequest");
        goto cleanup;
    }

    ret = _computeNewSecretAuthTo(cipher, sId_local, ctxt->auth, &Ping_secret_new);
    if (ret != 0)
    {
        _log_err("Erreur computeNewSecretAuthTo Ping_secret_new");
        goto cleanup;
    }

    size_t _len = 0;
    
    Message *msg = NULL;
    msg = initMessage(MSG_TYPE_CHANGE_PWD_REQUEST);
    if(!msg) {
        _log_err("Erreur initMessage");
        goto cleanup;
    }
    
    assert(pwd_b64);
    assert(msg);
    msg->fields.changePwdRequest.pwd = strdup(pwd_b64);
    if(!msg->fields.changePwdRequest.pwd) {
        _log_err("Erreur strdup");
        goto cleanup;
    }
    ctxt->mIdChangePwdRequest = strdup(msg->common.mId);
    if(!ctxt->mIdChangePwdRequest) {
        _log_err("Erreur strdup");
        goto cleanup;
    }

    setHorodate(&ctxt->timestamp);

    ret = Challenge_encode_data(&ctxt->timestamp, msg, cipher, ChangePwdRequest_secret_current, &ChangePwdRequest_encrypted_data, &_len);
    if (ret != _rsmp_no_error_) {
        _log_err("Erreur Challenge_encode_data ChangePwdRequest");
        goto cleanup;
    }

    freeMessage(&msg);

    msg = initMessage(MSG_TYPE_PING);
    if(!msg) {
        _log_err("Erreur initMessage");
        goto cleanup;
    }
    ctxt->pad = calloc(pad_len + 1, sizeof(*ctxt->pad));
    if(!ctxt->pad) {
        _log_err("Erreur calloc");
        goto cleanup;
    }
    for (size_t i = 0; i < pad_len; i++) {
        ctxt->pad[i] = (char) (rand() % 94 + 33); // caractères imprimables
        // \todo remplacer rand par RAND_bytes
    }
    ctxt->pad[pad_len] = '\0';
    msg->fields.ping.padding = strdup(ctxt->pad);
    if(-1 == asprintf(&msg->fields.ping.padding, "%s", ctxt->pad))
    {
        _log_err("Erreur asprintf");
        goto cleanup;
    }

    ctxt->mIdPing = strdup(msg->common.mId);
    if(!ctxt->mIdPing) {
        _log_err("Erreur strdup");
        goto cleanup;
    }
    ret = Challenge_encode_data(&ctxt->timestamp, msg, cipher, &Ping_secret_new, &Ping_encrypted_data, &_len);
    if(ret != _rsmp_no_error_) {
        _log_err("Erreur Challenge_encode_data Ping");
        goto cleanup;
    }

    freeMessage(&msg);

    msg = initMessage(MSG_TYPE_ENCRYPTED);
    msg->fields.encrypted.sId = strdup(sId_local);
    msg->fields.encrypted.data = calloc(2, sizeof(*msg->fields.encrypted.data));
    if(!msg->fields.encrypted.data) {
        _log_err("Erreur calloc");
        goto cleanup;
    }
    msg->fields.encrypted.data_len = 2;

    // int random = rand() % 2; // \todo remplacer rand par RAND_bytes
    int random = 0; // \todo potentielle evolution de la spec rendre aléatoire
    msg->fields.encrypted.data[random] = ChangePwdRequest_encrypted_data;
    msg->fields.encrypted.data[(random+1)%2] = Ping_encrypted_data;
    msg->fields.encrypted.login = strdup(ctxt->auth->key.login);

    ret = serializeMessage(msg, payload, len);
    if(!_rsmp_err_is_ok(ret)) {
        _log_err("Erreur serializeMessage");
        goto cleanup;
    }

    ctxt->running = true;

cleanup:
    freeMessage(&msg);
    if(_rsmp_err_is_ok(ret)) {
        *len = strlen(*payload);
    } else {
        *len = 0;
        _free(*payload);
    }

    return ret;
}



/*
'########::'########::'######:::'#######::'########::'########:
 ##.... ##: ##.....::'##... ##:'##.... ##: ##.... ##: ##.....::
 ##:::: ##: ##::::::: ##:::..:: ##:::: ##: ##:::: ##: ##:::::::
 ##:::: ##: ######::: ##::::::: ##:::: ##: ##:::: ##: ######:::
 ##:::: ##: ##...:::: ##::::::: ##:::: ##: ##:::: ##: ##...::::
 ##:::: ##: ##::::::: ##::: ##: ##:::: ##: ##:::: ##: ##:::::::
 ########:: ########:. ######::. #######:: ########:: ########:
........:::........:::......::::.......:::........:::........::*/



int CipherAgreement_deserial(const void *buf_in, const size_t length, char ***ciphers_list, size_t *ciphers_len, char **mId, char **sId_Remote_rxed) {
    int ret = _rsmp_errcod_gen_no_error_;
    struct json_object *parsed_json = NULL;
    *ciphers_len = 0;

    // Vérification des paramètres d'entrée
    // le sId_Remote n'est pas renseigné par avance sinon il faudrait vérifier sa cohérence avec celui reçu
    if (!buf_in || length == 0 || !ciphers_list || !ciphers_len || !mId || *mId || !sId_Remote_rxed) {
        _log_err("Invalid input parameters %p %zu %p %p %p %p", buf_in, length, ciphers_list, ciphers_len, mId, sId_Remote_rxed);
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    const char *buf = (const char *)buf_in;
    parsed_json = json_tokener_parse(buf);
    if (!parsed_json) {
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    struct json_object *mType;
    struct json_object *type;
    struct json_object *mId_json;
    struct json_object *sId_json;
    struct json_object *ciphers;
    
    if (!json_object_object_get_ex(parsed_json, "mType", &mType) ||
        !json_object_object_get_ex(parsed_json, "type", &type) ||
        !json_object_object_get_ex(parsed_json, "mId", &mId_json) ||
        !json_object_object_get_ex(parsed_json, "sId", &sId_json) ||
        !json_object_object_get_ex(parsed_json, "cipher", &ciphers)) {
        _log_err("Failed to get required JSON fields");
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    if (strcmp(json_object_get_string(mType), "eRMsg") != 0 ||
        strcmp(json_object_get_string(type), "CipherAgreement") != 0) {
        _log_err("JSON fields 'mType' or 'type' do not match expected values");
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    *mId = strdup(json_object_get_string(mId_json));
    if (!*mId) {
        _log_err("Failed to duplicate mId string");
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

    *sId_Remote_rxed = strdup(json_object_get_string(sId_json));
    if(!*sId_Remote_rxed) {
        _log_err("Failed to get sId_Remote from JSON");
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    *ciphers_len = json_object_array_length(ciphers);
    if (*ciphers_len == 0) {
        _log_err("Cipher list is empty");
        ret = _rsmp_errcod_json_parse_; // Liste vide
        goto cleanup;
    }

    *ciphers_list = calloc(*ciphers_len, sizeof(char *));
    if (!*ciphers_list) {
        _log_err("Failed to allocate memory for ciphers list");
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

    for (size_t i = 0; i < *ciphers_len; ++i) {
        struct json_object *cipher_element = json_object_array_get_idx(ciphers, i);
        const char *cipher_element_str = json_object_get_string(cipher_element);
        (*ciphers_list)[i] = strdup(cipher_element_str);
        if (!(*ciphers_list)[i]) {
            _log_err("Failed to duplicate cipher element string");
            ret = _rsmp_errcod_json_decode_;
            goto cleanup;
        }
    }

cleanup:
    if (parsed_json) {
        json_object_put(parsed_json);
    }
    // libérer les allocations en cas d'erreur
    if(!_rsmp_err_is_ok(ret)) {
        if (mId!=NULL && *mId) {
            free(*mId);
            *mId = NULL;
        }
        if (*ciphers_list) {
            for (size_t i = 0; i < *ciphers_len; ++i) {
                if ((*ciphers_list)[i]) 
                {
                    free((*ciphers_list)[i]);
                    (*ciphers_list)[i] = NULL;
                }
            }
            free(*ciphers_list);
            *ciphers_list = NULL;
        }
        *ciphers_len = 0;
        if(*sId_Remote_rxed) {
            free(*sId_Remote_rxed);
            *sId_Remote_rxed = NULL;
        }
    }

    return ret;
}


// décodage du MessageAck
int MessageAck_check(const void *buf_in, const size_t length, const char *mId /* mId de départ pour comparaison avec le oMId reçu */) {
    int ret = _rsmp_errcod_gen_no_error_;
    struct json_object *parsed_json = NULL;

    if (!buf_in || length == 0 || !mId) {
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }
    
    parsed_json = json_tokener_parse(buf_in);
    if (!parsed_json) {
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    struct json_object *mType;
    struct json_object *type;
    struct json_object *oMId;

    if (!json_object_object_get_ex(parsed_json, "mType", &mType) ||
        !json_object_object_get_ex(parsed_json, "type", &type) ||
        !json_object_object_get_ex(parsed_json, "oMId", &oMId)) {
        _log_err("Failed to get required JSON fields");
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    if (strcmp(json_object_get_string(mType), "eRMsg") != 0 ||
        strcmp(json_object_get_string(type), "MessageAck") != 0) {
        _log_err("mType = %s, type = %s", json_object_get_string(mType), json_object_get_string(type));
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    if(strncmp(json_object_get_string(oMId), mId, strlen(mId)) != 0) {
        _log_err("oMId = %s, mId = %s", json_object_get_string(oMId), mId);
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

cleanup:
    if(parsed_json) json_object_put(parsed_json);

    return ret;
}



int Ping_deserial(const void *buf_in, const size_t length, char **mId)
{
    int ret = _rsmp_no_error_;
    struct json_object *parsed_json = NULL;

    if (!buf_in || length == 0 || !mId) {
        return _rsmp_errcod_json_parse_;
    }

    const char *buf = (const char *)buf_in;
    parsed_json = json_tokener_parse(buf);
    if (!parsed_json) {
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    
    struct json_object *mType;
    struct json_object *type;
    struct json_object *padding_json;
    struct json_object *mId_json;


    if (!json_object_object_get_ex(parsed_json, "mType", &mType) ||
        !json_object_object_get_ex(parsed_json, "type", &type) ||
        !json_object_object_get_ex(parsed_json, "pad", &padding_json) ||
        !json_object_object_get_ex(parsed_json, "mId", &mId_json)) {
        _log_err("Failed to get required JSON fields");
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    if (strcmp(json_object_get_string(mType), "eRMsg") != 0 ||
        strcmp(json_object_get_string(type), "Ping") != 0) {
        _log_err("JSON fields 'mType' or 'type' do not match expected values");
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    char *padding = json_object_get_string(padding_json);
    if (!padding) {
        _log_err("Failed to get padding string");
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

    *mId = strdup(json_object_get_string(mId_json));
    if (!*mId) {
        _log_err("Failed to duplicate mId string");
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }
cleanup:
    if (parsed_json) {
        json_object_put(parsed_json);
    }
    // libérer les allocations en cas d'erreur
    if(!_rsmp_err_is_ok(ret)) {
        if (mId!=NULL && *mId) {
            free(*mId);
            *mId = NULL;
        }
    }

    return ret;
}


int Pong_deserial(const char *oMId, const char *payload, size_t len, char **mId)
{
    int ret = _rsmp_no_error_;
    struct json_object *parsed_json = NULL;

    if (!payload || len == 0 || !mId) {
        return _rsmp_errcod_json_parse_;
    }

    parsed_json = json_tokener_parse(payload);
    if (!parsed_json) {
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    struct json_object *mType;
    struct json_object *type;
    struct json_object *oMId_json;
    struct json_object *mId_json;

    if (!json_object_object_get_ex(parsed_json, "mType", &mType) ||
        !json_object_object_get_ex(parsed_json, "type", &type) ||
        !json_object_object_get_ex(parsed_json, "oMId", &oMId_json) ||
        !json_object_object_get_ex(parsed_json, "mId", &mId_json)) {
        _log_err("Failed to get required JSON fields");
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
        }
    
    if (strcmp(json_object_get_string(mType), "eRMsg") != 0 ||
        strcmp(json_object_get_string(type), "Pong") != 0) {
        _log_err("JSON fields 'mType' or 'type' do not match expected values");
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }
    char* oMId_rxed = json_object_get_string(oMId_json);
    if(!oMId_rxed) {
        _log_err("Failed to get oMId string");
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

    if(oMId && strcmp(oMId_rxed, oMId) != 0) {
        _log_err("JSON field 'oMId' does not match expected value");
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    *mId = strdup(json_object_get_string(mId_json));
    if (!*mId) {
        _log_err("Failed to duplicate mId string");
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

cleanup:
    if (parsed_json) {
        json_object_put(parsed_json);
    }
    // libérer les allocations en cas d'erreur
    if(!_rsmp_err_is_ok(ret)) {
        _free(*mId);
    }

    return ret;

}


int ChangePwdRequest_deserial(const void *buf_in, const size_t length, char **pwd_b64, char **mId)
{
    int ret = _rsmp_no_error_;
    struct json_object *parsed_json = NULL;

    if (!buf_in || length == 0 || !pwd_b64 || !mId) {
        return _rsmp_errcod_json_parse_;
    }

    const char *buf = (const char *)buf_in;
    parsed_json = json_tokener_parse(buf);
    if (!parsed_json) {
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    struct json_object *pwd;
    struct json_object *mId_json;

    if (!json_object_object_get_ex(parsed_json, "pwd", &pwd) ||
        !json_object_object_get_ex(parsed_json, "mId", &mId_json)) {
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    *pwd_b64 = strdup(json_object_get_string(pwd));
    if (!*pwd_b64) {
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

    *mId = strdup(json_object_get_string(mId_json));
    if (!*mId) {
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

cleanup:
    if (parsed_json) {
        json_object_put(parsed_json);
    }
    // libérer les allocations en cas d'erreur
    if(!_rsmp_err_is_ok(ret)) {
        if (pwd_b64!=NULL && *pwd_b64) {
            free(*pwd_b64);
            *pwd_b64 = NULL;
        }
        if (mId!=NULL && *mId) {
            free(*mId);
            *mId = NULL;
        }
    }

    return ret;
}


/**
 * @brief Décodage des Encrypted
 * 
 * \param[in] cipher pointeur sur le cipher utilisé
 * \param[in] sId_local identifiant local du serveur
 * \param[in] buf_in pointeur sur le buffer à décoder
 * \param[in] length longueur du buffer
 * \param[out] ptxt_list pointeur sur la liste des messages de type  de ptxt (payload .original + leur type .class + ts)
 * \param[out] valid_count pointeur sur le nombre de messages valides
 * \param[out] auth pointeur sur la structure d'authentification
 * \return int _rsmp_errcod_gen_no_error_ si tout va bien, enum errcod sinon
 */
// \todo basculer sur l'utilisation de deserializeMessage()
int Encrypted_decode(cipher_t const *cipher, char *sId_local, const void *buf_in, const size_t length, ptxt_t **ptxt_list, size_t *valid_count, auth_t **auth) {
    if (valid_count) *valid_count = 0;
    int ret = _rsmp_errcod_gen_no_error_;
    struct json_object *parsed_json = NULL;
    ptxt_t *ptxts = NULL;

    if (!cipher || !buf_in || length <= 0 || !ptxt_list || !valid_count) {
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    const char *buf = (const char *)buf_in;
    parsed_json = json_tokener_parse(buf);
    if (!parsed_json) {
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    struct json_object *jmType;
    struct json_object *jtype;
    struct json_object *jmId;
    struct json_object *jlogin;
    struct json_object *jsId;
    struct json_object *jdata;

    if (!json_object_object_get_ex(parsed_json, "mType", &jmType) ||
        !json_object_object_get_ex(parsed_json, "type", &jtype) ||
        !json_object_object_get_ex(parsed_json, "mId", &jmId) ||
        !json_object_object_get_ex(parsed_json, "login", &jlogin) ||
        !json_object_object_get_ex(parsed_json, "sId", &jsId) ||
        !json_object_object_get_ex(parsed_json, "data", &jdata)) {
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }
    const char *login = json_object_get_string(jlogin);
    const char *sId = json_object_get_string(jsId);

    if (strcmp(json_object_get_string(jmType), "eRMsg") != 0 ||
        strcmp(json_object_get_string(jtype), "Encrypted") != 0) {
        ret = _rsmp_errcod_json_parse_;
        goto cleanup;
    }

    size_t data_len = json_object_array_length(jdata);
    if (data_len == 0) {
        ret = _rsmp_errcod_json_parse_; // Liste vide
        goto cleanup;
    }

    ptxts = malloc(data_len * sizeof(ptxt_t));
    if (!ptxts) {
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }
    *valid_count = 0;

    for (size_t i = 0; i < data_len; ++i) {
        struct json_object *encrypted_data = json_object_array_get_idx(jdata, i);
        char *encrypted_str = json_object_get_string(encrypted_data);

        char *decrypted = NULL;
        size_t decrypted_len = 0;
        if (!_rsmp_err_is_ok(coder_decrypt(cipher, sId_local, json_object_get_string(jlogin), json_object_get_string(jsId), encrypted_str, &decrypted, auth)) || !decrypted) {
            _log_err("Erreur de décryptage (%s %s) pour l'élément %ld avec la clé courante", json_object_get_string(jlogin), cipher->lib, i);
            continue; // On passe au suivant
        }

        ptxt_t temp_msg = _ptxt_null_;
        if(!_rsmp_err_is_ok(ptxt_deserial(decrypted, &temp_msg)))
        {
            _log_err("Erreur de désérialisation pour l'élément %d\n", i);
            goto cleanup_decrypted;
        }

        if (!_rsmp_err_is_ok(ptxt_is_valid(&temp_msg))) {
            _log_err("Message invalide pour l'élément %d\n", i);
            goto cleanup_decrypted;
        }

        // insertion de l'élément dans la liste
        ptxts[(*valid_count)++] = temp_msg;
        ret = _rsmp_errcod_gen_no_error_; // si au moins un data valide alors le niveau supérieur peut-etre validé
        // _log_dump_info_in(decrypted, strlen(decrypted), " data[%ld] original=%s", i, temp_msg.original);

cleanup_decrypted:
        free(decrypted);

    }

    if (*valid_count == 0) {
        free(ptxts);
        *ptxt_list = NULL;
        ret = _rsmp_errcod_cipher_decrypt_; // Aucun élément valide
    } else {
        *ptxt_list = ptxts;
    }

cleanup:
    if (parsed_json) {
        json_object_put(parsed_json);
    }
    return ret;
}

/**@}*/