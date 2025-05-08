#include "disp_s40.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Libère dynamiquement une structure DISP/S40.
 *
 * @param res Structure à libérer
 */
void free_disp_s40_status_response(DispS40StatusResponse* res) {
    if (!res) return;
    if (res->displayPoolList) {
        free(res->displayPoolList);
        res->displayPoolList = NULL;
    }
    res->displayPoolCount = 0;
}
