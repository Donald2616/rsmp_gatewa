/**
* @file cmd-request.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-03
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"
#define RSMP_MESSAGES_CMD_RESPONSE_C

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "messages.h"

#include "cmd-response.h"


/**
 * @ingroup cmd-response
 * @{
 */

rsmp_field_desc_t _rsmp_msg_rvs_desc[] =
{
    rsmp_field_desc(rvs, cCI),
    rsmp_field_desc(rvs, n),
    rsmp_field_desc(rvs, v),
    rsmp_field_desc(rvs, age),
    rsmp_field_desc_end(),
};



rsmp_field_desc_t _rsmp_msg_CommandResponse_desc[] =
{
    rsmp_field_desc(rsmp_msg_CommandResponse_t, mType),
    rsmp_field_desc(rsmp_msg_CommandResponse_t, type),
    rsmp_field_desc(rsmp_msg_CommandResponse_t, mId),
    rsmp_field_desc(rsmp_msg_CommandResponse_t, ntsOId),
    rsmp_field_desc(rsmp_msg_CommandResponse_t, xNId),
    rsmp_field_desc(rsmp_msg_CommandResponse_t, cId),
    rsmp_field_desc(rsmp_msg_CommandResponse_t, cTS),
    rsmp_field_desc_array(rsmp_msg_CommandResponse_t, rvs, _rsmp_msg_rvs_desc),
   rsmp_field_desc_end(),
};

rsmp_msg_CommandResponse_t *rsmp_msg_CommandResponse_new(rsmp_context_t const *rsmp_context)
{
    rsmp_msg_CommandResponse_t *item = calloc(1, sizeof(*item));

    if (item)
    {
        item->type = _rsmp_msg_CommandResponse_;
        rsmp_mId_new(item->mId);
        rsmp_cTS_set(&item->cTS);
        rsmp_array_rvs_declare(&item->rvs);
        if (!rsmp_context) rsmp_context=rsmp_context_global_get();
        if (rsmp_context) rsmp_context_set(item, rsmp_context);
    }
    return item;
}

void *_rsmp_msg_CommandResponse_new(rsmp_context_t const *rsmp_context)
{
    return rsmp_msg_CommandResponse_new(rsmp_context);
}




void rsmp_msg_CommandResponse_free(rsmp_msg_CommandResponse_t *item)
{
    if (item)
    {
        rsmp_ntsOId_free(&item->ntsOId);
        rsmp_xNId_free(&item->xNId);
        rsmp_cId_free(&item->cId);
        rsmp_array_rvs_flush(&item->rvs);
        free(item);
    }
}

void _rsmp_msg_CommandResponse_free(void *item)
{
    rsmp_msg_CommandResponse_free((rsmp_msg_CommandResponse_t *)item);
}


rsmp_msg_CommandResponse_t *rsmp_msg_CommandResponse_from_request(rsmp_msg_CommandRequest_t const *request)
{
    rsmp_msg_CommandResponse_t *item = rsmp_msg_CommandResponse_new(NULL);
    if (item && request)
    {
        rsmp_ntsOId_set(&item->ntsOId, request->ntsOId);
        rsmp_xNId_set(&item->xNId, request->xNId);
        rsmp_cId_set(&item->cId, request->cId);
        rsmp_array_rvs_initialize(&item->rvs, request->arg.len);
        //-- Copy des champs de l'un dans l'autre
        for(size_t i=0; i< item->rvs.len; i++)
        {
            rsmp_cCI_set(&item->rvs.items[i].cCI, request->arg.items[i].cCI);
            rsmp_n_set(&item->rvs.items[i].n, request->arg.items[i].n);
            rsmp_array_char_flush(&item->rvs.items[i].v);
        }
    }
    return item;
}

/**@}*/