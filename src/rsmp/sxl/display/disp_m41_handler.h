#ifndef DISP_M41_HANDLER_H
#define DISP_M41_HANDLER_H

#include "rsmp/fields/arg.h"
#include "rsmp/fields/rvs.h"

int handle_disp_m41(void *context, void *rsmp_context, args const *request, rvs *response);

#endif // DISP_M41_HANDLER_H
