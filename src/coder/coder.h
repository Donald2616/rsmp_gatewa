/**
* @file coder.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_CODER_CODER_H
#define RSMP_CODER_CODER_H
/**
 * @defgroup coder 
 * 
 * @brief 
 * 
 * @ingroup rsmp
 * @{
 */

#include "cipher/cipher.h"
#include "cyber/challenge_ctxt.h"
#include "auth/auth.h"

#ifdef RSMP_CODER_CODER_C
#define CODER_EXTERN
#define CODER_PRIVATE
#endif

#ifndef CODER_EXTERN
#define CODER_EXTERN extern
#endif

#ifndef CODER_PRIVATE
#define CODER_PRIVATE const
#endif


typedef enum {
    _coder_encrypt_ = 0,
    _coder_decrypt_
} coder_enum_action_t;


typedef enum {
    _cipher_class_internal_ = 0,
    _cipher_class_external_,
    _cipher_class_unknown_
} pt_enum_class_t;


typedef struct ptxt
{
    HoroDate timestamp;
    pt_enum_class_t class;
    char *       original;
} ptxt_t;


typedef enum {
    MSG_TYPE_ENCRYPTED,
    MSG_TYPE_ALLCLEAR,
    MSG_TYPE_CHANGE_PWD_REQUEST,
    MSG_TYPE_CHANGE_PWD_RESPONSE,
    MSG_TYPE_PING,
    MSG_TYPE_PONG
} MessageType;

typedef struct {
    char mId[UUID_STR_LEN];
} CommonFields;

typedef struct {
    char * sId; // remote sId
    char * login;
    char **data; // Assuming base64 encoded data length
    size_t data_len;
} EncryptedFields;

typedef struct {
    char *pwd; // Assuming base64 encoded password length
} ChangePwdRequestFields;

typedef struct {
    bool status;
    char oMId[UUID_STR_LEN];
} ChangePwdResponseFields;

typedef struct {
    char *padding; // Assuming max padding length
} PingFields;

typedef struct {
    char oMId[UUID_STR_LEN];
    char *padding; // Assuming max padding length
} PongFields;

typedef union {
    EncryptedFields encrypted;
    ChangePwdRequestFields changePwdRequest;
    ChangePwdResponseFields changePwdResponse;
    PingFields ping;
    PongFields pong;
} MessageFields;

typedef struct {
    CommonFields common;
    MessageFields fields;
    MessageType messageType;
} Message;

#ifdef RSMP_CODER_CODER_C
ptxt_t const _ptxt_null_={.original=NULL, .timestamp={.tv_nsec=0, .tv_sec=0}};
#else
extern ptxt_t const _ptxt_null_;
#endif

int Encrypted_decode(cipher_t const *cipher, char *sId_local, const void *buf_in, const size_t length, ptxt_t **ptxt_list, size_t *valid_count, auth_t **auth);

int CipherAgreement_deserial(const void *buf_in, const size_t length, char ***ciphers_list, size_t *ciphers_len, char **mId, char **sId_Remote);

int MessageAck_check(const void *buf_in, const size_t length, const char *mId);


int coder_decrypt(cipher_t const *cipher, const auth_sId_t sId_local, const auth_login_t login, const auth_sId_t sId_remote, char *in, char **out, auth_t **auth);
int ptxt_deserial(char *payload, ptxt_t *ptxt);
int ptxt_set(ptxt_t *ptxt, HoroDate const *timestamp, char const *original);
 int coder_encrypt(cipher_t const *cipher, const auth_sId_t sId_local, const auth_login_t login, const auth_sId_t sId_remote, char const *in, char **out, auth_t **auth);
int ptxt_serialize(ptxt_t *ptxt, char **out);

void coder_time_windows_set(int val);

int Encrypted_encode(cipher_t const *cipher, char *sId_local, const char *login, const char *sId, ptxt_t *ptxt_list, size_t ptxt_count, char ** payload);

int CipherAgreement_serial(const char* sId_local, char ** ciphers_list, size_t ciphers_len, char ** payload, size_t *len, char ** mId);

int MessageAck_serial(const char *mId, char ** payload, size_t *len);

int Challenge_encode(ChallengeContext_t * ctxt, cipher_t const *cipher, char *sId_local, const char *login, const char *sId_remote, char ** payload, size_t *len);

Message * initMessage(MessageType type);
int serializeMessage(Message *msg, char **payload, size_t *len);
int deserializeMessage(const char *payload, size_t len, Message **msg);
void freeMessage(Message **msg);

// retourne 0 si les champs du message sont valides, -1 sinon
int messageCheckFields(Message *msg);

/*inline \todo pq inline ne passe pas l'assemblage ? */bool isAuthToAvailable(const auth_login_t login, const auth_sId_t sId_remote);

/**@}*/
#endif //CODER_CODER_H