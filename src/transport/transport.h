#ifndef TRANSPORT_H
#define TRANSPORT_H

#ifdef TRANSPORT_TRANSPORT_C
#define TRANSPORT_EXTERN
#define TRANSPORT_PRIVATE
#endif

#ifndef TRANSPORT_EXTERN
#define TRANSPORT_EXTERN extern
#endif

#ifndef TRANSPORT_PRIVATE
#define TRANSPORT_PRIVATE const
#endif


// callback pour le traitement des nouvelles connexions entrantes
// retourne un nouvel objet transport avec le (nouveau) flux de la connexion entrante
typedef struct transport transport_t;
typedef void (callback_peer_connected_t)(transport_t *transport);

typedef struct transport
{
    TRANSPORT_PRIVATE void *dptr;

    /**
     * @brief Fonction pour configurer l'activation d'un listener
     * 
     * @param[inout]  this le transport lui-même
     * @param[in]     callback fonction de callback pour le traitement des nouvelles connexions entrantes
     */
    int (*enableListener)(struct transport *this, callback_peer_connected_t *callback);

    /**
     * @brief Fonction pour récupérer si le mode listener (serveur de connexion) est activé
     * 
     * @param[inout]  this le transport lui-même
     */
    int (*isListener)(struct transport *this);

    /**
     * @brief Fonction pour démarrer le transport (peer ou listener)
     * 
     * @param[inout]  this le transport lui-même
     * @return ==0 si Ok, code d'erreur sinon
     */
    int (*start)(struct transport *this);

    /**
     * @brief Arrête les threads associés
     * 
     * @param[inout]  this le transport lui-même
     * @param[in]     wait si true, attend la fin des threads
     */
    void (*stop)(struct transport *this, bool wait);
    
    /**
     * @brief Fonction test pour savoir si le transport est connecté
     * 
     * @param[inout]  this le transport lui-même
     * @return ==1 si connecté, 0 sinon
     */
    int (*isConnected)(struct transport *this);

    /**
     * @brief envoi un message sur le flux
     * 
     * Ajoute le caractère de terminaison
     * 
     * \note ne libère pas le message....
     * 
     * @param[inout]  this le transport lui-même
     * @param[in]     payload  message
     * @param[in]     len taille du message
     * @return ==0 si OK, code d'erreur sinon...
     */
    int (*send)(struct transport *this, const char *payload, size_t len);
    
    /**
     * @brief Lecture d'un payload sur le flux
     * 
     * @param[inout]  this le transport lui-même
     * @param[out]    payload message
     * @param[out]    len taille du message
     * @param[in]     to_ms à -1 pour une attente infinie
     * @return ==0 si OK, code d'erreur sinon...
     */
    int (*waitForPayload)(struct transport *this, char **payload, size_t *len, int to_ms);

    /**
     * @brief Libération de l'engin
     * 
     * Ferme tout détruit tout...
     * @param[inout]  this le transport lui-même
     */
    void (*free)(struct transport *this);

} transport_t;

// créer l'objet transport à partir d'une chaine "host:port"
// doit etre suivie d'un appel à la méthode enableListener pour activer le mode serveur
transport_t* transport_new(const char *string);



#endif // TRANSPORT_H