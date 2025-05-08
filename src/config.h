#define _GNU_SOURCE
#define _LOG_FILE_MARK

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <unistd.h>

#include "log/log.h"
#include "fhs/fhs.h"


#ifndef __FILE_BASE_NAME__
#define __FILE_BASE_NAME__ __FILE__
#endif
#define VERSION "0.2"
#ifndef REVISION
#define REVISION "unknown source"
#endif
#ifndef PROJET
#define PROJET "rsmp-gateway"
#endif
#ifndef FHS_APP
#define FHS_APP PROJET
#endif
#define get_version() "V" VERSION " create: " __DATE__ " from: " REVISION ""
#ifndef _CHECK_MEM_
#define _CHECK_MEM_ 0


#endif
