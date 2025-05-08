#include "config.h"

#include "cyber/challenge_ctxt.h"
#include "memory/checkmem.h"
#include "lstdblcyc/lstdblcyc.h"


ChallengeContext_t *challenge_ctxt_new() {
    ChallengeContext_t *ctxt=calloc(1,sizeof(*ctxt));
    // ldcInitialize(&ctxt->chain);
    return ctxt;
}

void freeChallengeContextMembers(ChallengeContext_t * ctxt) {
    if(ctxt) {
        ctxt->auth = NULL;
        _free(ctxt->mIdChangePwdRequest);
        _free(ctxt->mIdPing);
        _free(ctxt->pad);
    }
}

void freeChallengeContext(ChallengeContext_t ** ctxt) {
    if(*ctxt) {
        freeChallengeContextMembers(*ctxt);
        _free(*ctxt);
    }
}