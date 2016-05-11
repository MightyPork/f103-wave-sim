#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "com_iface.h"
#include "utils/timebase.h"

// ---- accessor methods ----------------------

bool com_rx_rdy(ComIface *iface)
{
	return iface->rx_rdy(iface);
}


bool com_tx_rdy(ComIface *iface)
{
	return iface->tx_rdy(iface);
}


bool com_tx_done(ComIface *iface)
{
	return iface->tx_done(iface);
}

bool com_tx(ComIface *iface, uint8_t b)
{
	return iface->tx(iface, b);
}


bool com_rx(ComIface *iface, uint8_t *b)
{
	return iface->rx(iface, (uint8_t*) b);
}


bool com_unrx(ComIface *iface, uint8_t b)
{
	if (!iface->unrx) {
		return false; // not all may have it implemented
	}
	return iface->unrx(iface, b);
}


size_t com_tx_block(ComIface *iface, const void *blob, size_t size)
{
	return iface->txb(iface, blob, size);
}


size_t com_rx_block(ComIface *iface, void *buf, size_t length)
{
	return iface->rxb(iface, buf, length);
}


void com_poll(ComIface *iface)
{
	iface->poll(iface);
}


bool com_rx_char(ComIface *iface, char * c)
{
	return iface->rx(iface, (uint8_t*) c);
}


bool com_tx_char(ComIface *iface, const char c)
{
	return iface->tx(iface, (uint8_t)c);
}


/** Read string, terminate with \0. Returns real read size. */
size_t com_rx_str(ComIface *iface, char* buf, size_t buflen)
{
	size_t len = iface->rxb(iface, buf, buflen);
	buf[len] = 0; // zero terminator
	return len;
}


size_t com_tx_str(ComIface *iface, const char * string)
{
	return iface->txb(iface, string, strlen(string));
}


/** Wait for incoming byte */
bool com_rx_wait(ComIface *iface, uint16_t timeout)
{
	until_timeout(timeout) {
		if (iface->rx_rdy(iface)) return true;
	}

	return false;
}


/** Wait for tx buf ready */
bool com_tx_rdy_wait(ComIface *iface, uint16_t timeout)
{
	until_timeout(timeout) {
		if (iface->tx_rdy(iface)) return true;
	}

	return false;
}


/** Wait for tx complete */
bool com_tx_done_wait(ComIface *iface, uint16_t timeout)
{
	until_timeout(timeout) {
		if (iface->tx_done(iface)) return true;
	}

	return false;
}


void com_printf(ComIface *iface, const char *fmt, ...)
{
	if (iface->file == NULL) {
		com_tx_str(iface, "com_printf(), no iface file!\r\n");
		return;
	}

	// use the file descriptor attached

	va_list va;
	va_start(va, fmt);
	vfprintf(iface->file, fmt, va);
	va_end(va);
}


void com_vprintf(ComIface *iface, const char *fmt, va_list va)
{
	if (iface->file == NULL) {
		com_tx_str(iface, "com_vprintf(), no iface file!\r\n");
		com_tx_str(iface, fmt); // poor mans fallback
		return;
	}

	// use the file descriptor attached
	vfprintf(iface->file, fmt, va);
}


void com_v100_attr_(ComIface *iface, uint8_t count, ...)
{
	va_list va;
	va_start(va, count);

	com_tx_char(iface, 27);
	com_tx_char(iface, '[');

	for (int i = 0; i < count; i++) {
		int attr = va_arg(va, int);

		// comma
		if (i > 0) com_tx_char(iface, ';');

		// number
		com_printf(iface, "%d", attr);
	}

	com_tx_char(iface, 'm');

	va_end(va);
}
