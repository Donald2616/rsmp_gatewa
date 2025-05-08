/**
* @file cyber.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-06-27
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef RSMP_CYBER_CYBER_H
#define RSMP_CYBER_CYBER_H

#include <stdbool.h>

#include "flux/flux.h"

#include "cipher/cipher.h"
#include "transport/transport.h"

/**
 * @page cyber RSMP Engine: Mécanisme de communication
 * 
 * \attention il s'agit d'un engin de communication pour équipement,
 *            __pas pour un superviseur__
 * 
 * ## Rôle
 * Le module cyber met en place les mécanismes de communication asynchrones
 * pour transporter les messages RSMP vers un pair définie.
 * 
 * Asynchrone car il décorrelle la réception et l'émission des messages.
 * 
 * Les données amont sont des messages RSMP
 * 
 * Les mesages aval sont des trames de texte représentant l'encodage et/ou
 * le chiffrement des données transmises. Ces transformations ne sont pas 
 * réalisées par ce module mais déléguées au module coder.
 * 
 * ## Organisation des transmissions
 * L'cyber est à l'origine de l'ouverture de la communication sur un flux
 * prédéfinie.
 * 
 * Dès que la communcation RSMP est établie, les émissions et réceptions de 
 * messages sont autorisées.
 * 
 * La communication est séparée en deux threads :
 * 
 * - receive qui est en charge
 *  - de la gestion de la communication sur le flux duplex (ouverture, 
 *    initialisation, surveillance, fermeture sur demande)
 *  - de la réception des messages.
 * - send qui est en charge de l'émission asynchrone des message sur le flux 
 *   de communication (dès que possible)...
 * 
 * ### receive: mécanisme de réception
 * Le thread de réception :
 * - établit la communication : ouverture du flux
 * - négocie les versions RSMP
 * - lance la gestion des watchdog
 * - déclenche l'évacuation des messages en attente
 * - autorise l'emission
 * - passe en attente de message sur le flux
 *  - sur réception de message, décodage et lance les callback associé au type de message
 * 
 * ### send: mécanisme d'émission
 * 
 * L'émission de mesasges est asynchrone, les utilisateurs (appelants) déposent
 * les messages dans une queue qui est vidée au fur et à mesure par un thread 
 * dédié sur surveille aussi l'état de la communication.
 * 
 */

/**
 * @defgroup cyber Engine
 * 
 * @brief Ensemble des mécanismes permettant la communication RSMP
 * 
 * La communication RSMP est fullduplex sur un flux de type TCP/IP par défaut
 * mais peut être instancier sur l'importe quel flux communication avec un 
 * access asynchrone et définissant une session entr deux locuteurs.
 * 
 * Exemple :
 * - TCP/IP de base, UDP plus tard
 * - liaison série point à point fullduplex RS234 ou RS422
 * 
 * Aujourd'hui pour les équipements, le flux de communication est 
 * modélisé via la bibliothèque flux.
 * 
 * L'équipement est à l'origine de l'ouverture de la communication.
 *
 * @ingroup rsmp
 * @{
 */
#ifdef CYBER_CYBER_C
#define CYBER_EXTERN
#define CYBER_PRIVATE
#endif

#ifndef CYBER_EXTERN
#define CYBER_EXTERN extern
#endif

#ifndef CYBER_PRIVATE
#define CYBER_PRIVATE const
#endif



typedef struct cyber
{
    CYBER_PRIVATE void *dptr;
    CYBER_PRIVATE bool is_leader;
    CYBER_PRIVATE const cipher_t *cipher;


    int (*setSiteId_local)(struct cyber *this, const char *sId);
    
    int (*setSiteId_remote)(struct cyber *this, const char *sId);

    const char *(*getSiteId_local)(struct cyber *this);

    const char *(*getSiteId_remote)(struct cyber *this);

    int (*enableRoleLeader)(struct cyber *this);

    bool (*isLeader)(struct cyber *this);

    bool (*isRunning)(struct cyber *this);

    bool (*isAvailable)(struct cyber *this);
    
    int (*initialize)(void);

    /**
     * @brief Démarre les threads associés
     * 
     * @param[inout]  this l'engin lui-même
     * @return ==0 si Ok, code d'erreur sinon
     */
    int (*start)(struct cyber *this);

    /**
     * @brief Arrête les threads associés
     * 
     * @param[inout]  this l'engin lui-même
     * @return ==0 si Ok, code d'erreur sinon
     */
    int (*stop)(struct cyber *this, bool force);
    
    /**
     * @brief envoi un message sur le canal
     * 
     * Encode ccomplètement le message et le dépose dans
     * la queue d'attente.
     * 
     * \note ne libère pas le message....
     * 
     * @param[inout]  this l'engin lui-même
     * @param[in]     ptxt  message
     * @param[in]     attrib attribu de chiffrement par exemple
     * @return ==0 si OK, code d'erreur sinon...
     */
    // int (*send)(struct cyber *this, rsmp_messages_ptr ptxt, void const *attrib);
    // int (*send)(struct cyber *this, char *payload, size_t len, int flag_encrypt, void const *attrib);
    int (*send)(struct cyber *this, const char *payload, size_t len, const char * login/*void const *attrib*/);
    

    int (*waitForPayload)(struct cyber *this, char **payload, size_t *len, void **attrib, size_t to_sec);
    /**
     * @brief Libération de l'engin
     * 
     * Ferme tout détruit tout...
     * @param[inout]  this l'engin lui-même
     */
    void (*free)(struct cyber *this);

    //static int disable_encryption(cyber_t *this)
    /**
     * @brief positionne le cipher de l'cyber sur un fake cipher "désactivé"
    */
#ifdef TEST_ENABLED
    int (*disable_encryption)(struct cyber *this);
#endif

} cyber_t;


/**
 * @brief Initialization de l'environnement
 * 
 * @return ==0 si OK, code d'erreur sinon
 */
int cyber_initialize();

/**
 * @brief Création d'un cyber vers un destinataire
 * 
 * lance un thread de communication vers un destinataire
 * @param rsmp_context context RSMP à appliquer
 * @param flux         flux de comunication
 * @param id           id de l'engin, si NULL choix automatique
 * @param sId_Local    id local
 * @param sId_Remote   id du destinataire
 * @param isLeader     1 si leader, 0 sinon
 * @return pointeur sur l'engin
 */
// cyber_t *cyber_new(/*rsmp_context_t const *rsmp_context,*/ FLUX *flux, char const *id, char * sId_Local, char * sId_Remote, int is_leader);

cyber_t *cyber_new_from_transport(transport_t *transport);


#ifdef TEST_ENABLED
void cyber_disableChallenge(cyber_t* this, bool val);
#endif

// typedef int(*cyber_msg_autorise_t)(void *context, rsmp_messages_ptr ptxt);
// typedef int(*cyber_msg_callback_t)(void *context, cyber_t *cyber, rsmp_messages_ptr ptxt, char *err_str, size_t err_len);

/**@}*/
#endif //CYBER_CYBER_H