#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "com_iface.h"

#include "utils/timebase.h"

#include "debug.h"


void dbg_printf(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	com_vprintf(debug_iface, fmt, va); //	vsnprintf(strbuf, DBG_BUF_LEN, fmt, va);
	va_end(va);
}


void dbg_va_base(const char *fmt, const char *tag, va_list va)
{
	ms_time_t now = ms_now();
	uint32_t secs = now / 1000;
	uint32_t ms = now % 1000;

	com_printf(debug_iface, "%4"PRIu32".%03"PRIu32" ", secs, ms);

	dbg_raw(tag);

	com_vprintf(debug_iface, fmt, va);
	dbg_raw(DEBUG_EOL);
}

/** Print a log message with an INFO tag and newline (ONLY FOR BANNER - always shown) */
void banner_info(const char *fmt, ...)
{
	com_v100_attr(debug_iface, FMT_GREEN);

	va_list va;
	va_start(va, fmt);
	dbg_va_base(fmt, DEBUG_TAG_INFO, va);
	va_end(va);

	com_v100_attr(debug_iface, FMT_RESET);
}


#if VERBOSE_LOGGING

/** Print a log message with a DEBUG tag and newline */
void dbg(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	dbg_va_base(fmt, DEBUG_TAG_BASE, va);
	va_end(va);
}


/** Print a log message with an INFO tag and newline */
void info(const char *fmt, ...) __attribute__((alias("banner_info")));

#endif



/** Print a log message with an INFO tag and newline */
void banner(const char *fmt, ...)
{
	com_v100_attr(debug_iface, FMT_GREEN, FMT_BRIGHT);

	va_list va;
	va_start(va, fmt);
	dbg_va_base(fmt, DEBUG_TAG_INFO, va);
	va_end(va);

	com_v100_attr(debug_iface, FMT_RESET);
}


/** Print a log message with a warning tag and newline */
void warn(const char *fmt, ...)
{
	com_v100_attr(debug_iface, FMT_YELLOW, FMT_BRIGHT);

	va_list va;
	va_start(va, fmt);
	dbg_va_base(fmt, DEBUG_TAG_WARN, va);
	va_end(va);

	com_v100_attr(debug_iface, FMT_RESET);
}


/** Print a log message with an ERROR tag and newline */
void error(const char *fmt, ...)
{
	com_v100_attr(debug_iface, FMT_RED, FMT_BRIGHT);

	va_list va;
	va_start(va, fmt);
	dbg_va_base(fmt, DEBUG_TAG_ERROR, va);
	va_end(va);

	com_v100_attr(debug_iface, FMT_RESET);
}
