#ifndef CHALLENGE_CTXT_H
#define CHALLENGE_CTXT_H

#include "horodate/horodate.h"
#include "auth/auth.h"
#include "lstdblcyc/lstdblcyc.h"

enum ChallengeResponse {
    CHALLENGE_RESPONSE_NONE = 0,
    CHALLENGE_RESPONSE_SUCCESS,
    CHALLENGE_RESPONSE_BUSY,
    CHALLENGE_RESPONSE_TIMEOUT
};

// structure contexte challenge pour memoriser les mId des messages ChangePwdRequest et Ping, l'horodate des conteneurs de chiffrement, le pad du Ping, le login, l'état du challenge en cours
typedef struct {
    // sDblChain chain;
    auth_t *auth;
    bool toSend;
    bool running;
    void *timer_timeout_resp;
    HoroDate timestamp;
    char *mIdChangePwdRequest;
    char *mIdPing;
    char *pad;
    enum ChallengeResponse response;
} ChallengeContext_t;

ChallengeContext_t *challenge_ctxt_new();
void freeChallengeContextMembers(ChallengeContext_t * ctxt);
void freeChallengeContext(ChallengeContext_t ** ctxt);

#endif //CHALLENGE_CTXT_H