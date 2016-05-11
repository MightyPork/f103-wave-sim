#include <stdlib.h>

#include "iface_noop.h"
#include "com_fileio.h"
#include "malloc_safe.h"

// ==== NOOP com driver ====

static bool if_rx_rdy(ComIface *iface)
{
	(void)iface;
	return false;
}

static bool if_tx_rdy(ComIface *iface)
{
	(void)iface;
	return true;
}

static bool if_tx_done(ComIface *iface)
{
	(void)iface;
	return true;
}

static bool if_rx(ComIface *iface, uint8_t *b)
{
	(void)iface;
	(void)b;
	return false;
}

static bool if_unrx(ComIface *iface, uint8_t b)
{
	(void)iface;
	(void)b;
	return false;
}

static bool if_tx(ComIface *iface, uint8_t b)
{
	(void)iface;
	(void)b;
	return true;
}

static size_t if_rxb(ComIface *iface, void *buf, size_t buflen)
{
	(void)iface;
	(void)buf;
	(void)buflen;
	return 0;
}

static size_t if_txb(ComIface *iface, const void *blob, size_t size)
{
	(void)iface;
	(void)blob;

	return size;
}

static void if_poll(ComIface *iface)
{
	(void)iface;
}

ComIface *com_noop_init(void)
{
	ComIface *iface = malloc_s(sizeof(ComIface));

	iface->rx_rdy = if_rx_rdy;
	iface->tx_rdy = if_tx_rdy;
	iface->tx_done = if_tx_done;

	iface->rx = if_rx;
	iface->unrx = if_unrx;
	iface->tx = if_tx;

	iface->rxb = if_rxb;
	iface->txb = if_txb;

	iface->poll = if_poll;
	iface->rx_callback = NULL;

	return iface;
}



