#ifndef DISP_M32_HANDLER_H
#define DISP_M32_HANDLER_H

#include "rsmp/fields/arg.h"
#include "rsmp/fields/rvs.h"

int handle_disp_m32(void *context, void *rsmp_context, args const *request, rvs *response);

#endif // DISP_M32_HANDLER_H
