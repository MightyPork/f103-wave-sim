#include "datalink.h"
#include "com_fileio.h"
#include "debug.h"
#include "com_fileio.h"

SBMP_Endpoint *dlnk_ep;

static void dlnk_tx(uint8_t b);
static void dlnk_rx_bridge(ComIface *iface);

/** Set up the datalink */
void dlnk_init(void)
{
	// Allocate & setup the endpoint
	dlnk_ep = sbmp_ep_init(NULL, NULL, 100, dlnk_rx, dlnk_tx);
	sbmp_ep_seed_session(dlnk_ep, 0x3FFF); // just in case arbitration fails
	sbmp_ep_enable(dlnk_ep, true);

	sbmp_ep_init_listeners(dlnk_ep, NULL, 4); // really don' need many here..

	data_iface->rx_callback = dlnk_rx_bridge;
}

/**
 * Bridge between USART subsystem's Rx buffer, and the SBMP driver
 *
 * Called if bytes are received in the USART buffer,
 * and the USART subsystem is polled.
 */
static void dlnk_rx_bridge(ComIface *iface)
{
	uint8_t b;
	while (com_rx(iface, &b)) {
		SBMP_RxStatus st = sbmp_ep_receive(dlnk_ep, b);

		// If byte was not accepted, try to put it back into the buffer
		if (st == SBMP_RX_BUSY || st == SBMP_RX_DISABLED) {
			com_unrx(iface, b);
			break;
		}
	}
}

/** Datalink Tx func */
static void dlnk_tx(uint8_t b)
{
	com_tx(data_iface, b);
}
