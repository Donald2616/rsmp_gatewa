
#define TRANSPORT_TRANSPORT_C

#include "config.h"

#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "misc/thread.h"
#include "transport/transport.h"
#include "flux/flux.h"
#include "errcode/errcode.h"
#include "log/log.h"
#include "event/event.h"
#include "horodate/horodate.h"

#define MAX_THREADS 100



static const char RSMP_EOT[2]="\x0C";
static const size_t RSMP_EOT_LEN=(sizeof(RSMP_EOT)-1);

typedef struct transport_priv {
    FLUX *          flux;
    char *          host;
    int             port;
    // \todo singleton pour le serveur (listener) si meme port / host
    
    int             is_listener;
    pthread_context_t *listen_task;
    // void *       cyber; // cyber_t
    callback_peer_connected_t *callback;
    bool            stop;
} transport_priv_t;

#define check_transport(this) \
assert(this != NULL);\
assert(this->dptr != NULL);\
transport_priv_t *priv = this->dptr


typedef struct {
    int socket;
    // cyber_t *listener;
} thread_args_t;

void *handle_connection(void *args);

int transport_listen(transport_t *transport)
{
    check_transport(transport);
    // cyber_t *server_leaders = cyber_new(NULL, NULL, rsmp_sId_Local, rsmp_sId_Remote, rsmp_is_listener);

    int server_fd;
    struct sockaddr_in address;

    int addrlen = sizeof(address);

    // Création du socket du serveur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        return _rsmp_errcod_transport_listener_;
    }

    // Attache le socket à l'adresse et au port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(priv->port);

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        return _rsmp_errcod_transport_listener_;
    }
    if (listen(server_fd, 5 /* MAX_CLIENTS */) < 0)
    {
        perror("listen failed");
        return _rsmp_errcod_transport_listener_;
    }

    struct pollfd fds[1];
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    int timeout_msecs = 1000;
    pthread_t threads[MAX_THREADS];
    int thread_count = 0;

    while (!priv->stop)
    {
        _log_debug("\nWaiting for a connection...\n");
        int ret = poll(fds, 1, timeout_msecs);
        if (ret == -1)
        {
            perror("poll"); /* an error occurred */
        }
        else if (ret == 0)
        {
            // timeout, no incoming connections
            continue;
        }
        else
        {
            if (fds[0].revents & POLLIN)
            {
                // there is an incoming connection, call accept()
                int new_socket;
                if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
                {
                    _log_errno("accept failed");
                    return _rsmp_errcod_transport_listener_;
                }

                if (thread_count >= MAX_THREADS)
                {
                    _log_err("Reached maximum number of threads\n");
                    close(new_socket);
                    continue;
                }

                thread_args_t *args = malloc(sizeof(thread_args_t));
                if (args == NULL)
                {
                    _log_errno("Failed to allocate memory for thread arguments");
                    close(new_socket);
                    continue;
                }
                args->socket = new_socket;
                // args->listener = priv->cyber;

                if (pthread_create(&threads[thread_count++], NULL, handle_connection, (void *)args) != 0)
                {
                    _log_errno("Failed to create thread");
                    free(args);
                    close(new_socket);
                    continue;
                }
            }
        }
    }
    return 0;
}

int transport_enable_server(transport_t *transport, callback_peer_connected_t *callback) // \todo positionnement d'une callback pour la cyber
{
    check_transport(transport);
    priv->is_listener = 1;
    priv->callback = callback;
    return 0;
}

int transport_is_listener(transport_t *transport)
{
    check_transport(transport);
    return priv->is_listener;
}


int transport_start(transport_t *transport) {
    check_transport(transport);
    // cas simple : ouverture d'un flux pour un client
    FLUX *flux = priv->flux;

    // if(!priv->cyber)
    // {
    //     _log_err("cyber not set");
    // }
    
    // démarrage des comunications en tant que client ou serveur
    if(!priv->is_listener)
    {   // client
        // Ouverture du flux pour un client ou connexion entrante du serveur
        // dans le cas d'une connexion entrante, le flux est déjà créé
        if(flux == NULL && priv->host != NULL && priv->port != 0) 
        {
            char * flux_str = NULL;
            asprintf(&flux_str, "Socket:tcp:%s:%d", priv->host, priv->port);
            flux = fluxNew(flux_str);
            free(flux_str);
        } 

        if(flux == NULL) {
            _log_err("flux is NULL");
            return _rsmp_errcod_transport_generic_;
        }
        priv->flux = flux;
/*
        if(flux->open(flux) <= 0 || flux->ok(flux, FLUX_INOUT) == 0) {
            _log_errno("openning flux, failed");
            sleep(5);
            return _rsmp_errcod_transport_generic_;
        }*/
        
        if(flux->open(flux) <= 0) {
            _log_errno("openning flux, failed");
            sleep(5);
            return _rsmp_errcod_transport_generic_;
        }
        
        if( flux->ok(flux, FLUX_INOUT) == 0) {
            _log_errno("check flux, failed");
            sleep(5);
            return _rsmp_errcod_transport_generic_;
        }

    } else 
    { // serveur
    #warning "le cast est il ok"
        priv->listen_task = pthread_new((pthread_routine_t) transport_listen, NULL);
        pthread_start(priv->listen_task, transport);
        
    }

    return _rsmp_no_error_;
}




/**
 * @ingroup flux
 * @{
 */

inline static void *bufrealloc(char *buf, size_t new_size, size_t *max, size_t offset)
{
    if (new_size >= *max)
    {
        *max += offset;
        char * s = realloc(buf, (*max)*sizeof(*buf));
        if (s) buf = s;
    }
    return buf;
}

int fluxreadterm_with_realloc(FLUX *flux, char **buf, size_t *len, int to, char const *term, size_t after)
{
    size_t offset=128;
    size_t lenmax = offset;
    size_t i=0;
    *buf = calloc(lenmax, sizeof(**buf));
    assert(*buf != NULL);
    char c;
    int r;
    // while(1) \todo test revoir gestion attente read
    
    HoroDate now;
    HoroDate last_rxed;
    setHorodate(&last_rxed);

    while(flux->ok(flux, FLUX_IN))
    {
        r=flux->read(flux, &c, 1, to);
        if (r == 1)
        {
            *buf = bufrealloc(*buf, i, &lenmax, offset);
            (*buf)[i++]=c;
            if (strchr(term, c) != NULL) break; 
        }
        else
        {
            
            setHorodate(&now);
            static int count = 0;
            // sortie sur timeout court, vérifier si le timeout paramétré est atteint
            // if (diffHorodateMs(&now, &last_rxed)<10 && count<10)
            // {
            //     _log_err("suspicion boucle infinie lors coupure peer"); // \todo fix boucle infinie sur coupure peer
            //     count++;
            // }

            
            // if ((diffHorodateMs(&now, &last_rxed)<10 && count<10) || (diffHorodateMs(&now, &last_rxed)>=10))
            // {
            //     _log_info("Erreur r:%d (to = %d)", r, to);
            // }


            setHorodate(&last_rxed);

            break;
        }
    }
    if (r>=0)
    {
        *len = i;
        if (after)
        {
            *buf = bufrealloc(*buf, i+after, &lenmax, after);

            r = flux->read(flux, *buf + i, after, to);

        }
    }
    return r;
}

/// @brief Lecture d'un payload sur le flux
/// @param[in] flux
/// @param[out] payload
/// @param[in] to_ms à -1 pour une attente infinie
static inline int transport_payload_read(transport_t *transport, char **payload, size_t *len_out, int to_ms)
{
    check_transport(transport);
    
    // _log_info("flux addr = %p", priv->flux);
    FLUX *flux = priv->flux;
    assert(len_out);

    size_t len;

    int ret = _rsmp_no_error_;
    len = 0;
    int nb = fluxreadterm_with_realloc(flux, payload, &len, to_ms, RSMP_EOT, 0);

    if (len)
    {
        _log_dump_info_in(*payload, len, " %s", nb > 0?"succeed":"failed");
    }
    if (nb == -1)
    {
        return _rsmp_errcod_transport_connection_; // connexion tcp perdue (coupée par le peer...)
    } else if (nb <= 0){
        return _rsmp_errcod_cyber_generic_; // -2 si timeout
    }

    (*payload)[len-RSMP_EOT_LEN] = '\0'; // suppression du RSMP_EOT 

    *len_out = len-RSMP_EOT_LEN;

    return ret;
}

static inline int transport_is_connected(transport_t *transport)
{
    check_transport(transport);
    _log_info("flux addr = %p ok = %d (0 si connecté)", priv->flux, priv->flux->ok(priv->flux, FLUX_INOUT)); // \todo à supprimer

    // int ret = (priv->flux->ok(priv->flux, FLUX_INOUT) != 0);

    // if(!ret) {
    //     _log_err("flux is not connected, trying to reconnect...");
    //     // tentative de rattrapage (test debug) \todo a supprimer
    //     ret = (priv->flux->open(priv->flux)>0 && priv->flux->ok(priv->flux, FLUX_INOUT) != 0);
    // }
    // if(!ret) {
    //     _log_err("flux is not connected");
    // }
    // return ret;

    
    // _log_err("!!!!!!!!!!!!!!!!!!!!!!"); // \todo à supprimer
    // fflush(stdout);
    // fflush(stderr);
    // assert(0);
    return 1;
}

/// @brief Envoi d'un payload (chiffré ou pas) sur le flux
/// @note Ajoute le caractère de terminaison RSMP_EOT
/// @param flux 
/// @param payload 
/// @return code retour rsmp
static inline int transport_payload_write(transport_t * transport, const char *payload, size_t len_in)
{
    check_transport(transport);
    FLUX *flux = priv->flux;
    if(flux == NULL) {
        _log_err("flux is NULL");
        return _rsmp_errcod_family_transport_;
    }

    int ret = _rsmp_no_error_;
    ssize_t len_out;

    int _len = len_in;

    char *_buf = calloc(_len+RSMP_EOT_LEN, sizeof(char));
    if (!_buf)
    {
        return _rsmp_errcod_cyber_generic_;
    }
    memcpy(_buf, payload, _len);
    _log_dump_info_out(_buf, _len, " ");
    memcpy(_buf+_len, RSMP_EOT, RSMP_EOT_LEN);
    _len += RSMP_EOT_LEN;


    //-- écriture sur le flux du payload
    if ((len_out = flux->write(flux, _buf, _len, -1)) > 0 && ((size_t)len_out == _len))
    {
        // _log_dump_info_out(_buf, _len, " ");
    } else {
        ret = _rsmp_errcod_cyber_flux_;
        _rsmp_log_errno(ret,"");
    }
    free(_buf);
    /**
     * @todo un choix à faire, souviens-toi de la plugfest
     * 
     */
    return ret;
}


// Fonction pour fermer le transport
void transport_stop(struct transport *this, bool wait) {
    
    check_transport(this);

    priv->stop = true;
    priv->flux->close(priv->flux);
}


// Fonction pour libérer la mémoire allouée pour le transport
void transport_free(transport_t *transport) {
    
    if (transport != NULL) {
        free(transport);
    }
}

transport_priv_t * transport_priv_new(void) {
    transport_priv_t *priv = (transport_priv_t *)calloc(1, sizeof(transport_priv_t));
    if (priv) {
        priv->flux = NULL;
        priv->host = NULL;
        priv->port = 0;
        priv->is_listener = 0;
        priv->listen_task = NULL;
        priv->callback = NULL;
        priv->stop = false;
    }
    return priv;
}
// Fonction pour initialiser le transport
// méthode privée utilisée pour les connexions entrantes ou pour initialiser l'objet dans tous les cas
transport_t *transport_new_from_flux(FLUX *flux) {

    transport_t *item = (transport_t *)calloc(1, sizeof(transport_t));
    if (item) {
        transport_priv_t *priv = transport_priv_new();

        if (priv) {
            item->dptr = priv;
            item->enableListener = transport_enable_server;
            item->isListener = transport_is_listener;
            item->start = transport_start;
            item->stop = transport_stop;
            item->isConnected = transport_is_connected;
            item->send = transport_payload_write;
            item->waitForPayload = transport_payload_read;
            item->free = transport_free;

            priv->flux = flux;
        } else {
            free(item);
            item = NULL;
        }
    }

    return item;
}

void *handle_connection(void *args) {
    thread_args_t *thread_args = (thread_args_t *)args;
    int new_socket = thread_args->socket;
    // cyber_t *listener = thread_args->listener;

    FLUX *flux = fluxNewSocketShadow(new_socket);
    if(flux == NULL) {
        _log_err("Failed to create flux for new connection");
        goto cleanup;
    }

    transport_t *transport = transport_new_from_flux(flux);



    // créer un cyber pour la nouvelle connexion
    // cyber_t *cyber = cyber_new(flux, NULL, listener->getSiteId_local(listener), NULL /* remote */, listener->isLeader(listener));
    // \todo callback sur l'objet cyber parent qui fera le start du nouveau cyber

// #ifdef TEST_ENABLED
//     if (disable_encryption) {
//         cyber->disable_encryption(cyber);
//     }
// #endif
//     cyber->start(cyber);
//     while (!Stop)
//         pause();
//     cyber->stop(cyber, true);


    // passer le relais à la couche cyber afin qu'elle crée un nouvel objet cyber pour réaliser la négociation
    trigger_event(EVENT_TYPE_INCOMING_CONNECTION, transport);

cleanup:
    close(new_socket);
    free(thread_args);
    pthread_exit(NULL);
}

// Fonction pour initialiser le transport
transport_t *transport_new(const char *config_str) {
    transport_t *item = transport_new_from_flux(NULL);
    check_transport(item);

    // parse config_str
    if (config_str)
    {
        char *config_copy = strdup(config_str);
        if (config_copy)
        {
            char *s = strchr(config_copy, ':');
            if (s)
            {
                *s++ = '\0'; // remplace ':' par '\0' pour séparer host et port en 2 chaines
                priv->host = strdup(config_copy);

                errno = 0;
                char *endptr;
                long port = strtol(s, &endptr, 10);
                if (errno != 0 || *endptr != '\0' || s == endptr || port < 1000 || port > 65535)
                {
                    _log_debug("Failed to parse correct port number");
                    priv->port = 0;
                }
                else
                {
                    priv->port = (int)port;
                }
            }
            else
            {
                priv->host = strdup(config_copy);
                priv->port = 0; // Default port if not specified
            }
            free(config_copy);
        }
    }

    return item;
}




