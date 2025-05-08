#include "disp_m31.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Vérifie si un msgId respecte le format String1 (A-Z, 0-9, '-', '.')
 */
int is_valid_msg_id(const char* msgId) {
    if (!msgId || strlen(msgId) == 0)
        return 0;

    for (size_t i = 0; i < strlen(msgId); ++i) {
        char c = msgId[i];
        if (!(isalnum(c) || c == '-' || c == '.')) {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief Libère toute la mémoire allouée dynamiquement dans un DispM31CommandRequest
 */
void free_disp_m31_command_request(DispM31CommandRequest* req) {
    if (!req || !req->messages) return;

    for (int i = 0; i < req->messageCount; ++i) {
        MessageObject* msg = &req->messages[i];

        for (int j = 0; j < msg->sequenceCount; ++j) {
            Sequence* seq = &msg->sequences[j];

            for (int k = 0; k < seq->animationCount; ++k) {
                Animation* anim = &seq->animations[k];
                if (anim->contents)
                    free(anim->contents);
            }

            if (seq->animations)
                free(seq->animations);
        }

        if (msg->sequences)
            free(msg->sequences);
    }

    free(req->messages);
    req->messages = NULL;
    req->messageCount = 0;
}

