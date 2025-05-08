#include "config.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <signal.h>

#include "log/log.h"
#include "fhs/fhs.h"
#include "fhs/fhs-options.h"

#include "cipher/cipher.h"
#include "cipher/cipher.h"
#include "coder/coder.h"
#include "errcode/errcode.h"

#include "auth/auth-db.h"
// #include "tedi/tedi.h"
#include "flux/flux.h"

#include "misc/thread.h"
#include "cyber/cyber.h"
#include "timer/timer.h"

#include <json-c/json.h>
// \todo include a supprimer pour test unit à l arrache

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#ifdef TEST_ENABLED
#include "test/unitTest.h"
char testingEnabled = 0;
#endif
// #include "rsmp/env/rsmp-context.h"
// #include "rsmp/messages/messages.h"
#include "rsmp/coder/json.h"
#include "rsmp/utils/strings.h"
#include "rsmp/messages/messageack.h"
#include "event/event.h"

//-- Déclaration des options en ligne de commandes

void usage(int fd, char const *argv0, struct option *list)
{
    char const *bname = strrchr(argv0, '/');
    if (bname) 
    {
        bname++;
    }
    else 
    {
        bname=argv0;
    }
    dprintf(fd,"usage %s:\n", bname);
    while (list->name != NULL) {
        switch(list->has_arg)
        {
            case required_argument:
            dprintf(fd, "--%s <arg>", list->name);
            if (isalnum(list->val))
            {
              dprintf(fd, ", -%c <arg>", list->val);  
            }
            break;
            case optional_argument:
            dprintf(fd, "--%s[=<arg>]", list->name);
            if (isalnum(list->val))
            {
              dprintf(fd, ", -%c [<arg>]", list->val);  
            }
            break;
            default:
            dprintf(fd, "--%s", list->name);
            if (isalnum(list->val))
            {
              dprintf(fd, ", -%c", list->val);  
            }
            break;
        }
        dprintf(fd,"\n");
        list++;
    }
}




static int Stop=0;

/**
 * \brief Fonction d'arrêt de l'application
 *
 */
static void fstop(int sig)
{
    (void)sig;
	Stop=1;
	printf("pid:%d, Argh, par Zorg ! Je suis touché !\n", getpid());

}

// static void rsmp_sId_flux_decode(char**sId, char**flux, bool forflux, char const *option)
// {
//     char *__s=strdup(option);
//     if (__s)
//     {
//         char *s = strchr(__s, '@');
//         if (s)
//         {
//             *s++='\0';
//             rsmp_str_set(sId, __s);
//             rsmp_str_set(flux, s); 
//         }
//         else
//         if (forflux)
//         {
//             rsmp_str_set(flux, __s); 
//         }
//         else
//         {
//             rsmp_str_set(sId, __s);
//         }
//         free(__s);
//     }
// }

#define MAX_THREADS 100

pthread_context_t *receive_task;
static void *external_receive(void *arg);

void connected(void *this) {
    cyber_t *cyber = (cyber_t *)this;
    // printf("Callback 2 executed. Private variable value: %d\n", module->private_variable);
    _log_info("%s connected with %s", cyber->getSiteId_local(cyber), cyber->getSiteId_remote(cyber));
}

int sendMessage(cyber_t *cyber, rsmp_messages_ptr msg, const char *login);
int waitForMessage(cyber_t *cyber, rsmp_messages_ptr *msg, void *attrib, size_t to_sec);
void callback_cyber_ready(void *context_srv, void *context_incomming_cyber);
void callback_cyber_lost(void *context_srv, void *context_lost_cyber);


/**
 * @brief Test la communication version en "ligne"
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[])
{
    int daemon=0;
    char *rsmp_options = NULL;
    //char *tedi_fux=NULL;
    char * rsmp_flux=NULL; // de la forme <ip>:<port>
    char * rsmp_sId_Local=NULL;
    char * rsmp_sId_Remote=NULL;
    int rsmp_is_server = 0;
    int disable_challenge_tx = 0;
    int rsmp_server_port = 12111;
    bool disable_encryption = false;

    // afficher les paramettres de la ligne de commande
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
#ifdef TEST_ENABLED
    int testCase = 0;
#endif

	/**
	 * Armement de la sortie
	 */
	signal(SIGTERM, fstop);
	signal(SIGHUP, fstop);
	signal(SIGINT, fstop);

    //-- Mise en place des options de ligne de commande
    struct option long_options[] =
    {
        {.name="help",         .has_arg=no_argument, .flag=NULL,    .val='h'},
        {.name="daemon",       .has_arg=no_argument, .flag=&daemon, .val=1},
        {.name="background",   .has_arg=no_argument, .flag=&daemon, .val=1},
        {.name="verbose",        .has_arg=no_argument, .flag=NULL,    .val='V'},
        {.name="no-daemon",    .has_arg=no_argument, .flag=&daemon, .val=0},
        {.name="foreground",   .has_arg=no_argument,       .flag=&daemon, .val=0},
        {.name="version",      .has_arg=no_argument,       .flag=NULL,    .val='v'},
        {.name="rsmp"      ,   .has_arg=required_argument, .flag=NULL,    .val='r'},
        {.name="rsmp-flux",    .has_arg=required_argument, .flag=NULL,    .val='f'},
        {.name="rsmp-peer",    .has_arg=required_argument, .flag=NULL,    .val='p'},
        {.name="rsmp-sid-local",    .has_arg=required_argument,     .flag=NULL,    .val='l'},
        {.name="rsmp-role-is-Server (leader)",       .has_arg=no_argument,     .flag=NULL,    .val='S'},
        {.name="rsmp-server-port",       .has_arg=required_argument,     .flag=NULL,    .val='P'},
        {.name="cipher-twindow", .has_arg=required_argument, .flag=NULL,    .val='W'},
        // {.name="tedi-flux",    .has_arg=required_argument, .flag=NULL,    .val='t'},
        // {.name="tedi-timeout", .has_arg=required_argument, .flag=NULL,    .val='T'},
        {.name="cipher-list",    .has_arg=no_argument, .flag=NULL,        .val='L'},
        {.name="cipher-disable", .has_arg=required_argument, .flag=NULL,    .val='D'},
        
#ifdef TEST_ENABLED
        {.name="unit-testing-enabled", .has_arg=required_argument, .flag=NULL,  .val='u'},
        {.name="disable-challenge", .has_arg=no_argument, .flag=&disable_challenge_tx,  .val=1},
#endif   
        fhs_opt_declare(),
        {0,0,0,0},
    };


    //-- Décodage de la ligne de commande

    fhs_getenv(); //-- Positionne les option via l'environnement
    while(1)
    {
        int option_index=-1;
        int option = getopt_long(argc, argv, ":hvr:f:t:T:u:LD:dW:p:l:SP:V", long_options, &option_index);
        if (option ==  -1) break;

        if (option_index >=0 && long_options[option_index].flag) continue;
        if (fhs_opt_cli((unsigned int)(option), optarg)) continue;

        switch(option)
        {
            case 'r':
                //-- Concaténation ds option rsmp...
                if (rsmp_options) rsmp_options = string_concat(string_concat(rsmp_options, ","), optarg);
                else              rsmp_options = string_concat(rsmp_options, optarg);
                break;
            case 'f': // flux de la forme <ip>:<port>. Fonctionne en mode serveur ou client
                asprintf(&rsmp_flux, "%s", optarg);
                break;
            case 'p':
                //-- p (peer) Positionnement du nom du site distant
                asprintf(&rsmp_sId_Remote, "%s", optarg);
                break;
            case 'l':
                //-- Positionnement du nom du site local
                asprintf(&rsmp_sId_Local, "%s", optarg);
                break;
            // case 't':
            //     //-- Chaine pour l'ouverture du flux 
            //     tedi_flux_def_set(optarg);
            //     break;
            // case 'T':
            //     tedi_timeout_def_set(atoi(optarg));
            //     break;
            case 'L':
                cipher_list(fileno(stdout));
                return EXIT_SUCCESS;
            case 'W':
                coder_time_windows_set(atoi(optarg));
                return EXIT_SUCCESS;
            case 'D':
                if (!cipher_let_enabled_by_str(optarg, false))
                {
                    fprintf(stderr, "cipher %s unknown!", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'S':
                rsmp_is_server = 1;
                break;
case 'P':
                rsmp_server_port = atoi(optarg);
                break;
            case 'v':
                fprintf(stdout, "%s\n", get_version());
                return EXIT_SUCCESS;
            case 'V':                
                _log_level_set(LOG_LEVEL_ALL);
                break;
            case 'h':
                usage(fileno(stdout), argv[0], long_options);
                return EXIT_SUCCESS;
#ifdef TEST_ENABLED
            case 'u':
                testCase = atoi(optarg);
                if(testCase > 0){
                    testingEnabled = 1;
                }
                break;
            case 'd':
                //-- desactiver la nego et la couche d'encodage en utilisant un cipher fake
                disable_encryption = true;
                break;
#endif
            case '?':
            default:
                usage(fileno(stderr), argv[0], long_options);
                return EXIT_FAILURE;
        }
    }
#ifdef TEST_ENABLED
    if(testingEnabled)
    {
        // ********************************************************************
        // lancement de tests unitaires avec l'option -u <n°>
        // ********************************************************************
        return unitTest(&Stop, testCase);

    }
#endif
    fhs_initialize(FHS_APP);

    pthread_initialize();

	/*** La gestion des "timers lents"  ***/
	TimerInitialize();



    //-- Passage en Deamon si besoin.
    if (daemon)
    {
        //-- Pour plus tard...
    }

    //-- Initialisation
    if (rsmp_options)
        rsmp_context_global_from_str(rsmp_options);
    //-- Positionnement du log dans un fichier ou bien un pipe


    cyber_initialize();

    //-- Log les conditions de démarrage...
    rsmp_context_global_log();

    auth_db_from_file("auth.db");
    auth_db_serialize(fileno(stdout));

    if(!rsmp_flux) {
        asprintf(&rsmp_flux, "127.0.0.1:%d", rsmp_server_port);
    }

    //-- création d'une instance follower
    if(!rsmp_is_server)
    {
        transport_t *transport = transport_new(rsmp_flux);
        cyber_t *cyber = cyber_new_from_transport(transport);
        cyber->setSiteId_local(cyber, rsmp_sId_Local);

        cyber->setSiteId_remote(cyber, rsmp_sId_Remote);        
        
        #ifdef TEST_ENABLED
        if(disable_encryption)
        {
            cyber->disable_encryption(cyber);
        }
    #endif
    
        subscribe_event(EVENT_TYPE_ALLCLEAR_EXCHANGED, callback_cyber_ready, cyber);
        subscribe_event(EVENT_TYPE_CYBER_LOST, callback_cyber_lost, cyber);

#ifdef TEST_ENABLED
    cyber_disableChallenge(cyber, disable_challenge_tx);
#endif
        int ret = cyber->start(cyber);

        if (!_rsmp_err_is_ok(ret))
        {
            _log_err("failed to connect to the server (ret = %d)", ret);
            Stop = 1;
        }

        _log_info("waiting cyber layer ready...");
        while(!Stop && !cyber->isAvailable(cyber))
        {
            sleep(1);
        }

        if(!Stop)
        {
            _log_info("couche applicative client prete");
            _log_info("#############################################");
            _log_info("#############################################");
            _log_info("#############################################");
        }


        while(!Stop) sleep(1);

        _log_info("stopping client");
        cyber->stop(cyber, false);
        if (receive_task)
        {
            _log_info("Stopping external receive task");
            _pthread_stop(receive_task, false);
        }
        _log_info("client stopped");
        cyber->free(cyber);
    }
    else // rsmp_is_server (leader) :
    {
        // démarrer un serveur
        // cas 2 (cas creation listener) :
        transport_t *transport = transport_new(rsmp_flux);
        // transport->setTimeout_ms(transport, 10000); \todo timeout parametrable
        
        transport->enableListener(transport, NULL /*fct_callback_connected*/);
        cyber_t *server_leaders = cyber_new_from_transport(transport);
        server_leaders->setSiteId_local(server_leaders, rsmp_sId_Local);
        server_leaders->enableRoleLeader(server_leaders);

        subscribe_event(EVENT_TYPE_ALLCLEAR_EXCHANGED, callback_cyber_ready, server_leaders);
        subscribe_event(EVENT_TYPE_CYBER_LOST, callback_cyber_lost, server_leaders);
        
#ifdef TEST_ENABLED
    cyber_disableChallenge(server_leaders, disable_challenge_tx);
#endif
        int ret = server_leaders->start(server_leaders);


        // \todo garder les références de toutes les thread receive_task pour les arrêter proprement
        // \todo arreter proprement les différentes instances de cyber (tester avec minimum 2 clients en face)
    }

    while(!Stop) sleep(1);

    return 0;
}


/**
 * @brief Thread de gestion du bordel...
 * 
 * @param arg 
 * @return void* 
 */
static void *external_receive(void *arg)
{
    assert(arg != NULL);
    cyber_t *cyber = arg;

    HoroDate now;
    HoroDate last_rxed;
    HoroDate last_sent;
    uint watchdog_period_sec = 60;
    uint tolerance_sec = 30;
    

    setHorodate(&now);
    setHorodate(&last_rxed);
    setHorodate(&last_sent);

    // forcer l'envoi d'un premier watchdog
    addSecond(&last_sent, (0-(long)watchdog_period_sec));



    _log_info("receive_external start ##################################################################################################################################################################################################################################################################################");


    while (cyber->isRunning && !Stop)
    {


        int ret = _rsmp_no_error_;
        int to_sec = 1; // \todo timeout de reception de message
        
        if(!Stop && cyber->isRunning(cyber)){ 
            _log_debug("client running... (%d)", Stop);
            _log_info("client running... (%d)", Stop);

            // envoi watchdog toutes les minutes            
            setHorodate(&now);
            _log_info("diffHorodateSec(&now, &last_sent) = %ld", diffHorodateSec(&now, &last_sent));
            if (diffHorodateSec(&now, &last_sent)>=(watchdog_period_sec))
            {
                // création d'un message Watchdog
                rsmp_messages_ptr msg = rsmp_message_new(NULL, _rsmp_msg_Watchdog_);

                int ret = sendMessage(cyber, msg, NULL);
                if(!_rsmp_err_is_ok(ret))
                {
                    _log_err("failed to send the message");
                }
                rsmp_message_free(msg);

                setHorodate(&last_sent);
            } 
            to_sec = watchdog_period_sec - diffHorodateSec(&now, &last_sent);
            if(to_sec < 1) to_sec = 1;
        }

        if (_rsmp_err_is_ok(ret))
            _log_info("attente de message (to_sec = %d)", to_sec);
            
        void *attrib = NULL;
        rsmp_messages_ptr msg = {0};
        ret = waitForMessage(cyber, &msg, &attrib, to_sec); // \todo utiliser waitForMessage
        if (cyber->isRunning && _rsmp_err_is_ok(ret))       // ne pas traiter le message si en phase d'arrêt
        {

            if (msg.type == NULL)
            {
                _log_err("failed to decode the message");
                goto cleanup;
            }

            // traitement du cas particulier du watchdog \todo à voir si doit etre déplacé dans _exe
            if (rsmp_message_is_Watchdog(msg))
            {
                _log_info("Watchdog received");
                setHorodate(&last_rxed);

                rsmp_messages_ptr response = {0};
                response.MessageAck = rsmp_msg_MessageAck_to(msg); // répondre toujours ack 

                ret = sendMessage(cyber, response, NULL);
                // rsmp_message_free(msg);
                goto cleanup;
            }


            //-- execution de la commande associée...
            const rsmp_msg_desc_t *desc = rsmp_message_desc_get(msg); // \todo déplacer dans la external_task

            if (desc->callback != NULL)
            {
                // typedef int (*rsmp_msg_callback_t)(rsmp_engine_t *engine, const void *attrib, void *context, rsmp_messages_ptr msg);
                ret = desc->callback(cyber, (const void *)(attrib), NULL /* context */, msg);
            }

            _log_info("login = %s", (char *)attrib);

        cleanup:
            if (msg.type != NULL)
                rsmp_message_free(msg);
        }
        else // peut être un timeout
        {
            setHorodate(&now);

            // sortie sur timeout court, vérifier si le timeout paramétré est atteint
            if (watchdog_period_sec>=1 && diffHorodateSec(&now, &last_rxed)>=(watchdog_period_sec + tolerance_sec)) // \todo timeout tolerance paramétrable
            {
                _log_err("timeout reception message watchdog %ld sec", to_sec);
                cyber->stop(cyber, false);
            }
        }


        
    }
    _log_err("receive_external end (Stop = %d cyber->isRunning = %d)", Stop, cyber->isRunning?1:0);
}

int waitForMessage(cyber_t *cyber, rsmp_messages_ptr *msg, void *attrib, size_t to_sec)
{
    int ret = _rsmp_errcod_gen_no_error_;

    size_t len = 0;
    char *payload = NULL;
    ret = cyber->waitForPayload(cyber, &payload, &len, attrib, to_sec); // timeout de la couche transport figé à 1 sec pour pouvoir arrêter le thread par flag
    if (!_rsmp_err_is_ok(ret))
    {
        _log_err("failed to receive the message (including timeout)");
        return _rsmp_errcod_cyber_generic_;
    }

    _log_dump_info(LOG_MARK_DEBUG, payload, len, "message recu. Déserialisation...");

    rsmp_context_t const *rsmp_context = rsmp_context_global_get();
    *msg = rsmp_json_payload2msg(rsmp_context, payload);

    if ((*msg).type == NULL)
    {
        _log_err("failed to decode the message");
        _free(payload);
        return _rsmp_errcod_cyber_generic_;
    }

    return ret;
}

int sendMessage(cyber_t *cyber, rsmp_messages_ptr msg, const char *login)
{
    const char *payload = rsmp_json_msg2payload(msg);
    int ret = cyber->send(cyber, payload, strlen(payload), login);
    if (!_rsmp_err_is_ok(ret))
    {
        _log_err("failed to send the message");
        free((char *)payload);
    }
    else
    {
        _log_dump_info(LOG_MARK_DEBUG, payload, strlen(payload), "message sent"); // \todo check: en principe, payload libéré à l'autre bout de la file d'émission
    }
    return ret;
}

int sendVersionAndWaitAck(cyber_t *cyber)
{
    // envoi version
    rsmp_messages_ptr msgToSend = rsmp_message_new(NULL, _rsmp_msg_Version_);

    int ret = sendMessage(cyber, msgToSend, NULL);

    rsmp_message_free(msgToSend);

    rsmp_messages_ptr msgRxed = {0};
    void *attrib;
    ret = waitForMessage(cyber, &msgRxed, &attrib, 30); // \todo delais configurable attente Version/Ack
    if(_rsmp_err_is_ok(ret))
    {
        if (!rsmp_message_is_MessageAck(msgRxed))
        {
            _log_errno("Version Ack not received (received other type : %d)", *msgRxed.type);
            rsmp_message_free(msgRxed);
            return _rsmp_errcod_cyber_generic_;
        }
        _log_info("Version Ack received");
        rsmp_message_free(msgRxed);
        return ret;
    }
    else {
        _log_err("failed to receive the Ack for Version");
        return ret;
    }
}

int waitVersionAndSendAck(cyber_t *cyber)
{
    
    // attente ack Version
    size_t to_sec = 30; // \todo delais configurable attente Version/Ack
    int ret = _rsmp_errcod_gen_no_error_;
    if (_rsmp_err_is_ok(ret))
        _log_info("attente de message (to_sec = %ld)", to_sec);
    // ret = transport->waitForMessage(transport, &payload, &len, 1000); // timeout de la couche transport figé à 1 sec pour pouvoir arrêter le thread par flag

    void *attrib = NULL;
    rsmp_messages_ptr msgRxed = {0};
    rsmp_messages_ptr msgToSend = {0};
    ret = waitForMessage(cyber, &msgRxed, attrib, to_sec);
    if (cyber->isRunning && _rsmp_err_is_ok(ret))                        // ne pas traiter le message si en phase d'arrêt
    {
        if(!rsmp_message_is_Version(msgRxed))
        {
            _log_errno("Version not received (received other type : %d)", *msgRxed.type);
            rsmp_message_free(msgRxed);
            return _rsmp_errcod_cyber_generic_;
        }
        // _rsmp_log_err(ret, "message recu. Déserialisation...");
        
        msgToSend.MessageAck = rsmp_msg_MessageAck_to(msgRxed); // \todo verif element type est il positionné ?
        rsmp_message_free(msgRxed);

        ret = sendMessage(cyber, msgToSend, NULL);
        if (!_rsmp_err_is_ok(ret))
        {
            _log_err("failed to send Version Ack");
        }
        rsmp_message_free(msgToSend);
    }

    return ret;
}

int echange_versions(cyber_t *cyber) // \todo manque le passage d'un contexte rsmp et peut etre d'une structure de gestion du moteur rsmp
{
    int ret = _rsmp_errcod_gen_no_error_;
    if(cyber->isLeader(cyber)) {
        if(!_rsmp_err_is_ok(ret = waitVersionAndSendAck(cyber))) return ret;
        if(!_rsmp_err_is_ok(ret = sendVersionAndWaitAck(cyber))) return ret;
    } else
    {
        if(!_rsmp_err_is_ok(ret = sendVersionAndWaitAck(cyber))) return ret;
        if(!_rsmp_err_is_ok(ret = waitVersionAndSendAck(cyber))) return ret;
    }

    return ret;
}

void callback_cyber_ready(void *context_srv, void *context_incomming_cyber)
{

    cyber_t *cyber_incoming = context_incomming_cyber;
    cyber_t *cyber = context_incomming_cyber;
    
    // cyber_t *cyber_srv_ctx = context_srv;
    (void) context_srv;


    _log_info("AllClear done with %s", cyber_incoming->getSiteId_remote(cyber_incoming));

    // échange des Versions
    int ret = echange_versions(cyber);

    if (!_rsmp_err_is_ok(ret))
    {
        _log_err("failed to exchange (ret = %d)", ret);
        return;
    }

    receive_task = pthread_new(external_receive, NULL);
    pthread_start(receive_task, cyber); // \todo un peu bancal, cette structure de code ne permet le traitement que d'une instance cyber
}   


void callback_cyber_lost(void *context_srv, void *context_lost_cyber)
{

    cyber_t *cyber_lost = context_lost_cyber;
    
    // cyber_t *cyber_srv_ctx = context_srv;
    (void) context_srv;

    _log_info("Link closed with %s", cyber_lost->getSiteId_remote(cyber_lost));

}   