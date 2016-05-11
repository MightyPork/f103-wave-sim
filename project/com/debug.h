#pragma once

#include "main.h"
#include "com_iface.h"
#include "com_fileio.h"

#include "bus/event_queue.h"

// helper to mark printf functions
#define PRINTF_LIKE __attribute__((format(printf, 1, 2)))

#define DBG_BUF_LEN 256

#define ESCAPE_DEBUG_MESSAGES 1


// formatting symbols
#define DEBUG_EOL "\r\n"
#define DEBUG_TAG_WARN  "[W] "
#define DEBUG_TAG_ERROR "[E] "
#define DEBUG_TAG_BASE  "[ ] "
#define DEBUG_TAG_INFO  "[i] "


/** Print a log message with no tag and no newline */
void dbg_printf(const char *fmt, ...) PRINTF_LIKE;

/** Print via va_list */
void dbg_va_base(const char *fmt, const char *tag, va_list va);

/** Print a string to the debug interface (length not limited) */
static inline void dbg_raw(const char *str)
{
	com_tx_str(debug_iface, str);
}


/** Print a char to the debug interface */
static inline void dbg_raw_c(char c)
{
	com_tx(debug_iface, (uint8_t)c);
}


#if VERBOSE_LOGGING

/** Print a log message with a "debug" tag and newline */
void dbg(const char *fmt, ...) PRINTF_LIKE;


/** Print a log message with an "info" tag and newline */
void info(const char *fmt, ...) PRINTF_LIKE;

#else

#define dbg(fmt, ...)
#define info(fmt, ...)

#endif


/** Print a log message with an "info" tag and newline */
void banner_info(const char *fmt, ...) PRINTF_LIKE;

/** Print a log message with a "banner" tag and newline */
void banner(const char *fmt, ...) PRINTF_LIKE;

/** Print a log message with a "warning" tag and newline */
void warn(const char *fmt, ...) PRINTF_LIKE;


/** Print a log message with an "error" tag and newline */
void error(const char *fmt, ...) PRINTF_LIKE;
