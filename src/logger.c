#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#ifdef _MSC_VER
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include "logger.h"

#if defined(unix) || defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/select.h>
#endif

enum targ_type { TARG_STREAM, TARG_FILE, TARG_FUNC };

struct log_target {
	unsigned int msg_type;
	enum targ_type targ_type;
	FILE *fp;
	void (*func)(const char*, void*);
	void *func_cls;

	struct log_target *next;
};

static struct log_target *targ_list = (void*)1;		/* uninitialized */

static void init_once(void)
{
	if(targ_list == (void*)1) {
		/* perform one-time init */
		targ_list = 0;
		log_add_stream(LOG_INFO | LOG_DEBUG, stdout);
		log_add_stream(LOG_WARNING | LOG_ERROR, stderr);
	}
}

static void log_string(unsigned int type, const char *str);
static int typecolor(int type);

void log_clear_targets(void)
{
	init_once();

	while(targ_list) {
		struct log_target *tmp = targ_list;
		targ_list = targ_list->next;

		if(tmp->targ_type == TARG_FILE) {
			fclose(tmp->fp);
		}
		free(tmp);
	}
	targ_list = 0;
}

int log_add_stream(unsigned int type_mask, FILE *fp)
{
	struct log_target *targ;

	init_once();

	if(!(targ = malloc(sizeof *targ))) {
		perror("failed to allocate memory for log target");
		return -1;
	}
	targ->msg_type = type_mask;
	targ->targ_type = TARG_STREAM;
	targ->fp = fp;
	targ->next = targ_list;
	targ_list = targ;
	return 0;
}

int log_add_file(unsigned int type_mask, const char *fname)
{
	FILE *fp;
	struct log_target *targ;

	init_once();

	if(!(fp = fopen(fname, "w"))) {
		fprintf(stderr, "failed to open logfile: %s: %s\n", fname, strerror(errno));
		return -1;
	}
	if(type_mask & (LOG_ERROR | LOG_DEBUG)) {
		/* make error and debug logs unbuffered */
		setvbuf(fp, 0, _IONBF, 0);
	}
	if(!(targ = malloc(sizeof *targ))) {
		perror("failed to allocate memory for log target");
		fclose(fp);
		return -1;
	}
	targ->msg_type = type_mask;
	targ->targ_type = TARG_FILE;
	targ->fp = fp;
	targ->next = targ_list;
	targ_list = targ;
	return 0;
}

int log_add_func(unsigned int type_mask, void (*func)(const char*, void*), void *cls)
{
	struct log_target *targ;

	init_once();

	if(!(targ = malloc(sizeof *targ))) {
		perror("failed to allocate memory for log target");
		return -1;
	}
	targ->msg_type = type_mask;
	targ->targ_type = TARG_FUNC;
	targ->func = func;
	targ->func_cls = cls;
	targ->next = targ_list;
	targ_list = targ;
	return 0;
}


void log_va_msg(unsigned int type, const char *fmt, va_list ap)
{
	char fixedbuf[256];
	char *buf = fixedbuf;
	int len, sz = sizeof fixedbuf;

	init_once();

	if(!targ_list || !*fmt) return;	/* don't waste our time */

	/* try with the fixed size buffer first which should be sufficient most of the
	 * time. if this fails, allocate a buffer of the correct size.
	 */
	while((len = vsnprintf(buf, sz, fmt, ap)) >= sz || len < 0) {
		sz = len >= 0 ? len + 1 : sz * 2;
		if(buf != fixedbuf)
			free(buf);
		if(!(buf = malloc(sz))) {
			return;
		}
	}

	if(buf != fixedbuf)
		free(buf);

	log_string(type, buf);
}

/* helpers */
#define LOG_VA_MSG(t)	\
	do { \
		va_list ap; \
		va_start(ap, fmt); \
		log_va_msg(t, fmt, ap); \
		va_end(ap); \
	} while(0)

void log_msg(unsigned int type, const char *fmt, ...)
{
	LOG_VA_MSG(type);
}

void log_info(const char *fmt, ...)
{
	LOG_VA_MSG(LOG_INFO);
}

void log_warning(const char *fmt, ...)
{
	LOG_VA_MSG(LOG_WARNING);
}

void log_error(const char *fmt, ...)
{
	LOG_VA_MSG(LOG_ERROR);
}

void log_debug(const char *fmt, ...)
{
	LOG_VA_MSG(LOG_DEBUG);
}

void log_va_info(const char *fmt, va_list ap)
{
	log_va_msg(LOG_INFO, fmt, ap);
}

void log_va_warning(const char *fmt, va_list ap)
{
	log_va_msg(LOG_WARNING, fmt, ap);
}

void log_va_error(const char *fmt, va_list ap)
{
	log_va_msg(LOG_ERROR, fmt, ap);
}

void log_va_debug(const char *fmt, va_list ap)
{
	log_va_msg(LOG_DEBUG, fmt, ap);
}

static void log_string(unsigned int type, const char *str)
{
	struct log_target *targ;

	targ = targ_list;
	while(targ) {
		if(targ->msg_type & type) {
			if(targ->targ_type == TARG_STREAM || targ->targ_type == TARG_FILE) {
#if defined(unix) || defined(__unix__) || defined(__APPLE__)
				if(isatty(fileno(targ->fp)) && type != LOG_INFO) {
					int c = typecolor(type);

					int len = strlen(str);
					if(str[len - 1] == '\n') {
						char *s = alloca(len);
						memcpy(s, str, len);
						s[len - 1] = 0;
						fprintf(targ->fp, "\033[%dm%s\033[0m\n", c, s);
					} else {
						fprintf(targ->fp, "\033[%dm%s\033[0m", c, str);
					}
				} else
#endif
				{
					fputs(str, targ->fp);
				}

			} else if(targ->targ_type == TARG_FUNC) {
				targ->func(str, targ->func_cls);
			}
		}
		targ = targ->next;
	}
}

enum {
	BLACK = 0,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE
};

#define ANSI_FGCOLOR(x)	(30 + (x))
#define ANSI_BGCOLOR(x)	(40 + (x))

static int typecolor(int type)
{
	switch(type) {
	case LOG_ERROR:
		return ANSI_FGCOLOR(RED);
	case LOG_WARNING:
		return ANSI_FGCOLOR(YELLOW);
	case LOG_DEBUG:
		return ANSI_FGCOLOR(MAGENTA);
	default:
		break;
	}
	return 37;
}

#if defined(unix) || defined(__unix__) || defined(__APPLE__)
static int out_pipe[2], err_pipe[2];
static int thr_running;

static void *thr_func(void *arg);

static int std_intercept(void)
{
	pthread_t thr;

	if(thr_running) return 0;

	pipe(out_pipe);
	pipe(err_pipe);

	fcntl(out_pipe[0], F_SETFL, fcntl(out_pipe[0], F_GETFL) | O_NONBLOCK);
	fcntl(err_pipe[0], F_SETFL, fcntl(err_pipe[0], F_GETFL) | O_NONBLOCK);

	if(pthread_create(&thr, 0, thr_func, 0) == -1) {
		log_error("failed to start std intercept thread\n");
		return -1;
	}
	thr_running = 1;
	pthread_detach(thr);
	return 0;
}

static void *thr_func(void *arg)
{
	static char buf[512];
	static int bufsz = sizeof buf;
	int max_fd = out_pipe[0] > err_pipe[0] ? out_pipe[0] : err_pipe[0];

	for(;;) {
		int sz, res;
		fd_set rdset;

		FD_ZERO(&rdset);
		FD_SET(out_pipe[0], &rdset);
		FD_SET(err_pipe[0], &rdset);

		while((res = select(max_fd + 1, &rdset, 0, 0, 0)) == -1 && errno == EINTR);
		if(res == -1) {
			break;	/* assume EPIPE or non-recoverable error */
		}
		if(res == 0) continue;

		if(FD_ISSET(out_pipe[0], &rdset)) {
			while((sz = read(out_pipe[0], buf, bufsz - 1)) > 0) {
				buf[bufsz - 1] = 0;
				log_info("%s", buf);
			}
			if(sz == -1 && errno == EPIPE) break;
		}
		if(FD_ISSET(err_pipe[0], &rdset)) {
			while((sz = read(err_pipe[0], buf, bufsz - 1)) > 0) {
				buf[bufsz - 1] = 0;
				log_error("%s", buf);
			}
			if(sz == -1 && errno == EPIPE) break;
		}
	}
	return 0;
}

void log_grab_stdout(void)
{
	if(std_intercept() == -1) {
		return;
	}
	dup2(out_pipe[1], 1);
}

void log_grab_stderr(void)
{
	if(std_intercept() == -1) {
		return;
	}
	dup2(err_pipe[1], 2);
}
#else
void log_grab_stdout(void)
{
	log_error("log_grab_stdout only works on UNIX\n");
}

void log_grab_stderr(void)
{
	log_error("log_grab_stderr only works on UNIX\n");
}
#endif
