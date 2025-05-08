#include <stdlib.h>
#include <string.h>
#include "disp_m32.h"

/**
 * @file disp_m32.c
 * @brief Fonctions utilitaires pour DISP/M32
 */

void free_disp_m32_command_request(DispM32CommandRequest* req) {
    if (!req || !req->msgIdList) return;

    for (int i = 0; i < req->msgIdCount; ++i) {
        if (req->msgIdList[i]) {
            free(req->msgIdList[i]);
        }
    }
    free(req->msgIdList);
    req->msgIdList = NULL;
    req->msgIdCount = 0;
}

