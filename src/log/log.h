/**
* @file log.h
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-05-15
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef LOG_LOG_H
#define LOG_LOG_H
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdarg.h>

/**
 * @page log Log / Journalisation
 * 
 *  * Écriture d'un générateur de log plus _simple_ que celui des Libraries habituelles.
 * 
 * 
 * 
 */
/**
 * @defgroup log 
 * 
 * @brief 
 * 
 * @{
 */

#ifndef LOG_SECTION
#define LOG_SECTION "-----"
#endif

#ifdef LOG_LOG_C
#define LOG_PRIVATE
#define LOG_PROTECTED
#define LOG_EXTERN
#endif

#ifdef LOG_INHERITAGE
#define LOG_PROTECTED 
#endif

#ifndef LOG_PRIVATE
#define LOG_PRIVATE const
#endif
#ifndef LOG_PROTECTED
#define LOG_PROTECTED const
#endif
#ifndef LOG_EXTERN
#define LOG_EXTERN extern
#endif

typedef uint16_t log_level_t;
#define LOG_LEVEL_DUMP     ((log_level_t)(1<<0))
#define LOG_LEVEL_INFO     ((log_level_t)(1<<1)) 
#define LOG_LEVEL_ERROR    ((log_level_t)(1<<2))
#define LOG_LEVEL_WARNING  ((log_level_t)(1<<3))
#define LOG_LEVEL_DEBUG    ((log_level_t)(1<<4))
#define LOG_LEVEL_NONE     ((log_level_t)0)
#define LOG_LEVEL_ALL      ((log_level_t)-1)

#define _log_level_is_set(l) (((l)&log_level)!=0)
#define _log_info_is_set()  _log_level_is_set(LOG_LEVEL_INFO)
#define _log_err_is_set()   _log_level_is_set(LOG_LEVEL_ERROR)
#define _log_warn_is_set()  _log_level_is_set(LOG_LEVEL_WARNING)
#define _log_debug_is_set() _log_level_is_set(LOG_LEVEL_DEBUG)
#define _log_dump_is_set()  _log_level_is_set(LOG_LEVEL_DUMP)

#define LOG_LEVEL_DEFAULT (LOG_LEVEL_INFO|LOG_LEVEL_ERROR|LOG_LEVEL_WARNING)
LOG_EXTERN LOG_PRIVATE log_level_t log_level
#ifdef LOG_LOG_C
= LOG_LEVEL_DEFAULT
#endif
;
void _log_level_set(log_level_t level);
void _log_level_clear(log_level_t level);

LOG_EXTERN LOG_PRIVATE int _log_fd
#ifdef LOG_LOG_C
= 2
#endif
;

void _log_fd_set(int fd);
void _log_fd_stdout(void);

#define LOG_MARK_INFO    "Info "
#define LOG_MARK_ERROR   "Error"
#define LOG_MARK_WARNING "Warn "
#define LOG_MARK_IN      "<<<<<"
#define LOG_MARK_OUT     ">>>>>"
#define LOG_MARK_DEBUG   "Debug"
#define LOG_MARK_DISIN   "<DIS<"
#define LOG_MARK_DISOUT  ">DIS>"

#ifdef _LOG_FILE_MARK
#define LOG_FMARK_FORMAT "%s at %s:%d "
#else
#define LOG_FMARK_FORMAT "%*%*%*"
#endif

int _log_dump(int dlog, log_level_t level, char const *sens, int err, char const *section, char const *buf, ssize_t len, char const *fformat, char const *function, char const *fname, int line,  char const *format, ...) __attribute__ ((format(printf, 12, 13)));
int _log_varg(int dlog, log_level_t level, char const *prefix, int err, char const *section, char const *format, va_list ap);
int _log(int fd, log_level_t level, char const *sens, int err, char const *section, char const *format, ...) __attribute__ ((format(printf, 6, 7)));
// #define VVA
#ifdef VVA
#define _log_info(f, ...)       ((void)0)
#define _log_err(f, ...)    ((void)0)
#define _log_errno(f, ...)  ((void)0)
#define _log_warn(f, ...)    ((void)0)   
#define _log_debug(f, ...)       ((void)0)
#define _log_dump_debug(p, b, l, f, ...)  ((void)0)
#define _log_dump_info(p, b, l, f, ...)  ((void)0)
#define _log_dump_debug_in(b, l, f, ...)  ((void)0)
#define _log_dump_debug_out(b, l, f, ...) ((void)0)
#define _log_dump_info_in(b, l, f, ...)  ((void)0)
#define _log_dump_info_out(b, l, f, ...) ((void)0)
#define _log_dump_err(p, b, l, f, ...)   ((void)0)
#define _log_dump_err_in(b, l, f, ...)   ((void)0)
#define _log_dump_err_out(b, l, f, ...)  ((void)0)
#define _log_dump_errno(p, e, b, l, f, ...)  ((void)0)
#define _log_dump_errno_in(e, b, l, f, ...)  ((void)0)
#define _log_dump_errno_out(e, b, l, f, ...) ((void)0)
#define _log_dump_in(e, b, l, f, ...)  ((void)0)
#define _log_dump_out(e, b, l, f, ...) ((void)0)
#else
#define _log_info(f, ...)        _log(_log_fd, LOG_LEVEL_INFO, LOG_MARK_INFO, 0, LOG_SECTION, "" LOG_FMARK_FORMAT "" f "", __FUNCTION__, __FILE_BASE_NAME__, __LINE__, ##__VA_ARGS__)
#define _log_err(f, ...)         _log(_log_fd, LOG_LEVEL_ERROR, LOG_MARK_ERROR, 0, LOG_SECTION,  "" LOG_FMARK_FORMAT "" f "", __FUNCTION__, __FILE_BASE_NAME__, __LINE__, ##__VA_ARGS__)
#define _log_errno(f, ...)       _log(_log_fd, LOG_LEVEL_ERROR, LOG_MARK_ERROR, errno, LOG_SECTION,  "" LOG_FMARK_FORMAT "" f "", __FUNCTION__, __FILE_BASE_NAME__, __LINE__, ##__VA_ARGS__)
#define _log_warn(f, ...)        _log(_log_fd, LOG_LEVEL_WARNING, LOG_MARK_WARNING, 0, LOG_SECTION,  "" LOG_FMARK_FORMAT "" f "", __FUNCTION__, __FILE_BASE_NAME__, __LINE__, ##__VA_ARGS__)

#define _log_debug(f, ...)       _log(_log_fd, LOG_LEVEL_DEBUG, LOG_MARK_DEBUG, 0, LOG_SECTION,  "" LOG_FMARK_FORMAT "" f "", __FUNCTION__, __FILE_BASE_NAME__, __LINE__, ##__VA_ARGS__)
#define _log_dump_debug(p, b, l, f, ...)  _log_dump(_log_fd, LOG_LEVEL_DEBUG, p, 0, LOG_SECTION, b, l, LOG_FMARK_FORMAT, __FUNCTION__, __FILE_BASE_NAME__, __LINE__, f, ##__VA_ARGS__)
#define _log_dump_info(p, b, l, f, ...)  _log_dump(_log_fd, LOG_LEVEL_INFO, p, 0, LOG_SECTION, b, l, LOG_FMARK_FORMAT, __FUNCTION__, __FILE_BASE_NAME__, __LINE__, f, ##__VA_ARGS__)
#define _log_dump_debug_in(b, l, f, ...)  _log_dump_debug(LOG_MARK_IN, b, l,  f, ##__VA_ARGS__)
#define _log_dump_debug_out(b, l, f, ...) _log_dump_debug(LOG_MARK_OUT, b, l, f, ##__VA_ARGS__)
#define _log_dump_info_in(b, l, f, ...)  _log_dump_info(LOG_MARK_IN, b, l,  f, ##__VA_ARGS__)
#define _log_dump_info_out(b, l, f, ...) _log_dump_info(LOG_MARK_OUT, b, l, f, ##__VA_ARGS__)
#define _log_dump_err(p, b, l, f, ...)   _log_dump(_log_fd, LOG_LEVEL_ERROR, p, 0, LOG_SECTION, b, l, LOG_FMARK_FORMAT, __FUNCTION__, __FILE_BASE_NAME__, __LINE__, f, ##__VA_ARGS__)
#define _log_dump_err_in(b, l, f, ...)   _log_dump_err(LOG_MARK_IN, b, l, f, ##__VA_ARGS__)
#define _log_dump_err_out(b, l, f, ...)  _log_dump_err(LOG_MARK_OUT, b, l, f, ##__VA_ARGS__)
#define _log_dump_errno(p, e, b, l, f, ...)  _log_dump(_log_fd, LOG_LEVEL_ERROR, p, , LOG_SECTION, b, l, __FUNCTION__, __FILE_BASE_NAME__, __LINE__, f, ##__VA_ARGS__)
#define _log_dump_errno_in(e, b, l, f, ...)  _log_dump_errno(LOG_MARK_IN, e, b, l,  f, ##__VA_ARGS__)
#define _log_dump_errno_out(e, b, l, f, ...) _log_dump_errno(LOG_MARK_OUT, e, b, l, f,  ##__VA_ARGS__)
#define _log_dump_in(e, b, l, f, ...)  _log_dump(_log_fd, LOG_LEVEL_DUMP, LOG_MARK_IN, e, LOG_SECTION, b, l, LOG_FMARK_FORMAT, __FUNCTION__, __FILE_BASE_NAME__, __LINE__, f, ##__VA_ARGS__)
#define _log_dump_out(e, b, l, f, ...) _log_dump(_log_fd, LOG_LEVEL_DUMP, LOG_MARK_OUT, e, LOG_SECTION, b, l, LOG_FMARK_FORMAT, __FUNCTION__, __FILE_BASE_NAME__, __LINE__, f, ##__VA_ARGS__)
#endif

/**
 * @brief Définition d'une fonction de log
 * 
 * @param dlog descripteur du log
 * @param level niveau de log souhaité
 * @param prefix description du marqueur de sens de communication ou d'erreur
 * @param buf  description du buffer à dumper
 * @param len longueur du buffer
 * @param err errno associée
 */
typedef int(*log_dump_t)(int dlog, log_level_t level, char const *sens, int err, char const *buf, ssize_t len);

/**
 * @brief Définition d'une fonction de log
 * 
 * @param dlog descripteur du log
 * @param level niveau de log souhaité
 * @param prefix description du marqueur de sens de communication ou d'erreur
 * @param buf  description du buffer à dumper
 * @param len longueur du buffer
 * @param err errno associée
 */
typedef int(*log_varg_t)(int dlog, log_level_t level, char const *prefix, int err, char const *format, va_list ap);

char const *_log_level_to_str(void);
/**@}*/
#endif //LOG_LOG_H
