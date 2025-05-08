#ifndef DISP_M31_H
#define DISP_M31_H

#include <stdint.h>
#include "disp_display.h"

/**
 * @file disp_m31.h
 * @brief Structures de données pour le message DISP/M31 (Écriture des messages disponibles)
 */

// Durée
typedef struct {
    int onDuration;
} Duration;

// Contenu
typedef struct {
    char contentType[16];
    char contextText[256];
    Duration duration;
} Content;

// Animation
typedef struct {
    char zoneId[32];
    char animType[16];
    Content* contents;
    int contentCount;
} Animation;

// Séquence
typedef struct {
    char templateId[32];
    char end[16];
    Animation* animations;
    int animationCount;
} Sequence;

// MessageObject
typedef struct {
    char msgId[64];
    int repeatCount;
    Sequence* sequences;
    int sequenceCount;
} MessageObject;

// CommandRequest DISP/M31
typedef struct {
    MessageObject* messages;
    int messageCount;
} DispM31CommandRequest;

// Structures de réponse
typedef struct {
    char code[32];
    char rea[256];
} RetStatusObject;

typedef struct {
    char msgId[64];
    RetStatusObject retStatus;
} RetMessageObject;

typedef struct {
    RetMessageObject* retMessages;
    int retMessageCount;
} DispM31CommandResponse;

// Fonctions déclarées
void free_disp_m31_command_request(DispM31CommandRequest* req);
int is_valid_msg_id(const char* msgId);

#endif // DISP_M31_H
