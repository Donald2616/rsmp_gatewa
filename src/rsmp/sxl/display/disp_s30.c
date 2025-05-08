#include "disp_s30.h"
#include <stdlib.h>
/*
void free_disp_s30_status_response(DispS30StatusResponse* res) {
    if (!res) return;

    for (size_t i = 0; i < res->msgIdCount; ++i) {
        free(res->msgIdList[i]);
    }
    free(res->msgIdList);
    res->msgIdList = NULL;
    res->msgIdCount = 0;
}
*/
void free_disp_s30_status_response(DispS30StatusResponse* res) {
    if (res == NULL) return;

    for (size_t i = 0; i < res->msgIdCount; ++i) {
        if (res->msgIdList[i]) {
            free(res->msgIdList[i]);
        }
    }

    free(res->msgIdList);
    res->msgIdList = NULL;
    res->msgIdCount = 0;
}
