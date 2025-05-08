/**
* @file rsmp-context.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-06
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include "log/log.h"
#include "rsmp/rsmp.h"
#include "rsmp/utils/misc.h"
#include "rsmp/messages/messages.h"

#define RSMP_ENV_RSMP_CONTEXT_C
#include "rsmp-context.h"

#include "ioprintf/ioprintf.h"

/**
 * @ingroup rsmp-context
 * @{
 */
#ifndef RSMP_VERS_DEFAULT
#define RSMP_VERS_DEFAULT "3.2.1"
#endif

#ifndef SITEID_SID_DEFAULT
// #define SITEID_SID_DEFAULT "SVMS LCR Gateway"
// \todo bug le sId positionné par option du lanceur n'affecte que la couche cyber et pas un message de version
#define SITEID_SID_DEFAULT "CLIENT_SVMS"
#endif

#ifndef SXL_DEFAULT
#define SXL_DEFAULT ""
#endif

#ifndef NTSOID_DEFAULT
#define NTSOID_DEFAULT "AA+BBCCC=DDDEEFFF"
#endif

eSXLGs eSXLG_default[] = {
    {  "1.0.0" , _rsmp_sg_GEN_},
    { "0.5.2", _rsmp_sg_SLT_ }
    // { "IO", "0.3.0" },
    // { "VAR", "0.1.0" }
};


rsmp_context_t *_rsmp_context_default = NULL;


static void rsmp_context_atexit(void)
{
    if (_rsmp_context_default)
    {
        rsmp_context_free(_rsmp_context_default);
        _rsmp_context_default = NULL;
    }
}

static rsmp_context_t *rsmp_context_set_default(rsmp_context_t *item)
{
    if (item && _rsmp_context_default)
    {
        rsmp_ntsOId_set(&item->ntsOId, _rsmp_context_default->ntsOId);
        rsmp_cId_set(&item->cId,       _rsmp_context_default->cId);
        rsmp_xNId_set(&item->xNId,     _rsmp_context_default->xNId);

        rsmp_SXL_set(&item->SXL,       _rsmp_context_default->SXL?_rsmp_context_default->SXL:SXL_DEFAULT);
        if (_rsmp_context_default->RSMP.len)
        {
            rsmp_array_RSMPs_dup2(&item->RSMP, &_rsmp_context_default->RSMP);
        }
        else
        {
            RSMPs *rec = rsmp_array_RSMPs_new_item(&item->RSMP);
            rsmp_vers_set(&rec->vers, RSMP_VERS_DEFAULT);
        }

        if (_rsmp_context_default->eSXLG.len)
        {
            rsmp_array_eSXLGs_dup2(&item->eSXLG, &_rsmp_context_default->eSXLG);
        }
        else
        {
            eSXLGs *rec = rsmp_array_eSXLGs_new_item(&item->eSXLG);
            for (size_t i = 0; i < sizeof(eSXLG_default) / sizeof(eSXLG_default[0]); i++)
            {
                rec[i].sg = eSXLG_default[i].sg;
                rsmp_vers_set(&rec[i].vers, eSXLG_default[i].vers);                
            }
        }

        if (_rsmp_context_default->siteId.len)
        {
            rsmp_array_siteIds_dup2(&item->siteId, &_rsmp_context_default->siteId);
        }
        else
        {
            siteIds *rec = rsmp_array_siteIds_new_item(&item->siteId);
            rsmp_sId_set(&rec->sId, SITEID_SID_DEFAULT);
        }

    }
    return item;
}

static inline rsmp_context_t *rsmp_context_get_global(void)
{
    if (!_rsmp_context_default) 
    {
        atexit(rsmp_context_atexit);
        _rsmp_context_default=calloc(1,sizeof(*_rsmp_context_default));
        assert(_rsmp_context_default != NULL);
        rsmp_array_RSMPs_declare(&_rsmp_context_default->RSMP);
        rsmp_array_siteIds_declare(&_rsmp_context_default->siteId);
        rsmp_array_eSXLGs_declare(&_rsmp_context_default->eSXLG);
        rsmp_SXL_set(&_rsmp_context_default->SXL, SXL_DEFAULT);
        rsmp_ntsOId_set(&_rsmp_context_default->ntsOId, NTSOID_DEFAULT);
        rsmp_cId_set(&_rsmp_context_default->cId, NTSOID_DEFAULT);
    }
    return _rsmp_context_default;
}


rsmp_context_t *rsmp_context_new(void)
{
    rsmp_context_t *item=calloc(1,sizeof(*item));
    if (item)
    {
        //-- recopie des valeurs par défaut
        rsmp_context_set_default(item);
    }
    return item;
}

void rsmp_context_free(rsmp_context_t *item)
{
    if (item)
    {
        rsmp_array_RSMPs_flush(&item->RSMP);;
        rsmp_array_eSXLGs_flush(&item->eSXLG);
        rsmp_array_siteIds_flush(&item->siteId);
        rsmp_str_free(&item->SXL);
        rsmp_str_free(&item->ntsOId);
        rsmp_str_free(&item->cId);
        rsmp_str_free(&item->xNId);
        rsmp_str_free(&item->sId);
        free(item);
    }
}


void rsmp_context_RSMP_add(rsmp_context_t *item, const vers_t value)
{
   if (!item) item =  rsmp_context_get_global();
   if (item && value)
   {
        RSMPs *rec = rsmp_array_RSMPs_new_item(&item->RSMP);
        rsmp_vers_set(&rec->vers, value);
   }
}


void rsmp_context_eSXLG_add(rsmp_context_t *item, sg_t sg_val, const vers_t vers_val)
{
   if (!item) item =  rsmp_context_get_global();
   if (item && vers_val && sg_val<_rsmp_sg_no_)
   {
        eSXLGs *rec = rsmp_array_eSXLGs_new_item(&item->eSXLG);
        rec->sg = sg_val;
        rsmp_vers_set(&rec->vers, vers_val);
   }
}



void rsmp_context_siteId_add(rsmp_context_t *item, const sId_t value)
{
   if (!item) item =  rsmp_context_get_global();
   if (item && value)
   {
        siteIds *rec = rsmp_array_siteIds_new_item(&item->siteId);
        rsmp_vers_set(&rec->sId, value);
   }
}



void rsmp_context_SXL_set(rsmp_context_t *item, SXL_t value)
{
   if (!item) item =  rsmp_context_get_global();
   if (item)
   {
        rsmp_SXL_set(&item->SXL, value);
   }
}

void rsmp_context_sId_set(rsmp_context_t *item, sId_t value)
{
   if (!item) item =  rsmp_context_get_global();
   if (item)
   {
        rsmp_sId_set(&item->sId, value);
   }
}




void rsmp_context_ntsOId_set(rsmp_context_t *item, ntsOId_t value)
{
   if (!item) item =  rsmp_context_get_global();
   if (item)
   {
        rsmp_ntsOId_set(&item->ntsOId, value);
   }
}





void rsmp_context_cId_set(rsmp_context_t *item, cId_t value)
{
   if (!item) item =  rsmp_context_get_global();
   if (item)
   {
        rsmp_cId_set(&item->cId, value);
   }
}





void rsmp_context_xNId_set(rsmp_context_t *item, xNId_t value)
{
   if (!item) item =  rsmp_context_get_global();
   if (item)
   {
        rsmp_xNId_set(&item->xNId, value);
   }
}



rsmp_context_t const *rsmp_context_global_get(void)
{
    return rsmp_context_get_global();
}

void rsmp_context_set(void *msg, rsmp_context_t const *item)
{
    if (msg)
    {
        rsmp_msg_desc_t const *desc = rsmp_message_desc_get(((type_t *)msg));
        if (desc)
        {
            rsmp_field_desc_t const *field = desc->fields;
            while (field->type != _rsmp_field_desc_end_mark_)
            {
                void *base = msg + field->offset;
                switch(field->field)
                {
                    case _rsmp_field_xNId_:
                        rsmp_xNId_set((xNId_t *)base, item->xNId);
                        break;

                    case _rsmp_field_ntsOId_:
                        rsmp_ntsOId_set((ntsOId_t *)base, item->ntsOId);
                        break;

                    case _rsmp_field_cId_:
                        rsmp_cId_set((cId_t *)base, item->cId);
                        break;

                    case _rsmp_field_SXL_:
                        rsmp_cId_set((SXL_t *)base, item->SXL);
                        break;

                    case _rsmp_field_RSMP_:
                        rsmp_array_RSMPs_dup2((rsmp_array_RSMPs_t *)base, &item->RSMP);
                        break;

                    case _rsmp_field_eSXLG_:
                        rsmp_array_eSXLGs_dup2((rsmp_array_eSXLGs_t *)base, &item->eSXLG);
                        break;

                    case _rsmp_field_siteId_:
                        rsmp_array_siteIds_dup2((rsmp_array_siteIds_t *)base, &item->siteId);
                        break;

                    default:
                        break;
                }
                field++;
            }
        }
    } 
}





int rsmp_context_from_str(rsmp_context_t *item, const char *options)
{
    
    int ret = _rsmp_no_error_;
    if (!item) item = rsmp_context_get_global();
    if (item)
    {
        //-- Extraction des paramètres
        int argc=0;
        char **argv=NULL;
        char *options_bis = strdup(options);
        if ((argc = decodeparam(options_bis, ',', &argv))>0)
        {
            bool sId_set = false;
            for(size_t i=0; i< (size_t)argc && ret == _rsmp_no_error_; i++)
            {
                //-- décodage des valeurs label:value
                char *value = strchr(argv[i], ':');
                if (value)
                {
                    *value='\0';
                    value++;
                    switch(_rsmp_field_from_lib(argv[i]))
                    {
                        case _rsmp_field_vers_:
                            rsmp_context_RSMP_add(item, value);
                            break;

                        case _rsmp_field_sId_:
                            rsmp_context_siteId_add(item, value);
                            if (!sId_set)
                            {
                               rsmp_context_sId_set(item, value); 
                               sId_set = true;
                            }
                            break;

                        case _rsmp_field_xNId_:
                            rsmp_context_xNId_set(item, value);
                            break;

                        case _rsmp_field_SXL_:
                            rsmp_context_SXL_set(item, value);
                            break;

                        // case _rsmp_field_eSXLG_:
                        //     rsmp_context_eSXLG_add(item, value); \todo affectation depuis chaine ne fonctionne pas (définir une syntaxe pour la hierarchie ; eSXLG a 2 elements)

                        case _rsmp_field_ntsOId_:
                            rsmp_context_ntsOId_set(item, value);
                            break;
                        
                        case _rsmp_field_cId_:
                            rsmp_context_cId_set(item, value);
                            break;
                            
                        default:
                            ret = _rsmp_errcod_gen_config_;
                            _rsmp_log_err(ret, "field: %s, unknown in rsmp context", argv[i]);
                            break;
                    }
                }
                else
                {
                    ret=_rsmp_errcod_gen_config_;
                    _rsmp_log_err(ret, "syntax error - no separator %s", argv[i]);
                }
            }
        }
        if (argv) free(argv);
        if (options_bis) free(options_bis);

    }
    return ret;
}

void rsmp_context_log(rsmp_context_t const *item)
{
    rsmp_context_initialize();
    if (!item) item = rsmp_context_get_global();
    if (item)
    {
        char *s=NULL;
        _log_info("RSMP versions: %s", rsmp_array_RSMPs_to_string(&item->RSMP, NULL, &s));
        free(s);
        s=NULL;
        _log_info("siteId: %s", rsmp_array_siteIds_to_string(&item->siteId, NULL, &s));
        free(s);
        s=NULL;
        _log_info("SXL version: %s", item->SXL);
        _log_info("eSXLG versions: %s", rsmp_array_eSXLGs_to_string(&item->eSXLG, NULL, &s));
        free(s);
        s=NULL;        
        _log_info("cId: %s", item->cId);
        _log_info("ntsOId: %s", item->ntsOId);
        _log_info("xNId: %s", item->xNId);
        _log_info("sId: %s", item->sId);
    }
}

void rsmp_context_initialize(void)
{
    static bool init=false;
    if (init) return;
    rsmp_context_get_global();
    //-- Ajout des valeurs par défaut si NULL...
    if (!_rsmp_context_default->RSMP.len) rsmp_context_global_RSMP_add(RSMP_VERS_DEFAULT);
    if (!_rsmp_context_default->siteId.len) rsmp_context_global_siteId_add(SITEID_SID_DEFAULT);
    if (!_rsmp_context_default->eSXLG.len) {
        
        for (size_t i = 0; i < (sizeof(eSXLG_default) / sizeof(eSXLG_default[0])); i++)
        {
            rsmp_context_global_eSXLG_add(eSXLG_default[i].sg, eSXLG_default[i].vers);
        }
    }
    init = true;
}
/**@}*/