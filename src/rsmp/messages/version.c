#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "messages.h"

#define RSMP_MESSAGES_VERSION_C
#include "version.h"
#include "rsmp/fields/sg.h"

rsmp_field_desc_t _rsmp_msg_RSMP_desc[] =
{
    rsmp_field_desc(RSMPs, vers),
    rsmp_field_desc_end(),
};

rsmp_field_desc_t _rsmp_msg_siteId_desc[] =
{
    rsmp_field_desc(siteIds, sId),
    rsmp_field_desc_end(),
};




rsmp_field_desc_t _rsmp_msg_eSXLG_desc[] =
{
    rsmp_field_desc(eSXLGs, vers),
    // rsmp_field_desc(eSXLGs, sg), // \todo parsing de sg
    rsmp_field_desc_end(),
};



rsmp_field_desc_t _rsmp_msg_Version_desc[] =
{
    rsmp_field_desc(rsmp_msg_Version_t, mType),
    rsmp_field_desc(rsmp_msg_Version_t, type),
    rsmp_field_desc(rsmp_msg_Version_t, mId),
    rsmp_field_desc_array(rsmp_msg_Version_t, RSMP, _rsmp_msg_RSMP_desc),
    rsmp_field_desc_array(rsmp_msg_Version_t, siteId, _rsmp_msg_siteId_desc),
    // rsmp_field_desc_array(rsmp_msg_Version_t, eSXLG, _rsmp_msg_eSXLG_desc),
    rsmp_field_desc(rsmp_msg_Version_t, SXL),
    rsmp_field_desc_end(),
};


rsmp_msg_Version_t *rsmp_msg_Version_new(rsmp_context_t const *rsmp_context)
{
    rsmp_msg_Version_t * msg = calloc(1, sizeof(*msg));
    if (msg)
    {
        msg->type = _rsmp_msg_Version_;
        rsmp_mId_new(msg->mId);
        rsmp_array_RSMPs_declare(&msg->RSMP);
        rsmp_array_siteIds_declare(&msg->siteId);
        rsmp_array_eSXLGs_declare(&msg->eSXLG);

        if (!rsmp_context) rsmp_context=rsmp_context_global_get();
        if (rsmp_context) rsmp_context_set(msg, rsmp_context);
    }
    return msg;
}
void *_rsmp_msg_Version_new(rsmp_context_t const *rsmp_context) { return rsmp_msg_Version_new(rsmp_context);}

void rsmp_msg_Version_free(rsmp_msg_Version_t *item)
{
    if (item == NULL) return;
    assert(rsmp_message_is_Version(item));
    rsmp_str_free(&item->SXL);
    rsmp_array_RSMPs_flush(&item->RSMP);
    rsmp_array_siteIds_flush(&item->siteId);
    rsmp_array_eSXLGs_flush(&item->eSXLG);
    free(item);
}

void _rsmp_msg_Version_free(void *item)
{
    rsmp_msg_Version_free((rsmp_msg_Version_t *)item);
}


bool rsmp_msg_Version_set(rsmp_msg_Version_t *version)
{
    if (!version) return false;
    version->type = _rsmp_msg_Version_;
    rsmp_mId_new(version->mId);

    // RSMPs *rsmp = rsmp_array_RSMPs_new_item(&version->RSMP);
    // rsmp_str_set(&rsmp->vers,"3.2");
    // siteIds *siteId = rsmp_array_siteIds_new_item(&version->siteId);
    // rsmp_str_set(&siteId->sId,"Mon site à moi");
    // rsmp_str_set(&version->SXL, "1.0.0");
    
    return true;
}