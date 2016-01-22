#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <stdarg.h>

enum {
	LOG_INFO		= 1,
	LOG_WARNING		= 2,
	LOG_ERROR		= 4,
	LOG_DEBUG		= 8
};

#ifdef __cplusplus
extern "C" {
#endif

/* Clear log outputs. Initially info/debug messages go to stdout, and
 * warning/error messages to stderr.
 */
void log_clear_targets(void);

/* set logging outputs for any combination of message types.
 * type_mask expects a bitmask.
 */
int log_add_stream(unsigned int type_mask, FILE *fp);
int log_add_file(unsigned int type_mask, const char *fname);
int log_add_func(unsigned int type_mask, void (*func)(const char*, void*), void *cls);

void log_msg(unsigned int type, const char *fmt, ...);
/* log_msg helpers */
void log_info(const char *fmt, ...);
void log_warning(const char *fmt, ...);
void log_error(const char *fmt, ...);
void log_debug(const char *fmt, ...);

void log_va_msg(unsigned int type, const char *fmt, va_list va);
/* log_va_msg helpers */
void log_va_info(const char *fmt, va_list ap);
void log_va_warning(const char *fmt, va_list ap);
void log_va_error(const char *fmt, va_list ap);
void log_va_debug(const char *fmt, va_list ap);

/* Intercept stdout/stderr and handle them through the logger. stdout as an
 * info log, and stderr as an error log. This only works on UNIX.
 */
void log_grab_stdout(void);
void log_grab_stderr(void);

#ifdef __cplusplus
}
#endif

#endif	/* LOGGER_H_ */
