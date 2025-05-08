/**
* @file cyber.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-06-27
* 
* @copyright Copyright (c) 2023
* 
*/
#define CYBER_CYBER_C
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "lstdblcyc/lstdblcyc.h"
#include "lock/mutex.h"
#include "lock/condition.h"
#include "sync/queue.h"
#include "coder/coder.h"
#include "crypt/bcd.h"




#include "auth/auth.h"
#include "auth/auth-basic.h"
#include "auth/auth-db.h"
#include "auth/auth-db-to.h"
#include "auth/auth-db-from.h"
#include "misc/flux.h"
#include "misc/thread.h"
#include "errcode/errcode.h"
#include "event/event.h"
#include "timer/timer.h"

#include <unistd.h>














#include "memory/checkmem.h"
#include "cyber/cyber.h"
#include "cyber/challenge_ctxt.h"

#ifdef LOG_SECTION
#undef LOG_SECTION
#endif
#define LOG_SECTION "CYBER"

static int _stop(struct cyber *this, bool force);

/**
 * @ingroup cyber
 * @{
 */



/*

88b           d88   ad88888ba     ,ad8888ba,               ,ad8888ba,
888b         d888  d8"     "8b   d8"'    `"8b             d8"'    `"8b
88`8b       d8'88  Y8,          d8'                      d8'        `8b
88 `8b     d8' 88  `Y8aaaaa,    88                       88          88
88  `8b   d8'  88    `"""""8b,  88      88888            88          88
88   `8b d8'   88          `8b  Y8,        88            Y8,    "88,,8P
88    `888'    88  Y8a     a8P   Y8a.    .a88             Y8a.    Y88P
88     `8'     88   "Y88888P"     `"Y88888P"               `"Y8888Y"Y8a

                                             888888888888
*/

/*

88888888ba   88888888ba   88  8b           d8
88      "8b  88      "8b  88  `8b         d8'
88      ,8P  88      ,8P  88   `8b       d8'
88aaaaaa8P'  88aaaaaa8P'  88    `8b     d8'
88""""""'    88""""88'    88     `8b   d8'
88           88    `8b    88      `8b d8'
88           88     `8b   88       `888'
88           88      `8b  88        `8'


*/
static ldcDeclareHead(_cyber_list);

typedef struct {
    // size_t pw_len; // longueur de la bdd reprise à l'identique du mdp initial
    int timeout_challenge_rx_sec; // période de réception du challenge
} config_t;


config_t *config_new() {
    config_t *cfg=calloc(1,sizeof(*cfg));
    
    cfg->timeout_challenge_rx_sec = 60; 
    // cfg->timeout_challenge_rx_sec = 10; 
    // \todo à mettre dans le fichier de configuration 

    return cfg;
}
typedef struct cyber_priv
{
    sDblChain       chain;
    cyber_t * this;
    transport_t     * transport;
    lock_mutex_t    mutex;
    queue_t       * send_q;
    queue_t       * internal_q;
    queue_t       * external_q;
    condition_t     send_enabled;
    condition_t     internal_enabled;
    condition_t     external_enabled;
    void *          watchdog;
    void *          cmd_request;
    char *           sId_Local;
    char *           sId_Remote;
    pthread_context_t *receive_task;
    pthread_context_t *internal_task;
    pthread_context_t *send_task;
    ChallengeContext_t * challenge_ctxt;
    void * timer_timeout_allClearRxed; // \todo timeout de réception du AllClear
    // \todo compter les echecs de challenge et désactiver le login
    bool allClearRxed;
    bool allClearSent;
    int to_sec;
    int inhib_s;
    int stop;
    config_t * config;
#ifdef TEST_ENABLED
    bool disableChallenge;
#endif
} cyber_priv_t;

#define cyber_from_lst(n) container_of(n, cyber_priv_t, chain)

static int send_signal_enabled_eval(void *context);
static int internal_signal_enabled_eval(void *context);
static int external_signal_enabled_eval(void *context);


// callback pour le traitement des nouvelles connexions entrantes
// point de vue server, fonction de callback pour la connexion follower entrante
// void fct_callback_connected(cyber_t *cyber_srv_ctx, cyber_t *cyber_incoming) 
void callback_incoming_connection(void *context, void *data)
{
    cyber_t *cyber_srv_ctx = context;

    transport_t * transport = data;
    cyber_t *cyber_incoming = cyber_new_from_transport(transport);

    _log_info("Starting exchange with transport: %p)", transport);


    cyber_incoming->setSiteId_local(cyber_incoming, cyber_srv_ctx->getSiteId_local(cyber_srv_ctx));
    cyber_incoming->enableRoleLeader(cyber_incoming);
#ifdef TEST_ENABLED
    const cyber_priv_t* priv_srv = cyber_srv_ctx->dptr;
    cyber_disableChallenge(cyber_incoming, priv_srv->disableChallenge);
#endif
    int ret = cyber_incoming->start(cyber_incoming);
    if(!_rsmp_err_is_ok(ret))
    {
        _log_err("failed to exchange (ret = %d)", ret);
        return;
    }

    _log_info("Connected with %s", cyber_incoming->getSiteId_remote(cyber_incoming));

    const cyber_priv_t* priv = cyber_incoming->dptr;
    unsigned int cpt = 0;
    while(!priv->stop) { // boucle pour maintenir le thread en vie
        sleep(1);
        if(cpt%60 == 0) _log_info("cyber_incoming still alive (cyber @%p)", cyber_incoming);
        cpt++;
    }

    // // server en attente de message version
    // char *payload=NULL;
    // int len;
    // int ret = cyber_priv->transport->waitForPayload(cyber_incoming, &payload, &len);
    // if (ret != 0)
    // {
    //     _log_error("failed to receive the message");
    //     return -1;
    // }


    // _log_info("Received message: %s", payload);


    // \todo fermeture propre du thread créé pour la connexion entrante

}


void fct_log_error(const cyber_t *cyber_srv_ctx, const cyber_t *engine_incomming, int sId_remote, const char *msg) {
    (void)cyber_srv_ctx;
    (void)engine_incomming;
    _log_err("failed to connect with %d: %s", sId_remote, msg);
}


static cyber_priv_t *cyber_priv_new(transport_t * transport)
{
    cyber_priv_t *item=calloc(1,sizeof(*item));

    if (item)
    {
        item->transport = transport;
        mutex_initialize_rec(&item->mutex);
        ldcInitialize(&item->chain);
        item->send_q = queue_new();
        item->internal_q = queue_new();
        item->external_q = queue_new();
        condition_initialize(&item->send_enabled);
        condition_check_set(&item->send_enabled, send_signal_enabled_eval, item);

        condition_initialize(&item->internal_enabled);
        condition_check_set(&item->internal_enabled, internal_signal_enabled_eval, item);

        condition_initialize(&item->external_enabled);
        condition_check_set(&item->external_enabled, external_signal_enabled_eval, item);

        item->to_sec = -1; // 10 sec pour timeout reception CipherAgreement puis Version
        item->inhib_s = 10; // attente 10 sec avant de faire tomber la connexion tcp après une erreur dans l'échange du Cipher
        item->sId_Local = NULL;
        item->sId_Remote = NULL;
        item->challenge_ctxt = challenge_ctxt_new();
        item->config = config_new();
#ifdef TEST_ENABLED
        item->disableChallenge = false;
#endif
    }
    return item;
}


static void cyber_priv_free(cyber_priv_t *priv)
{
    assert(priv!=NULL);
    mutex_destroy(&priv->mutex);
    
    if (priv->challenge_ctxt){ 
        if(priv->challenge_ctxt->timer_timeout_resp) {
            TimerDelete(priv->challenge_ctxt->timer_timeout_resp);
        }
        freeChallengeContext(&priv->challenge_ctxt);
    }
    if(priv->transport) priv->transport->free(priv->transport);
    if (priv->send_q) priv->send_q->free(priv->send_q);
    if (priv->internal_q) priv->internal_q->free(priv->internal_q);
    if (priv->external_q) priv->external_q->free(priv->external_q);
    condition_destroy(&priv->send_enabled);
    condition_destroy(&priv->internal_enabled);
    condition_destroy(&priv->external_enabled);
    free(priv);
}


#ifdef TEST_ENABLED
void cyber_disableChallenge(cyber_t* this, bool val)
{
    cyber_priv_t *priv = this->dptr;
    priv->disableChallenge = val;
}
#endif

/*

888888888888  88        88  88888888ba   88888888888         db         88888888ba,
     88       88        88  88      "8b  88                 d88b        88      `"8b
     88       88        88  88      ,8P  88                d8'`8b       88        `8b
     88       88aaaaaaaa88  88aaaaaa8P'  88aaaaa          d8'  `8b      88         88  ,adPPYba,
     88       88""""""""88  88""""88'    88"""""         d8YaaaaY8b     88         88  I8[    ""
     88       88        88  88    `8b    88             d8""""""""8b    88         8P   `"Y8ba,
     88       88        88  88     `8b   88            d8'        `8b   88      .a8P   aa    ]8I
     88       88        88  88      `8b  88888888888  d8'          `8b  88888888Y"'    `"YbbdP"'


*/

static int send_signal_enabled_eval(void *context)
{
    cyber_priv_t *item = context;
    _log_debug("send_signal_enabled_eval : nb ptxt %d", item->send_q->no(item->send_q));
    return item->send_q->no(item->send_q);
}

static int internal_signal_enabled_eval(void *context)
{
    cyber_priv_t *item = context;
    _log_debug("internal_signal_enabled_eval : nb ptxt %d", item->internal_q->no(item->internal_q));
    return item->internal_q->no(item->internal_q);
}

static int external_signal_enabled_eval(void *context)
{
    cyber_priv_t *item = context;
    _log_debug("external_signal_enabled_eval : nb ptxt %d", item->external_q->no(item->external_q));
    return item->external_q->no(item->external_q);
}

void print_queue_msg(const queue_msg_t *msg) {
    if (msg == NULL) {
        printf("Message is NULL\n");
        return;
    }

    _log_debug("Message Details:\n");
    // _log_debug("Buffer: %s\n", msg->buf);
    _log_debug("Length: %zu\n", msg->length);
    _log_debug("Priority: %d\n", msg->prio);
    _log_debug("Mark: %p\n", msg->mark);
    _log_debug("Mark: %s\n", (const char *)msg->mark);
    // _log_debug("Cyber: %p\n", msg->cyber);
    _log_debug("Flag Encrypt: %d\n", msg->flag_do_encrypt);
    _log_debug("Attrib: %p\n", msg->attrib);
}

int Encrypted_write(transport_t * transport, rsmp_cipher_t* cipher, char* sId_local, char *login, char* sId_remote, char* buf_in);

/// @brief tâche d'envoi des messages
/// @param arg, pointeur sur cyber
/// @return 
static void *cyber_send(void *arg)
{
    cyber_t *this = arg;
    cyber_priv_t *priv = this->dptr;
    queue_t *send_q = priv->send_q;
    transport_t * transport = priv->transport;

    while(!priv->stop)
    {
        _log_debug("cyber_send : wait for send_enabled");

        //-- Attente de la condition d'ouverture de la communication
        //-- Attente infinie sur la queue de message
        condition_wait(&priv->send_enabled, 1000000 /* 1 sec */);

            
    //-- Allocation dynamique de msg_q
    queue_msg_t *msg_q = (queue_msg_t *)malloc(sizeof(queue_msg_t));
    if (msg_q == NULL) {
        _log_err("Failed to allocate memory for msg_q");
        return NULL;
    }

    //-- C'est bon on extrait le message !
    if (!send_q->pull(send_q, msg_q, 1 /* 1 µs (on a deja attendu dans le condition_wait)*/))
    {
        print_queue_msg(msg_q);

        // réccupération du contexte pour l'encodage aes
        char *login = msg_q->attrib;

        int ret = _rsmp_no_error_;
        // envoi d'un message crypté si le cipher est bien positionné
        if (msg_q->flag_do_encrypt) {
            ret = Encrypted_write(transport, this->cipher, priv->sId_Local, login, priv->sId_Remote, msg_q->buf);
        } else {
            // flag de débrayage actif dans la structure msg_q
            // écriture directe sur le flux, cas du challenge (changePwd + Ping)
            ret = transport->send(transport, msg_q->buf, msg_q->length);
        }

        if (!_rsmp_err_is_ok(ret)) {
            _log_errno("encode fail");
        }

            free(msg_q->buf);
            free((char *)msg_q->mark);

        }
        // else
        // {
        //     static int cpt_dbg = 0;
        //     if (cpt_dbg < 10)
        //     {
        //         _log_info("send queue empty (display 10 first occurences)");
        //         cpt_dbg++;
        //     }
        // }
    } 
    _log_info("cyber_send end");
    return NULL;
}

int Encrypted_write(transport_t * transport, rsmp_cipher_t* cipher, char* sId_local, char *login, char* sId_remote, char* buf_in) {

    int ret = _rsmp_no_error_;

    assert(sId_local);

    if(!cipher) {
        _log_err("cipher is NULL");
        return _rsmp_errcod_cyber_generic_;
    }

    if(!sId_remote) {
        _log_err("sId_remote is NULL");
        return _rsmp_errcod_auth_failing_input_;
    }

    char *request;
    ptxt_t *ptxt_list = malloc(sizeof(ptxt_t));
    ptxt_list[0] = _ptxt_null_;
    ret = ptxt_set(&ptxt_list[0], NULL /* ts */, buf_in);
    if (!_rsmp_err_is_ok(ret))
    {
        _log_err("encode data[0] fail");
        return _rsmp_errcod_gen_encode_;
    }

    // if (login)
    {
        ret = Encrypted_encode(cipher, sId_local, login, sId_remote, ptxt_list, 1, &request);
    }
    // else
    // {
    //     auth_t *auth = NULL;
    //     auth_key_t const *key = NULL;
    //     key = auth_to_key_set(&key, NULL /* login */, sId_remote);
    //     auth = auth_get_first(key);
    //     ret = Encrypted_encode(cipher, sId_local, auth->key.login, sId_remote, ptxt_list, 1, &request);
    // }

    if (!_rsmp_err_is_ok(ret))
    {
        _log_err("encode Encrypted fail");
        return _rsmp_errcod_gen_encode_;
    }

    transport->send(transport, request, strlen(request));
    // free(request); \todo libération de la mémoire 

    return ret;
}
#ifdef TEST_ENABLED
static int _disable_encryption(cyber_t *this)
{
    cyber_priv_t *priv = this->dptr;
    assert(priv->this->cipher == NULL||priv->this->cipher == cipher_get_fake_one_disabling_encryption());
    priv->this->cipher = cipher_get_fake_one_disabling_encryption();
    return 0;
}
#endif

// objet context pour la fonction de callback de cipher_foreach
typedef struct {
    char **ciphers_list;
    size_t ciphers_len;
} cipher_foreach_context_t;

static int cipher_foreach_cb(void *context, const cipher_t *cipher_desc)
{
    if (cipher_desc->enabled)
    {
        cipher_foreach_context_t *_ciphers = context;
        // allocation de la liste des ciphers (ou realloc)
        _ciphers->ciphers_list = realloc(_ciphers->ciphers_list, (_ciphers->ciphers_len + 1) * sizeof(char *));
        _ciphers->ciphers_list[_ciphers->ciphers_len++] = strdup(cipher_desc->lib);
    }
    return 1;
}

int initiateChallenge(cyber_priv_t *priv);

// Fonction pour dormir pendant un certain nombre de millisecondes
void msleep(unsigned int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;

    // Appeler nanosleep pour dormir pendant le temps spécifié
    nanosleep(&ts, NULL);
}

/**
 * @brief Lance la séquence d'initialisation (follower)
 * 
 * @param priv 
 * @return ==0 si OK, !=0 sur erreur 
 * @return false 
 */
static int cyber_exchange_follower(cyber_priv_t *priv)
{
    _log_debug("cyber_exchange_follower...");
#ifdef TEST_ENABLED
    if(priv->this->cipher == cipher_get_fake_one_disabling_encryption()){

         return 0;
    } 
#endif

    int ret = _rsmp_no_error_;

    assert(priv != NULL);
    
    transport_t *transport = priv->transport; 
    // if(!transport->isConnected(transport)) {
    //     _log_err("transport is not connected");
    //     return _rsmp_errcod_family_transport_; \todo reactiver test (debug)
    // }   

    char *sndPayload = NULL;
    size_t sndLen = 0;
    char *mId = NULL;
    char *sId_Remote_rxed = NULL;
    char *rcvPayload = NULL;
    size_t rcvLen = 0;

    // construction de la liste des ciphers valides    
    // objet contexte call back comprenant le tableau de char * et le nombre d'elements
    cipher_foreach_context_t ciphers_context = {0};
    cipher_foreach(cipher_foreach_cb, &ciphers_context);

    // envoi de la liste des ciphers disponibles
    // construction du message CipherAgreement
    ret = CipherAgreement_serial(priv->sId_Local, ciphers_context.ciphers_list, ciphers_context.ciphers_len, &sndPayload, &sndLen, &mId);

    // libération liste des ciphers
    for (size_t i = 0; i < ciphers_context.ciphers_len; i++) {
        _free(ciphers_context.ciphers_list[i]);
    }
    _free(ciphers_context.ciphers_list);
    ciphers_context.ciphers_len = 0;

    if (!_rsmp_err_is_ok(ret)) {
        _log_err("encode fail");
        goto cleanup;
    }

    // envoi du message
    ret = transport->send(transport, sndPayload, sndLen);
    _free(sndPayload);
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("write fail");
        goto cleanup;
    }


    // réception du cipher retenu
    _free(rcvPayload);
    rcvLen = 0;
    ret = transport->waitForPayload(transport, &rcvPayload, &rcvLen, priv->to_sec*1000);
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("reading payload failed");
        goto cleanup;
    }
    _free(mId);

    ret = CipherAgreement_deserial(rcvPayload, rcvLen, &ciphers_context.ciphers_list, &ciphers_context.ciphers_len, &mId, &sId_Remote_rxed);
    _free(rcvPayload);
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("CipherAgreement parser failed"); // \todo quand faut il fournir un nack ?
        goto cleanup;
    }
    _free(rcvPayload);

    // vérification de l'identifiant du remote
    if(priv->sId_Remote) {
        if(strcmp(sId_Remote_rxed, priv->sId_Remote) != 0) {
            _log_err("sId_Remote mismatch");
            ret = _rsmp_errcod_json_parse_;
            goto cleanup;
        }
    } else {
        priv->sId_Remote = strdup(sId_Remote_rxed);
    }

    // valider et enregistrer le choix du cipher
    // liste de 1 element
    if (ciphers_context.ciphers_len != 1) {
        _log_err("cipher not valid");
        ret = _rsmp_errcod_cyber_generic_;
        goto cleanup;
    }

    // element existant et "enabled"
    priv->this->cipher = cipher_get_by_str(ciphers_context.ciphers_list[0]);
    if (priv->this->cipher == NULL || priv->this->cipher->enabled == false) {
        _log_err("cipher not valid");
        ret = _rsmp_errcod_json_decode_;
        goto cleanup;
    }

cleanup:
    _free(rcvPayload);
    _free(mId);
    _free(sId_Remote_rxed);
    _free(sndPayload);
    
    // libération liste des ciphers
    for (size_t i = 0; i < ciphers_context.ciphers_len; i++) {
        _free(ciphers_context.ciphers_list[i]);
    }
    _free(ciphers_context.ciphers_list);
    ciphers_context.ciphers_len = 0;

    return ret;
}

void handler_timeout_challenge(void *timer, void *context)
{
    cyber_priv_t *priv = context;
    if(priv->challenge_ctxt && priv->challenge_ctxt->running)
    {
        _log_err("challenge timeout, close connection"); // \todo éventuellement voir s'il faut réarmer le timer sur l'arrivée d'un internal valide ?
        _stop(priv->this, false);
        // \todo faut il faire des flush de queue ?
        // \todo faut il dissocier/temporiser la coupure du transport ?
        
    } else {
        _log_err("timer zombie %p", timer);
    }
    _log_info("handler timeout end (timer %p)", timer);
}

static int stopTimerResponse(ChallengeContext_t *challenge_ctxt)
{
    if (challenge_ctxt && challenge_ctxt->timer_timeout_resp != NULL)
    {
        _log_info("désarmement timer %p d'attente de réponse challenge", challenge_ctxt->timer_timeout_resp);
        TimerDelete(challenge_ctxt->timer_timeout_resp);
        challenge_ctxt->timer_timeout_resp = NULL;
    }
    return _rsmp_errcod_gen_no_error_;
}

static int launchTimerResponse(cyber_priv_t *priv)
{
    if(!priv->challenge_ctxt) {
        _log_err("challenge_ctxt is NULL");
        return _rsmp_errcod_cyber_generic_;
    }

    stopTimerResponse(priv->challenge_ctxt);

    priv->challenge_ctxt->timer_timeout_resp = TimerCreate(handler_timeout_challenge, priv, ONCE, NULL);
    if (priv->challenge_ctxt->timer_timeout_resp)
    {
        _log_info("armement timer %p pour réponse challenge (%d sec)", priv->challenge_ctxt->timer_timeout_resp, priv->config->timeout_challenge_rx_sec);
        TimerUpdate(priv->challenge_ctxt->timer_timeout_resp, priv->config->timeout_challenge_rx_sec);
        return _rsmp_errcod_gen_no_error_;
    }
    else
    {
        _log_err("alloc timer_timeout_resp");
        return _rsmp_errcod_cyber_generic_;
    }
}

// retourne le premier login répondant aux conditions (reprise Challenge ou pw expiré)
static int searchLoginForChallenge(void *context, const auth_dbId_t dbId, auth_t const *auth)
{
    cyber_priv_t *priv = context;
    if(priv->challenge_ctxt && priv->challenge_ctxt->auth && (priv->challenge_ctxt->running || priv->challenge_ctxt->toSend)){
        // _log_debug("a challenge is already processing");
         return 0; // un login a déjà été trouvé ou en cours de Challenge
    }
    if(priv->challenge_ctxt && priv->challenge_ctxt->auth && !priv->challenge_ctxt->running && !priv->challenge_ctxt->toSend){
        stopTimerResponse(priv->challenge_ctxt);
        freeChallengeContext(&priv->challenge_ctxt);
    }

    bool foundAuthForChallenge = false;
    char *pwd = NULL;
    char *newPwd = NULL;
 
    if (auth->pwd.len && auth->pwd.buf) pwd = bcd_encode(auth->pwd.buf, auth->pwd.len);
    if (auth->new_pwd_pending && auth->new_pwd.len && auth->new_pwd.buf) newPwd = bcd_encode(auth->new_pwd.buf, auth->new_pwd.len);

    // affichage login et etat
    // if(!auth->new_pwd_pending) _log_debug("sId %s found login %s with pwd %s", auth->key.sId, auth->key.login, pwd);
    // else  _log_debug("sId %s found login %s with pwd %s and new_pwd %s", auth->key.sId, auth->key.login, pwd, newPwd);
   
    // si Challenge en cours 
    if(auth->new_pwd_pending) foundAuthForChallenge = true;

    // affichage du delta de temps
    HoroDate now;
    setHorodate(&now);
    long delta_sec = diffHorodateSec(&now, &auth->timestamp);
    int validPeriod_days = 0; // \todo parametriser le nombre de jours
    int validPeriod_sec = 135; // \todo parametriser le nombre de sec et peut etre le borner
    // int validPeriod_sec = 3600; // \todo parametriser le nombre de sec
                              // \todo ratio arbitraire de 3/4 (spec ?)
    // if (priv->this->isLeader(priv->this)) validPeriod_sec = 24 * 3600 * 60; // testing
    long triggerPeriod_sec = ((3600*24*3)/4)*validPeriod_days+(validPeriod_sec*3)/4;
    #ifndef TEST_ENABLED
    static bool once = true;
    if(triggerPeriod_sec<=0 && once) {
        // message affiché une seule fois
        _log_err("periode renouvellement nulle ou negative (triggerPeriod_sec=%ld)", triggerPeriod_sec);
        once = false;
    }
    #endif
    if(triggerPeriod_sec>0 && delta_sec>triggerPeriod_sec){ // si pwd expiré
        _log_info("pwd login %s expiré ou proche de son expiration (%ld > %ld sec)", auth->key.login, delta_sec, triggerPeriod_sec);
        foundAuthForChallenge = true;
    }

    if(foundAuthForChallenge) // créer le contexte du challenge avec le login
    {
        if (!priv->challenge_ctxt)
        {
            priv->challenge_ctxt = challenge_ctxt_new();
            if (!priv->challenge_ctxt)
            {
                _log_err("alloc challenge_ctxt");
                return 0; // arrêt du parcours de la base
            }
        }
        

        // Initialiser le générateur de nombres aléatoires
        srand(time(NULL)); // \todo remplacer rand par RAND_bytes ; quid de l'initialisation du generateur de nombres ?

        // Générer un nombre aléatoire entre 0 et 5000 millisecondes
        int wait_time_ms = rand() % 5001; // \todo remplacer rand par RAND_bytes

        // Enregistrer l'attente dans le journal
        _log_info("attente pendant %d ms", wait_time_ms);

        // Convertir en microsecondes et attendre
        usleep(wait_time_ms * 1000);

        // Enregistrer l'attente dans le journal
        _log_info("fin attente %d ms", wait_time_ms);

        priv->challenge_ctxt->auth = auth;
        priv->challenge_ctxt->toSend = true;
    }

    _free(pwd);
    _free(newPwd);
    
    return 1; // continue le parcours de la base
}

/**
 * @brief Lance la séquence d'initialisation
 * 
 * @param priv 
 * @return ==0 si OK, !=0 sur erreur 
 * @return false 
 */
static int cyber_exchange_leader(cyber_priv_t *priv)
{
    _log_debug("cyber_exchange_leader\n");
#ifdef TEST_ENABLED
    if (priv->this->cipher == cipher_get_fake_one_disabling_encryption()) {
        // return _rsmp_err_is_ok;
        return -1;
    }
#endif

    assert(priv != NULL);

    char **ciphers_list = NULL;
    size_t ciphers_len = 0;
    char *mId = NULL;
    char *sId_Remote_rxed = NULL;
    char *rcvPayload = NULL;
    size_t rcvLen = 0;

    transport_t * transport = priv->transport;    

    _free(rcvPayload);
    rcvLen = 0;
    int ret = transport->waitForPayload(transport, &rcvPayload, &rcvLen, priv->to_sec*1000);
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("reading payload failed");
        goto cleanup;
    }
    _free(mId);

    ret = CipherAgreement_deserial(rcvPayload, rcvLen, &ciphers_list, &ciphers_len, &mId, &sId_Remote_rxed);
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("CipherAgreement parser failed"); // \todo quand faut il fournir un nack ?
        goto cleanup;
    }
    
    // vérification de l'identifiant du remote
    if(priv->sId_Remote) {
        if(strcmp(sId_Remote_rxed, priv->sId_Remote) != 0) {
            _log_err("sId_Remote mismatch");
            ret = _rsmp_errcod_json_parse_;
            goto cleanup;
        }
    } else {
        priv->sId_Remote = sId_Remote_rxed;
        sId_Remote_rxed = NULL;
    }

    _log_info("CipherAgreement received");

    // identifier les ciphers communs
    // affichage des ciphers reçus
    char *ciphers_str = NULL;
    char *temp_str = NULL;

    // Utiliser asprintf pour construire la chaîne de caractères
    asprintf(&ciphers_str, "Afficher les ciphers recus (%ld): ", ciphers_len);
    for (int i = 0; i < ciphers_len; i++)
    {
        if (i == 0)
        {
            asprintf(&temp_str, "%s%s", ciphers_str, ciphers_list[i]);
        }
        else
        {
            asprintf(&temp_str, "%s, %s", ciphers_str, ciphers_list[i]);
        }
        free(ciphers_str);
        ciphers_str = temp_str;
    }

    // le log
    _log_info("%s", ciphers_str);

    // Libérer la mémoire allouée
    free(ciphers_str);

    // affichage des ciphers disponibles
    // question par rapport au banissement d'un cipher : cipher_foreach doit il avoir une liste configurable ? (aujourd'hui en dur)
    // TODO comparaison des ciphers disponibles et choix du cipher
    // proposition : l'ordre des ciphers dans la liste locale indique la priorité. Parcours de la liste locale (leader) depuis la fin. Le premier cipher commun est retenu.
    // TODO : si aucun cipher commun, tracer et couper la connexion

    // test : construction d'une reponse avec le dernier de la liste
    cipher_t const *tmp = NULL;

    for (int i = ciphers_len - 1; i >= 0; i--) {
        _log_info("Checking cipher: %s (prend le dernier de la liste)", ciphers_list[i]);
        tmp = cipher_get_by_str(ciphers_list[i]);

        if (tmp == NULL) {
            _log_info("Cipher not found: %s", ciphers_list[i]);
            continue;
        }

        _log_info("Cipher found: %s", ciphers_list[i]);

        if (!tmp->enabled) {
            _log_err("Cipher not valid: %s", ciphers_list[i]);
            tmp = NULL; // Reset tmp to ensure we continue searching
            continue;
        }

        break;
    }

    if (tmp == NULL) {
        _log_err("No valid cipher found in the list");
        ret = _rsmp_errcod_cyber_generic_;
        goto cleanup;
    }

    // envoyer un Ack de la demande
    char *sndPayload = NULL;
    size_t sndLen = 0;

    // construire la réponse du cipher choisi
    ciphers_len = 1;
    ciphers_list = calloc(ciphers_len, sizeof(char *));
    ciphers_list[0] = strdup(tmp->lib);

    ret = CipherAgreement_serial(priv->sId_Local, ciphers_list, ciphers_len, &sndPayload, &sndLen, &mId);
    _free(ciphers_list[0]);
    _free(ciphers_list);
    ciphers_len = 0;
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("encode fail");
        goto cleanup;
    }
    ret = transport->send(transport, sndPayload, sndLen);
    _free(sndPayload);
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("write fail");
        goto cleanup;
    }

    // mémorisation du cipher
    priv->this->cipher = tmp;

cleanup:
    _free(rcvPayload);
    _free(mId);
    _free(sId_Remote_rxed);
    _free(sndPayload);
    
    // libération de la liste des ciphers reçus
    for (size_t i = 0; i < ciphers_len; i++)
    {
        _free(ciphers_list[i]);
    }
    _free(ciphers_list);
    ciphers_len = 0;

    return ret;
}


static int _sendMessage(cyber_t *this, Message *msg, void const *attrib);

// retourne le premier login répondant aux conditions (reprise Challenge ou pw expiré)
static int printLogin(void *context, const auth_dbId_t dbId, auth_t const *auth)
{
    cyber_priv_t *priv = context;
    
    assert(auth);
     char *pwd = NULL;
    char *newPwd = NULL;
    
    if (auth->pwd.len && auth->pwd.buf) pwd = bcd_encode(auth->pwd.buf, auth->pwd.len);
    if (auth->new_pwd_pending && auth->new_pwd.len && auth->new_pwd.buf) newPwd = bcd_encode(auth->new_pwd.buf, auth->new_pwd.len);

    if(!auth->new_pwd_pending) _log_info("sId %s found login %s with pwd %s", auth->key.sId, auth->key.login, pwd);
    else  _log_info("sId %s found login %s with pwd %s and new_pwd %s", auth->key.sId, auth->key.login, pwd, newPwd);
    
    return 1; // continue le parcours de la base
}

int sendAllClear(cyber_priv_t *priv)
{
    int ret = _rsmp_errcod_cyber_no_error_;
    Message *msg = NULL;
    msg = initMessage(MSG_TYPE_ALLCLEAR);
    if(!msg) {
        _log_err("Erreur initMessage");
        return _rsmp_errcod_cyber_generic_;
    }
// if(priv->this->is_leader) sleep(10); // \todo pour test Version qui part avant les AllClear

    ret = _sendMessage(priv->this, msg, NULL);
    
    if (!_rsmp_err_is_ok(ret)) {
        _log_err("write fail");
        goto cleanup;
    }

    priv->allClearSent = true;
    if(priv->allClearRxed) trigger_event(EVENT_TYPE_ALLCLEAR_EXCHANGED, priv->this);

cleanup:
    _free(msg);

    return ret;
}

static int _send_optional_encryption(struct cyber *this, char *payload, size_t len, int flag_encrypt, void const *attrib);
// 
int initiateChallenge(cyber_priv_t *priv)
{
    // envoyer un Ack de la demande
    char *sndPayload = NULL;
    size_t sndLen = 0;

    int ret = _rsmp_errcod_gen_no_error_;
    
    // afficher les login (debug)
    
        // _log_debug("affichage des login pour Challenge...");
        // auth_to_foreach(priv->sId_Remote, printLogin, priv);

        // recherche premier login 
        #ifdef TEST_ENABLED
        if(!priv->disableChallenge && (!priv->challenge_ctxt || (!priv->challenge_ctxt->running && !priv->challenge_ctxt->toSend)))
        #else
        if(!priv->challenge_ctxt || (!priv->challenge_ctxt->running && !priv->challenge_ctxt->toSend))
        #endif
        {
            // _log_info("Choix d'un login pour Challenge...");
            auth_to_foreach(priv->sId_Remote, searchLoginForChallenge, priv);

        }
    if(!priv->challenge_ctxt || (!priv->challenge_ctxt->toSend && !priv->challenge_ctxt->running))
    {
        // _log_info("no challenge to send");
        
        if(!priv->allClearSent)
         {
            _log_info("ALL CLEAR to send");

            ret = sendAllClear(priv);
        }
        goto cleanup;
    }

    if (priv->challenge_ctxt->toSend)
    {
        // _log_info("sId %s found login %s for challenge", priv->challenge_ctxt->auth->key.sId, priv->challenge_ctxt->auth->key.login);
        _log_dump_info_out(" ", 1, "sId %s found login %s for challenge", priv->challenge_ctxt->auth->key.sId, priv->challenge_ctxt->auth->key.login);

        ret = Challenge_encode(priv->challenge_ctxt, priv->this->cipher, priv->sId_Local, priv->challenge_ctxt->auth->key.login, priv->sId_Remote, &sndPayload, &sndLen);
        if (!_rsmp_err_is_ok(ret))
        {
            _log_err("encode fail");
            goto cleanup;
        }

        // _log_dump_info_out(sndPayload, sndLen, "Challenge");
        // ret = priv->transport->send(priv->transport, sndPayload, sndLen); // \todo attention cela court circuite la file et pb si file pas prete dans la phase exchange
        // static int _send_optional_encryption(struct cyber *this, char *payload, size_t len, int flag_encrypt, void const *attrib)
        ret = _send_optional_encryption(priv->this, sndPayload, sndLen, 0, priv->challenge_ctxt->auth->key.login);

        // _free(sndPayload); \todo attention à la libération à faire une fois l'objet consommé ou sinon recopier le payload à l'insertion
        if (!_rsmp_err_is_ok(ret))
        {
            _log_err("write fail");
            goto cleanup;
        }
        priv->challenge_ctxt->running = true;
        priv->challenge_ctxt->toSend = false;
        // création timer timeout pour la réponse
        launchTimerResponse(priv);
    }

cleanup:
    if (!_rsmp_err_is_ok(ret)) _free(sndPayload);

    return ret;
}
/*

888888888888  88                          88888888ba   88888888888  ,ad8888ba,   88888888888  88  8b           d8  88888888888
     88       88                          88      "8b  88          d8"'    `"8b  88           88  `8b         d8'  88
     88       88                          88      ,8P  88         d8'            88           88   `8b       d8'   88
     88       88,dPPYba,   8b,dPPYba,     88aaaaaa8P'  88aaaaa    88             88aaaaa      88    `8b     d8'    88aaaaa
     88       88P'    "8a  88P'   "Y8     88""""88'    88"""""    88             88"""""      88     `8b   d8'     88"""""
     88       88       88  88             88    `8b    88         Y8,            88           88      `8b d8'      88
     88       88       88  88  888        88     `8b   88          Y8a.    .a8P  88           88       `888'       88
     88       88       88  88  888        88      `8b  88888888888  `"Y8888Y"'   88888888888  88        `8'        88888888888


*/


/**
 * @brief Thread de gestion du bordel...
 * 
 * @param arg 
 * @return void* 
 */
static void *cyber_receive(void *arg)
{
    assert(arg != NULL);
    cyber_t *this = arg;
    cyber_priv_t *priv = this->dptr;
    transport_t *transport = priv->transport;
    HoroDate now;
    HoroDate last_rxed;
    // uint watchdog_period_sec = 60;

    setHorodate(&now);
    setHorodate(&last_rxed);

    int ret = _rsmp_no_error_;

    while (/*transport->isConnected(transport) &&*/ !priv->stop)
    {

        // if(flux->ok(flux, FLUX_INOUT) == 0)
        // {
        //     priv->state = _cyber_off_;
        //     sleep(5);
        //     continue;
        // }
        //-- Check si c'est l'heure d'un envoi de watchdog...

        // setHorodate(&now);
        // if (diffHorodateSec(&now, &last_watchdog)>=watchdog_period_sec)
        // {
        //     //-- send a new one
        //     // rsmp_context_t const *rcontext = priv->context->rsmp;
        //     // rsmp_msg_Watchdog_t *w = rsmp_msg_Watchdog_new(rcontext);
        //     // this->send(this, w, NULL);
        //     // rsmp_msg_Watchdog_free(w);
        //     _log_err("watchdog to send");
        //     setHorodate(&last_watchdog);
        // }

        //-- \todo attente de message en réception
        /*
        int to = rsmp_Watchdog_timeout_get(priv->watchdog);
        if (to<=0)
        {
            if (rsmp_Watchdog_check(this, NULL, priv->watchdog) == _rsmp_errcod_wdog_failure_)
            {
                flux->close(flux);
                priv->state = cyber_change_state(priv->state, _cyber_off_);
                sleep(5);
                continue;
            }
            to = rsmp_Watchdog_timeout_get(priv->watchdog);
        }
        */

        // switch (flux->waitinbuf(flux, 1000))
        // {
        // case -1: //erreur...
        //     flux->close(flux);
        //     priv->state = cyber_change_state(priv->state, _cyber_off_);
        //     sleep(5);
        //     break;
        // case 0: //timeout
        //     break;
        // default:
        // if(transport->isConnected(transport))

        size_t len = 0;
        char *payload;
        ptxt_t *ptxt_list;
        // if(_rsmp_err_is_ok(ret)) _log_info("attente de message (to_sec = %d)", priv->to_sec);
        ret = transport->waitForPayload(transport, &payload, &len, 1000); // timeout de la couche transport figé à 1 sec pour pouvoir arrêter le thread par flag
        if(!priv->stop && _rsmp_err_is_ok(ret))
        {
            setHorodate(&last_rxed);

            void *attrib = NULL;
            // rsmp_messages_ptr ptxt = {.type=NULL}; suppression refs spécifiques à rsmp (external)

            auth_t *auth;
            // ret = rsmp_decode_payload(this->cipher, priv->context->rsmp, payload, &ptxt, &auth);
            // l'utilisation de  rsmp_json_payload2ptxt(context, buf) doit se faire au niveau applicatif (même pas dans external_task)
            // _log_dump_info_in(payload, strlen(payload), " ");
            size_t nb_msg = 0;
            ret = Encrypted_decode(this->cipher, priv->sId_Local, payload, len, &ptxt_list, &nb_msg, &auth);

            if (!_rsmp_err_is_ok(ret))
                _log_err("Aucun cipher décodé !!!! (err=%d)", ret);

            // on passe à l'aiguilleur message par message.
            for (size_t i = 0; i < nb_msg; i++)
            {
                if (ptxt_list[i].class == _cipher_class_internal_)
                {
                    _log_debug("pousser ptxt_list[%d].original dans internal_q", i);
                    queue_msg_t msg_q = {.buf = ptxt_list[i].original, .length = (strlen(ptxt_list[i].original)), .attrib = auth}; // pas besoin de passer le car null
                    priv->internal_q->push(priv->internal_q, &msg_q, __queue_clone_msg__);
                    //-- Stimuler l'autre bout de la queue
                    condition_check(&priv->internal_enabled);
                }
                else if (ptxt_list[i].class == _cipher_class_external_)
                {
                    _log_info("pousser ptxt_list[%d].original dans external_q", i);
                    _log_dump_info(LOG_MARK_DEBUG, ptxt_list[i].original, strlen(ptxt_list[i].original), " ");
                    queue_msg_t msg_q = {.buf = ptxt_list[i].original, .length = (strlen(ptxt_list[i].original)+1), .attrib = auth}; // pas besoin de passer le car null
                    priv->external_q->push(priv->external_q, &msg_q, __queue_clone_msg__);
                    //-- Stimuler l'autre bout de la queue
                    condition_check(&priv->external_enabled);
                }
            }
            _free(ptxt_list);

            /*
                                        if(!_rsmp_err_is_ok(ret)) _rsmp_log_err(ret, "message recu non decode ##########");
                                        // else if(ptxt.type != NULL) _log_err("message recu decode mais type non reconnu ###########");
                                        // if (ret == _rsmp_no_error_)
                                        else
                                        {
                                            //-- execution de la commande associée...
                                            // rsmp_msg_desc_t *desc = rsmp_message_desc_get(ptxt); // \todo déplacer dans la external_task

                                            if (desc->callback != NULL)
                                            {
                                                ret = desc->callback(this, ptxt, attrib);
                                            }
                                        }
                                        if (ptxt.type != NULL)
                                        {
                                            rsmp_message_free(ptxt);
                                            ptxt.type = NULL;
                                        }
                                        if (attrib)
                                        {
                                            assert(0);
                                            // rsmp_crypt_attrib_free(attrib);
                                            attrib = NULL;
                                        }
            */
        } else if(ret == _rsmp_errcod_transport_connection_) {
            _log_err("transport connection closed");
            _stop(priv->this, false);
        }
        else
        {
            setHorodate(&now);

            // sortie sur timeout court, vérifier si le timeout paramétré est atteint
            if (priv->to_sec>=1 && diffHorodateSec(&now, &last_rxed)>=priv->to_sec)
            {
                _log_err("timeout reception message %d sec", priv->to_sec);
                _stop(priv->this, false);
            }
        }
        _free(payload);
    }
    // _log_info("cyber_receive end (transport->isConnected=%d stop=%d)", transport->isConnected(transport) /* renvoit tjr 1 */, priv->stop);
    return (!priv->stop);
}

int cyber_process_internal_msg(cyber_t *this, auth_t *auth_FROM , Message *msg);


/// @brief tâche de traitement des messages INTERNAL
/// @param arg, pointeur sur cyber
/// @return 
static void *cyber_receive_internal(void *arg)
{
    cyber_t *this = arg;
    cyber_priv_t *priv = this->dptr;
    queue_t *internal_q = priv->internal_q;

    _log_info("lancement tache attente d'un internal");

    while(!priv->stop)
    {
        queue_msg_t msg_q;
        //-- Attente de la condition d'ouverture de la communication
        //-- Attente infinie sur la queue de message
        condition_wait(&priv->internal_enabled, 1000000 /*1 sec*/);
        // _log_debug("condition d'attente d'un internal passée");

        //-- C'est bon on extrait le message !
        // \todo si en etat d'init (challenge à envoyer au démarrage), faire une attente aléatoire pour démarrage du premier challenge
        // la situation a éviter et l'envoi de 2 demandes simultanées par chaque peer qui utilisent les mêmes login
        // au pire on attend pour rien
        // un all clear reçu annule toute attente initiale


        if (!internal_q->pull(internal_q, &msg_q, 1 /*1 µs (on a deja attendu dans le condition wait)*/))
        {


            _log_dump_info_in(msg_q.buf, msg_q.length, "reception d'un internal");

            auth_t * auth = msg_q.attrib;
            // auth_t *auth = NULL;
            // if(priv->challenge_ctxt && priv->challenge_ctxt->running) auth = priv->challenge_ctxt->auth;

            char *payload = msg_q.buf;

            size_t len = msg_q.length;


            // deserialisation 
// int deserializeMessage(const char *payload, size_t len, Message **msg) 
            Message *msg = NULL;
            // size_t len = strlen(payload);
            int ret = deserializeMessage(payload, len, &msg);
            if (!_rsmp_err_is_ok(ret) || msg == NULL) {
                _log_err("Failed to deserialize message");
                
            }
            else {
                _log_debug("Message received type: %d", msg->messageType);
                // traitement du message
                ret = cyber_process_internal_msg(this, auth, msg);
                if(!_rsmp_err_is_ok(ret)) {
                    _log_err("Failed to process internal message");
                }

            }
            // libération
            freeMessage(&msg);
            free(msg_q.buf);
            free((char *)msg_q.mark);
        }
        // else
        // {
        //     static int cpt_dbg = 0;
        //     if (cpt_dbg < 10)
        //     {
        //         _log_debug("internal queue empty (display 10 first occurences)");
        //         cpt_dbg++;
        //     }
        // }

        // vérifier si challenge à traiter
        int ret = initiateChallenge(priv);
        if (!_rsmp_err_is_ok(ret))
        {
            _log_err("initiateChallenge failed during exchange");
        }
    } 

    _log_info("cyber_receive_internal end");
    return NULL;
}


int waitForMessage_external(struct cyber *this, char **payload, size_t *len, void **attrib, size_t to_sec)
{
    cyber_priv_t *priv = this->dptr;
    queue_t *external_q = priv->external_q;    //-- Allocation dynamique de msg_q
    queue_msg_t *msg_q = (queue_msg_t *)malloc(sizeof(queue_msg_t));
    if (msg_q == NULL) {
        _log_err("Failed to allocate memory for msg_q");
        return _rsmp_errcod_cyber_generic_;
    }

    (void)attrib;

    HoroDate startWaiting;
    HoroDate now;
    setHorodate(&startWaiting);

    do
    {

        //-- Attente de la condition d'ouverture de la communication
        //-- Attente infinie sur la queue de message
        _log_info("condition d'attente d'un external AVANT"); // \todo blocages constatés avec simulateur Fareco
        condition_wait(&priv->external_enabled, 1000000*to_sec /*1 sec*/);
        _log_info("condition d'attente d'un external passée");

        //-- C'est bon on extrait le message !
        // if (!external_q->pull(external_q, msg_q, 1 /*1 µs (on a deja attendu dans le condition wait)*/))
        if (!external_q->pull(external_q, msg_q, 1 /*1 µs (on a deja attendu dans le condition wait)*/))
        {


            _log_dump_info_in(msg_q->buf, msg_q->length, "reception d'un external");

            // réccupération du contexte pour l'encodage aes
            // *attrib = msg_q.attrib; \todo 

            *payload = msg_q->buf;

            *len = msg_q->length;


            // deserialisation et libération à faire par l'appelant
            
            return _rsmp_errcod_gen_no_error_;
        }

        // _log_info("external queue empty");
        setHorodate(&now);

    } while(!priv->stop && diffHorodateSec(&startWaiting, &now) < to_sec);
    
    _log_info("timeout waiting for external message");
    return _rsmp_errcod_cyber_generic_;
} 

void process_data(char *data) {
    // Insérez ici le code pour traiter les données reçues
    printf("Processing: %s\n", data);
}


/*

88b           d88  88888888888  888888888888  88        88    ,ad8888ba,    88888888ba,     ad88888ba
888b         d888  88                88       88        88   d8"'    `"8b   88      `"8b   d8"     "8b
88`8b       d8'88  88                88       88        88  d8'        `8b  88        `8b  Y8,
88 `8b     d8' 88  88aaaaa           88       88aaaaaaaa88  88          88  88         88  `Y8aaaaa,
88  `8b   d8'  88  88"""""           88       88""""""""88  88          88  88         88    `"""""8b,
88   `8b d8'   88  88                88       88        88  Y8,        ,8P  88         8P          `8b
88    `888'    88  88                88       88        88   Y8a.    .a8P   88      .a8P   Y8a     a8P
88     `8'     88  88888888888       88       88        88    `"Y8888Y"'    88888888Y"'     "Y88888P"


*/
#define check_cyber(this) \
assert(this != NULL);\
assert(this->dptr != NULL);\
cyber_priv_t *priv = this->dptr


static int _start(struct cyber *this)
{
    // \todo start doit vérifier le positionnement de sId avant de démarrer ; ajouter sId en parametre de start (deplacer le code de cyber_new() )
    check_cyber(this);

    transport_t * transport = priv->transport;

    int ret = _rsmp_no_error_;

    if (!priv->sId_Local) {
        _log_err("sId_Local missing");
        return _rsmp_errcod_auth_failing_input_;
    }

    // ouverture du flux client tcp
    // if (!this->is_leader && (flux->open(flux) <= 0 || flux->ok(flux, FLUX_INOUT) == 0))
    // {
    //     _log_errno("openning flux, failed");
    //     sleep(5);
    //     return _rsmp_errcod_cyber_generic_;
    // }

    
    ret = transport->start(transport);

    if(!_rsmp_err_is_ok(ret)) {
        _log_err("transport start failed");
        return ret;
    }

    if(transport->isListener(transport)) {
        _log_info("listener started");

        // accroche nouvelles connexions entrantes
        // il faut notifier la couche cyber afin de créer un nouvel objet cyber pour initier la negociation
        subscribe_event(EVENT_TYPE_INCOMING_CONNECTION, callback_incoming_connection, this); // \todo test plusieurs clients

        return ret;
    } else {
        _log_info("transport for client started");
    }


    //-- Négociation
    // on traite en direct ce qui correspond à des CipherAgreement, une fois l'échange de cipher établi on passe par la tache internal_task

    // difficulté :
    // - faire partir l'échange de version puis le premier watchdog depuis la tache external.
    // - se remettre en situation de négociation CipherAgreement après une perte de com
    //      a) par absence watchdog => niveau supérieur
    //      b) par coupure flux tcp => à minima, notifier le niveau supérieur voir pour réinitier la connexion

    // Négociation du cipher
    if (this->is_leader)
    {
        ret = cyber_exchange_leader(priv);
    }
    else
    {
        ret = cyber_exchange_follower(priv);
    }
    if (!_rsmp_err_is_ok(ret))
    {
        _log_err("error during exchange");
        // \todo check spec : maintient tcp puis coupure de la connexion après 10 secondes
        sleep(priv->inhib_s);
        // flux->close(flux);

        transport->stop(transport, true);

        return ret;
    }

    _log_info("cyber exchange done");

    //-- Démarrage du thread de réception
    priv->receive_task = pthread_new(cyber_receive, NULL);
    pthread_start(priv->receive_task, this);

    //-- Démarrage du thread d'emission
    priv->send_task = pthread_new(cyber_send, NULL);
    pthread_start(priv->send_task, this);
    priv->internal_task = pthread_new(cyber_receive_internal, NULL);
    pthread_start(priv->internal_task, this);


    _log_info("cyber started");
    
    sleep(1); // \todo : à supprimer, attente de la fin de l'initialisation des threads (debug)

    return ret;
}


static int _stop(struct cyber *this, bool force)
{

    check_cyber(this);
    int ret = _rsmp_no_error_;
    if(!priv->stop)
    {
        priv->stop = true;

        if (force)
        {
            _log_info("force stop deconseillée en fonctionnement nominal car ne permet pas de terminer proprement les threads et de libérer les ressources");
        }
        else
        {
            _log_info("stop en cours");
        }

        // chaque consommateur libère ses buffers et file via le flag stop
        // \todo : voir si on peut terminer la fermeture d'un thread par une raz de sa réf pour ne pas faire 2 join (sinon vérifier comportement par rapport à EINVAL et EDEADLK)
        // concurrence fermeture et fermeture de lui-même
        if (priv->transport)
        {
            _log_info("Stopping transport");
            priv->transport->stop(priv->transport, force);
        }
        if (priv->receive_task)
        {
            _log_info("Stopping receive task");
            _pthread_stop(priv->receive_task, force);
        }
        stopTimerResponse(priv->challenge_ctxt);
        if (priv->internal_task)
        {
            _log_info("Stopping internal task");
            _pthread_stop(priv->internal_task, force);
        }
        if (priv->send_task)
        {
            _log_info("Stopping send task");
            _pthread_stop(priv->send_task, force);
        }
        trigger_event(EVENT_TYPE_CYBER_LOST, this);
        _log_info("cyber _stop done");
    }
    return ret;
}


static void _cyber_free(struct cyber *this)
{
    check_cyber(this);

    //-- prise définitve du mutex
    mutex_lock(&priv->mutex);
    //-- arrêt du thread
    this->stop(this, true);
    //-- libération des ressources
    ldcSuppress(&priv->chain);
    cyber_priv_free(priv);
    free(this);
}

int cyber_process_internal_msg(cyber_t *this, auth_t *auth_FROM /* base FROM */, Message *msg)
{
    check_cyber(this);
    int ret = _rsmp_no_error_;
    ret = messageCheckFields(msg);
    if(!_rsmp_err_is_ok(ret)) {
        _log_err("Failed to check message fields");
        ret = _rsmp_errcod_cyber_generic_;
        goto cleanup;
    }
    assert(this != NULL);
    assert(msg != NULL);
    Message *response = NULL;
    switch (msg->messageType)
    {
    case MSG_TYPE_ALLCLEAR:
        _log_info("All clear message received");
        priv->allClearRxed = true;
        if(priv->allClearSent) {
            trigger_event(EVENT_TYPE_ALLCLEAR_EXCHANGED, this);
        }
        break;
    case MSG_TYPE_CHANGE_PWD_REQUEST:
        _log_info("Change password request message received");
        response = initMessage(MSG_TYPE_CHANGE_PWD_RESPONSE);
        if(response == NULL) {
            _log_err("Failed to create Change password response message");
            ret = _rsmp_errcod_cyber_generic_;
            goto cleanup;
        }
        memcpy(response->fields.changePwdResponse.oMId, msg->common.mId, sizeof(msg->common.mId));
        
        

        response->fields.changePwdResponse.status = true;
        if(priv->challenge_ctxt && (priv->challenge_ctxt->toSend || priv->challenge_ctxt->running)) {
            // identifier si un login disponible (sans challenge en cours) pour le chemin retour
            
            auth_t *auth = NULL;
            const auth_key_t key = {0};
            
            auth_to_key_set(&key, NULL /* login à null */, priv->sId_Remote);
            
            // recherche du premier à partir de la clé
            auth = auth_get_first(&key);

            // vérifier si l'authentification est disponible, expliciter le login choisi si NULL
            if((auth && !auth->new_pwd_pending)) { // \todo BUG auth->new_pwd_pending ne veut pas dire challenge en cours sur ce pwd ; deplus factoriser code (cf. _send())
                //login = auth->key.login; // le login choisi n'a pas de Challenge en cours
                _log_info("login %s available for Challenge response", auth->key.login);
            } else if(auth) {
                // rechercher le suivant et tester si disponible (hypothese qu'il n'y a qu'un login en cours de challenge)

                auth = auth_get_next_from_auth(auth);
                if (auth && !auth->new_pwd_pending)
                {
                    _log_info("login %s available for Challenge response", auth->key.login);
                }
            }
               
            // \todo test du busy ; répondre false si AUCUN login disponible pour le chemin retour ; il faudra choisir le premier login qui n'est pas en challenge
            if(!auth || auth->new_pwd_pending) {
                _log_err("Change password busy (auth=%p, new_pwd_pending=%d)", auth, auth?auth->new_pwd_pending:0);
                response->fields.changePwdResponse.status = false; 
            } 

        }

        if(response->fields.changePwdResponse.status){
            // \todo réaliser le changement de mot de passe dans la base de données par passage d'un flag dans la file ou détection du type de message afin de purger les messages en attente devant etre déchiffrés avec l'ancien mot de passe
            // demande de bascule de mot de passe pour : base TO / login
            // auth_t *auth_TO = NULL;
            // ret = coder_auth_get(auth_FROM->key.login, auth_FROM->key.sId, _coder_encrypt_, &auth_TO);
            // if (!_rsmp_err_is_ok(ret))
            // {
            //     _log_err("Erreur coder_auth_get");
            //     goto cleanup;
            // }
            int _ret = auth_set_pwd_b64(auth_FROM, msg->fields.changePwdRequest.pwd);
            if (_rsmp_err_is_ok(_ret))
            {
                // auth_db_serialize(fileno(stdout));
                auth_db_to_file("auth.db"); // \todo faire une accroche dans cyber priv ou/et par paramètre de configuration
                _log_info("mot de passe enregistré pour login %s", auth_FROM->key.login);

                // \todo purge de l'ancienne derivation
                // \todo voir pour intégrer la suppression de l'ancienne derivation dans auth_set_pwd_b64 et switch_to_new_pwd
                auth_secret_delete_by_key(&auth_FROM->key, this->cipher->lib);
                

            } else
            {
                _log_err("Fail setting pw");
                response->fields.changePwdResponse.status = false;
            }
        }
        _log_info("sending response ok");

        // if(this->isLeader(this)) {
        //     // test reprise de challenge du distant ; on doit répondre un Pong
        //     sleep(1);
        //     assert(0);
        // }

        ret = _sendMessage(this, response, NULL); 
        if(!_rsmp_err_is_ok(ret)) {
            _log_err("Failed to send Change password response message");
            ret = _rsmp_errcod_cyber_generic_;
            goto cleanup;
        }
        _log_info("sending response done");

        break;
    case MSG_TYPE_CHANGE_PWD_RESPONSE:
        _log_info("Change password response message received");
        if(priv->challenge_ctxt == NULL || !priv->challenge_ctxt->running) {
            _log_err("Change password response received while no change password running");
            ret = _rsmp_errcod_cyber_generic_;
            goto cleanup;
        }
        
        if(strcmp(msg->fields.changePwdResponse.oMId, priv->challenge_ctxt->mIdChangePwdRequest) != 0) {
            _log_err("oMId mismatch");
            ret = _rsmp_errcod_json_parse_;
            goto cleanup;
        }

        stopTimerResponse(priv->challenge_ctxt);
        if(msg->fields.changePwdResponse.status) {
            priv->challenge_ctxt->response = CHALLENGE_RESPONSE_SUCCESS;
            ret = auth_switch_to_new_pwd(priv->challenge_ctxt->auth);
            if(!_rsmp_err_is_ok(ret))
            {
                _log_err("fail switching new pw to current");
                goto cleanup;
            }
            // purge de l'ancienne derivation
            auth_secret_delete_by_key(&priv->challenge_ctxt->auth->key, priv->this->cipher->lib);

            _log_info("Change password success for login %s", priv->challenge_ctxt->auth->key.login);
            // auth_db_serialize(fileno(stdout));
        } else {
            _log_info("received Change password busy");
            priv->challenge_ctxt->response = CHALLENGE_RESPONSE_BUSY;
            // \todo à ce moment là, on peut décider d'oublier le new_pwd sans risque : oui il faut le faire !
        }
        priv->challenge_ctxt->running = false;
        trigger_event(EVENT_TYPE_CHALLENGE_PROCESSING, this); // \todo voir architecture soft pour challenge. Thread dédié ?
        ret = initiateChallenge(priv);
        if (!_rsmp_err_is_ok(ret))
        {
            _log_err("initiateChallenge failed after exchange");
            goto cleanup;
        }

        break;
    case MSG_TYPE_PING:
        _log_info("Ping message received");
        response = initMessage(MSG_TYPE_PONG);
        if(response == NULL) {
            _log_err("Failed to create Pong message");
            ret = _rsmp_errcod_cyber_generic_;
            goto cleanup;
        }
        // response->fields.pong.oMId
        memcpy(response->fields.pong.oMId, msg->common.mId, sizeof(msg->common.mId));
        response->fields.pong.padding = strdup(msg->fields.ping.padding);
        
        ret = _sendMessage(this, response, NULL);
        if(!_rsmp_err_is_ok(ret)) {
            _log_err("Failed to send Pong message");
            ret = _rsmp_errcod_cyber_generic_;
            goto cleanup;
        }
        
        break;
        
    case MSG_TYPE_PONG:
        _log_info("Pong message received");
        if(priv->challenge_ctxt == NULL || !priv->challenge_ctxt->running) {
            _log_err("Pong received while no challenge running");
            ret = _rsmp_errcod_cyber_generic_;
            goto cleanup;
        }
        
        if(strcmp(msg->fields.pong.oMId, priv->challenge_ctxt->mIdPing) != 0) {
            _log_err("oMId mismatch");
            ret = _rsmp_errcod_json_parse_;
            goto cleanup;
        }

        // vérifier la cohérence du message et du contexte
        // \todo on pourrait également vérifier la clé utilisée pour le décodage du message

        if (priv->challenge_ctxt)
        {
            _log_info("priv->challenge_ctxt->running = %d, ->auth->key.login = %s, .sId = %s", priv->challenge_ctxt->running, priv->challenge_ctxt->auth->key.login, priv->challenge_ctxt->auth->key.sId);
        }

        if(!priv->challenge_ctxt||!priv->challenge_ctxt->running) {
            _log_err("pas de challenge en cours");
            ret = _rsmp_errcod_cyber_generic_;
            goto cleanup;
        }

        // affichage du delta de temps
        HoroDate now;
        setHorodate(&now);
        long delta = diffHorodateMs(&now, &priv->challenge_ctxt->timestamp);
        _log_info("Pong received in %ld ms", delta);
        if(priv->challenge_ctxt->timer_timeout_resp) {
            _log_info("Stopping timer %p timeout response", priv->challenge_ctxt->timer_timeout_resp);
            TimerDelete(priv->challenge_ctxt->timer_timeout_resp);
            priv->challenge_ctxt->timer_timeout_resp = NULL;
        }
        stopTimerResponse(priv->challenge_ctxt);

        ret = auth_switch_to_new_pwd(priv->challenge_ctxt->auth);
        if(!_rsmp_err_is_ok(ret))
        {
            _log_err("fail switching new pw to current");
            goto cleanup;
        }

        // purge de l'ancienne derivation
        auth_secret_delete_by_key(&priv->challenge_ctxt->auth->key, priv->this->cipher->lib);

        priv->challenge_ctxt->running = false;
        priv->challenge_ctxt->response = CHALLENGE_RESPONSE_SUCCESS;
        trigger_event(EVENT_TYPE_CHALLENGE_PROCESSING, this); // \todo archi sw. thread dedié pour challenge ?

        // engager un nouveau challenge
        ret = initiateChallenge(priv);
        if (!_rsmp_err_is_ok(ret))
        {
            _log_err("initiateChallenge failed after exchange");
            goto cleanup;
        }

        break;
    default:
        _log_err("Unknown message type: %d", msg->messageType);
        ret = _rsmp_errcod_cyber_generic_;
        break;
    }

    cleanup:
        freeMessage(&response);
        return ret;
}

static int _send_optional_encryption(struct cyber *this, char *payload, size_t len, int flag_encrypt, void const *attrib)
{
    check_cyber(this);
    assert(payload != NULL);
    assert(len > 0);
    int ret = _rsmp_no_error_;

    // _log_debug("flag_encrypt=%d", flag_encrypt);

    //-- Sérialise le message   
    if (ret == _rsmp_no_error_)
    {
        char *mark = NULL;
        asprintf(&mark, "msg_q.mark toto");

        // queue_msg_t msg_q = {.mark = mark, .buf = payload, .length = len, .flag_encrypt = flag_encrypt, .attrib = attrib};

        // Allocation dynamique de msg_q
        queue_msg_t *msg_q = (queue_msg_t *)calloc(1, sizeof(queue_msg_t));
        if (msg_q == NULL) {
            _log_err("Failed to allocate memory for msg_q");
            return -1;
        }

        // msg_q->cyber = this;
        msg_q->mark = mark;
        msg_q->buf = payload;
        msg_q->length = len;
        msg_q->flag_do_encrypt = flag_encrypt;
        if(attrib) msg_q->attrib = strdup((char*)attrib); // \todo : pb d'identification d'un login retrouvé à la volée (cf. _send())

        // on passe le contexte pour faire l'encodage aes de l'autre côté de la file, juste avant l'envoi
        // attrib peut varier d'un message à l'autre en terme d'authentification d'où l'intérêt de le passer dans la file

        if(flag_encrypt) _log_dump_info(LOG_MARK_DEBUG, payload, len,"PUSH message (login = %s)", (char *)attrib);
        else _log_dump_debug_out(payload, len,"PUSH message (no encryption)" );

        //-- On pousse dans la queue
        // priv->send_q->push(priv->send_q, msg_q, __queue_clone_msg__);
        priv->send_q->push(priv->send_q, msg_q, __queue_direct_msg__);

        //-- Stimuler l'autre bout de la queue
        condition_check(&priv->send_enabled);
    }
    return ret;
}

static bool isAuthToAvailableForLogin(const auth_login_t login, const auth_sId_t sId_remote)
{ 
    if(!login) return false;
    const auth_t *auth;
    // auth_key_t *key = auth_to_key_set(&key, login, sId_remote);
    // \todo remove
    auth_key_t key = { 0};
    auth_to_key_set(&key, login, sId_remote);
    auth = auth_find_by_key(&key);
    
    if (!auth) return false;

    return true;
}

//inline bool isAuthAvailable(const auth_login_t login, const auth_sId_t sId_remote, coder_enum_action_t action)
static int _send(struct cyber *this, const char *payload, size_t len, const char *login)
{
    char * _payload2 = strdup(payload);
    check_cyber(this);
    if(!priv->allClearRxed || !priv->allClearSent || (login && !isAuthToAvailableForLogin(login, priv->sId_Remote))) 
    {
        if(priv->challenge_ctxt) _log_err("Not ready. allClear rxed=%d sent=%d Challenge running=%d (login=%s)", priv->allClearRxed, priv->allClearSent, priv->challenge_ctxt->running, login);
        else _log_err("Not ready. allClear rxed=%d sent=%d (login=%s)", priv->allClearRxed, priv->allClearSent, login);
        
        return _rsmp_errcod_cyber_generic_;
    }

    auth_t *auth = NULL;
    const auth_key_t key = {0};
    
    auth_to_key_set(&key, login, priv->sId_Remote);
    
    // recherche du premier à partir de la clé
    auth = auth_get_first(&key);

    // vérifier si l'authentification est disponible, expliciter le login choisi si NULL
    if(!login) {
        if(!auth) {
            _log_err("No available auth (login = %s)", login);
            return _rsmp_errcod_cyber_generic_;
        }

        if (auth && (!priv->challenge_ctxt || (auth != priv->challenge_ctxt->auth) || (!priv->challenge_ctxt->running && !priv->challenge_ctxt->toSend)))
        {
            login = auth->key.login; // le login choisi n'a pas de Challenge en cours
        } // le suivant si premier en cours de challenge
        else
        {
            _log_err("auth busy (login = %s), recherche d'une autre auth", auth->key.login);
            if (auth)
            {
                auth = auth_get_next_from_auth(auth);
                if (auth && !auth->new_pwd_pending)
                {
                    login = auth->key.login; // le login choisi n'a pas de Challenge en cours
                }
            }
        }

        if (login)
        {
            _log_info("login choisi automatiquement = %s", login);
        }
        else
        {
            _log_err("No available auth (login = %s)", login);
            return _rsmp_errcod_cyber_generic_;
        }
    } else {
        // login positionné par l'external, il faut vérifier qu'il n'a pas un challenge en cours
        _log_info("login choisi par l external = %s", login);

        // est-il disponible ?
        if(!auth || auth->new_pwd_pending) {
            _log_err("login %s not available", login);
            return _rsmp_errcod_cyber_generic_;
        }
    }
assert(strcmp(payload, _payload2) == 0);
    return _send_optional_encryption(this, _payload2, len, 1, login);
}

static int _sendMessage(cyber_t *this, Message *msg, void const *attrib)
{
    int ret = _rsmp_no_error_;
    char *payload = NULL;
    size_t len = 0;

    if (messageCheckFields(msg) != 0)
    {
        _log_err("Failed to check Change password response message fields");
        ret = _rsmp_errcod_cyber_generic_;
        goto cleanup;
    }

    ret = serializeMessage(msg, &payload, &len);
    if(!_rsmp_err_is_ok(ret)) {
        _log_err("Failed to serialize message");
        ret = _rsmp_errcod_cyber_generic_;
        goto cleanup;
    }

    // ret = this->send(this, payload, len, attrib);
    ret = _send_optional_encryption(this, payload, len, 1, attrib);

    if(!_rsmp_err_is_ok(ret)) {
        _log_err("Failed to send message");
        ret = _rsmp_errcod_cyber_generic_;
        goto cleanup;
    }

    cleanup:
        if(!_rsmp_err_is_ok(ret)) {
            _free(payload);
        }
        
        return ret;
}

/**
 * @brief Fermeture sur sortie
 * 
 * Ferme tous les flux ouverts
 * 
 */
void cyber_atexit(void)
{
    while (!ldcIsEmpty(&_cyber_list))
    {
        sDblChain *node=_cyber_list.next;
        ldcSuppress(node);
        cyber_priv_t *priv = cyber_from_lst(node);
        priv->this->free(priv->this);
    }
}

int cyber_setSiteId_local(cyber_t *this, const char *sId)
{
    check_cyber(this);
    if (priv->sId_Local) {
        _log_err("sId_Local already set");
        return _rsmp_errcod_auth_failing_input_;
    }
    priv->sId_Local = strdup(sId);

    return 0;
}

const char * cyber_getSiteId_local(cyber_t *this)
{
    check_cyber(this);
    return priv->sId_Local;
}

int cyber_setSiteId_remote(cyber_t *this, const char *sId)
{
    check_cyber(this);
    if (priv->sId_Remote) {
        _log_err("sId_Remote already set");
        return _rsmp_errcod_auth_failing_input_;
    }
    priv->sId_Remote = strdup(sId);

    return 0;
}

const char * cyber_getSiteId_remote(cyber_t *this)
{
    check_cyber(this);
    return priv->sId_Remote;
}

int cyber_enableRoleLeader(cyber_t *this)
{
    check_cyber(this);
    this->is_leader = true;
    return 0;
}

bool cyber_isLeader(cyber_t *this)
{
    check_cyber(this);
    return this->is_leader;
}

bool cyber_isRunning(cyber_t *this)
{
    check_cyber(this);
    return !priv->stop;
}


bool cyber_isAvailable(cyber_t *this)
{
    check_cyber(this);
    return !priv->stop && priv->allClearRxed && priv->allClearSent && (!priv->challenge_ctxt || (priv->challenge_ctxt && !priv->challenge_ctxt->running));
}

int cyber_initialize(void)
{

    // cyber_context_initialize();
    //-- Mise en place de l'arrêt sur sortie
    atexit(cyber_atexit);

    return 1;
}

// cyber_t *cyber_new(/*rsmp_context_t const *rsmp_context,*/ FLUX *flux, char const *id, char * sId_Local, char * sId_Remote, int is_leader)
// {
//     assert(is_leader || flux != NULL);
//     cyber_t *item = calloc(1, sizeof(*item));
//     if (item)
//     {
//         item->cipher = NULL;
//         item->is_leader = is_leader;
//         cyber_priv_t *priv= cyber_priv_new(flux);
        
//         if ((item->dptr = priv) != NULL)
//         {
//             priv->this = item;

//             //-- initialisation des méthodes
//             item->setSiteId_local = cyber_setSiteId_local;
//             item->setSiteId_remote = cyber_setSiteId_remote;
//             item->getSiteId_local = cyber_getSiteId_local;
//             item->getSiteId_remote = cyber_getSiteId_remote;
//             item->enableRoleLeader = cyber_enableRoleLeader;
//             item->isLeader = cyber_isLeader;
//             item->initialize = cyber_initialize;
//             item->start = _start;
//             item->stop  = _stop;
//             item->send  = _send; // \todo : test
//             item->free  = _cyber_free;
// #ifdef TEST_ENABLED
//             item->disable_encryption = _disable_encryption;
// #endif
//             //-- Ajout dans la liste principale
//             ldcInsert(&_cyber_list, &priv->chain);

//             // assert(priv->context == NULL);
//             // if(id != NULL && rsmp_context != NULL)
//             // {
//             //     priv->context->id = strdup(id);
//             //     priv->context->rsmp = rsmp_context;
//             // } else {
//             //     _log_warn("rsmp context not set");
//             //     priv->context = calloc(1, sizeof(*priv->context));
//             //     priv->context->rsmp = rsmp_context_global_get();
//             //     priv->context->id = strdup("global");
//             // }

//             if(sId_Local) 
//             {
//                 // rsmp_sId_set(&priv->context->rsmp->sId, sId_Local);
//                 priv->sId_Local = strdup(sId_Local);
//             }
            
//             if (sId_Remote) 
//             {
//                 // rsmp_sId_set(&priv->sId_Remote, sId_Remote);
//                 priv->sId_Remote = strdup(sId_Remote);
//                 auth_key_t key;
//                 auth_t *auth = auth_get_first(auth_key_set(&key, _auth_to_dbId, NULL, priv->sId_Remote));
//                 _log_info("cyber, sId From: '%s', login: %s", priv->sId_Remote, auth?auth->key.login:"---");
//             }
//         }
//         else
//         {
//             free(item);
//             item = NULL;
//         }
//     }
//     return item;
// }



// cyber_t *cyber_new_from_transport(/*rsmp_context_t const *rsmp_context,*/ FLUX *flux, char const *id, char * sId_Local, char * sId_Remote, int is_leader)

cyber_t *cyber_new_from_transport(transport_t * transport)
{
    cyber_t *item = calloc(1, sizeof(*item));
    if (item)
    {
        // transport->setCyber(item);
        item->cipher = NULL;
        item->is_leader = false;
        cyber_priv_t *priv= cyber_priv_new(transport);
        
        if ((item->dptr = priv) != NULL)
        {
            priv->this = item;

            //-- initialisation des méthodes
            item->setSiteId_local = cyber_setSiteId_local;
            item->setSiteId_remote = cyber_setSiteId_remote;
            item->getSiteId_local = cyber_getSiteId_local;
            item->getSiteId_remote = cyber_getSiteId_remote;
            item->enableRoleLeader = cyber_enableRoleLeader;
            item->isLeader = cyber_isLeader;
            item->isRunning = cyber_isRunning;
            item->isAvailable = cyber_isAvailable;
            item->initialize = cyber_initialize;
            item->start = _start;
            item->stop  = _stop;
            item->send  = _send;
            item->waitForPayload = waitForMessage_external;
            item->free  = _cyber_free;
#ifdef TEST_ENABLED
            item->disable_encryption = _disable_encryption;
#endif
            // \todo fermeture de cyber depuis transport
            // en cas de coupure de connexion, on arrête le thread de réception
            // il faut pouvoir informer le niveau supérieur (cyber) qui lui même en informera l'applicatif
            // transport->setCallbackCloseParent(transport, item->stop, item);
            // est-ce qu'un traitement de l'erreur de waitForPayload ne suffirait pas pour détecter la coupure de connexion ?
            // tester : forcer la fermeture du flux tcp pour voir si le thread de réception s'arrête au niveau cyber

            //-- Ajout dans la liste principale
            ldcInsert(&_cyber_list, &priv->chain);

            // assert(priv->context == NULL);
            // if(id != NULL && rsmp_context != NULL)
            // {
            //     priv->context->id = strdup(id);
            //     priv->context->rsmp = rsmp_context;
            // } else {
            //     _log_warn("rsmp context not set");
            //     priv->context = calloc(1, sizeof(*priv->context));
            //     priv->context->rsmp = rsmp_context_global_get();
            //     priv->context->id = strdup("global");
            // }

            // if(sId_Local) 
            // {
            //     // rsmp_sId_set(&priv->context->rsmp->sId, sId_Local);
            //     priv->sId_Local = strdup(sId_Local);
            // }
            
            // if (sId_Remote) 
            // {
            //     // rsmp_sId_set(&priv->sId_Remote, sId_Remote);
            //     priv->sId_Remote = strdup(sId_Remote);
            //     auth_key_t key;
            //     auth_t *auth = auth_get_first(auth_key_set(&key, _auth_to_dbId, NULL, priv->sId_Remote));
            //     _log_info("cyber, sId From: '%s', login: %s", priv->sId_Remote, auth?auth->key.login:"---");
            // }
        }
        else
        {
            free(item);
            item = NULL;
        }
    }
    return item;
}


/**@}*/