#include "rsmp/vars/timestamp.h"
#include "rsmp/messages/messages.h"
#include "rsmp/messages/watchdog.h"
#include "rsmp/messages/cmd-request.h"
#include "rsmp/messages/cmd-response.h"
#include "rsmp/messages/messageack.h"
#include "rsmp/messages/messagenotack.h"
#include "rsmp/messages/version.h"
#include "rsmp/messages/alarm.h"
#include "rsmp/messages/agg-request.h"
#include "rsmp/messages/agg-status.h"
#include "rsmp/messages/stat-response.h"
#include "rsmp/messages/stat-subscribe.h"
#include "rsmp/messages/stat-unsubscribe.h"
#include "rsmp/coder/json.h"

void test_rsmp_messages(void)
{
        rsmp_context_global_log(); // permet le positionnement de eSXLG
        
        // test de sérialisation/désérialisation des trames RSMP json
        rsmp_context_global_RSMP_add("3.1.5");
        rsmp_context_global_RSMP_add("3.2.1");
        rsmp_context_global_siteId_add("Mon Site à moi tout seul");
        rsmp_context_global_ntsOId_set("ntsOId_t");
        rsmp_context_global_cId_set("cId_t");
        rsmp_context_t const *rcontext = rsmp_context_global_get();
        char *rvers = NULL;
        rsmp_array_RSMPs_to_string(&rcontext->RSMP, NULL, &rvers);
        _log_info("%s", &rvers);
        free(rvers);

        for (type_t t = 0; rsmp_type_exits(t); t++)
        {
            rsmp_messages_ptr msg = rsmp_message_new(NULL, t);
            char const *out = rsmp_json_msg2payload(msg);
            rsmp_message_free(msg);
            msg = rsmp_json_payload2msg(rcontext, out);
            char const *out2 = rsmp_json_msg2payload(msg);
            if (strcmp(out2, out))
            {
                _rsmp_log_err(_rsmp_errcod_gen_generic_, "msg:%s, strcmp failure\n%s\n%s", rsmp_type_to_string(t), out2, out);
                printf("{\"error\":\"failure\",\"json1\":%s,\"json2\":%s", out, out2);
            }
            else
            {
                
                printf("%s \n*****************\n", out2);
            }
            rsmp_message_free(msg);
            free((void *)out);
            free((void *)out2);
        }

        
    }
