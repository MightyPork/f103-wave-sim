#include "main.h"
#include "malloc_safe.h"
#include "iface_usart.h"

#include "utils/timebase.h"
#include "utils/circbuf.h"

#include "com/com_fileio.h"
#include "com/debug.h"

#include "bus/event_queue.h"


#define DEF_WAIT_TO_MS 5


typedef struct  {
	uint16_t wait_timeout;
	USART_TypeDef *dev;
	CircBuf *rxbuf; // allocated receive buffer
	CircBuf *txbuf; // allocated transmit buffer, can be NULL -> no buffer
} usart_opts;

#define opts(iface) ((usart_opts *)(iface)->opts)

// ---- Instances ----
// (needed for async rx/tx with interrupts)

static ComIface *usart1_iface = NULL;
static ComIface *usart2_iface = NULL;
static ComIface *usart3_iface = NULL;

// -------------------


/** Check if data is ready for reading */
static bool if_rx_rdy(ComIface *iface)
{
	CircBuf *buf = opts(iface)->rxbuf;

	return !cbuf_empty(buf);
}


/** Check if sending is done */
static bool if_tx_rdy(ComIface *iface)
{
	CircBuf *buf = opts(iface)->txbuf;

	if (buf == NULL) {
		// non-buffered mode
		USART_TypeDef* USARTx = opts(iface)->dev;
		return (USARTx->SR & USART_SR_TXE);
	}

	return !cbuf_full(buf);
}


/** Check if sending is done */
static bool if_tx_done(ComIface *iface)
{
	CircBuf *buf = opts(iface)->txbuf;
	USART_TypeDef* USARTx = opts(iface)->dev;

	// NULL buffer is considered empty
	return cbuf_empty(buf) && (USARTx->SR & USART_SR_TC);
}


/** Read one byte (wait for it). False on error. */
static bool if_rx(ComIface *iface, uint8_t *b)
{
	// wait for data in the buffer
	if (!com_rx_wait(iface, opts(iface)->wait_timeout) || b == NULL) {
		return false;
	}

	// read from buffer
	cbuf_pop(opts(iface)->rxbuf, b);

	return true;
}


/** Try to unreceive a byte. False on error. */
static bool if_unrx(ComIface *iface, uint8_t b)
{
	// push back
	return cbuf_push(opts(iface)->rxbuf, &b);
}


/** Send one byte (waits for tx) */
static bool if_tx(ComIface *iface, uint8_t b)
{
	usart_opts *uopts = opts(iface);
	USART_TypeDef* USARTx = uopts->dev;

	if (uopts->txbuf == NULL) {

		// Blocking tx mode
		until_timeout(uopts->wait_timeout) {
			if (USARTx->SR & USART_SR_TXE) {
				USARTx->DR = b;
				return true; // success
			}
		}

		return false;

	} else {

		// Buffered mode

		// wait for free space in the buffer
		bool ready = com_tx_rdy_wait(iface, uopts->wait_timeout);

		if (ready) {
			// append to the buffer
			cbuf_append(uopts->txbuf, &b);
		}

		// regardless ready state, start Tx if there are bytes
		// (should fix a bug where full buffer was never emptied)

		// If TXE (send buffer empty), start sending the buffer
		// Otherwise, IRQ chain is in progress.
		if (USARTx->SR & USART_SR_TXE) {
			USART_ITConfig(USARTx, USART_IT_TXE, ENABLE); // start tx chain
		}

		return ready;
	}
}


/** Read a blob. Returns real read size */
static size_t if_rxb(ComIface *iface, void *buf, size_t buflen)
{
	if (buf == NULL) return false;
	//if (!com_rx_wait(iface, opts(iface)->wait_timeout)) return 0;

	uint8_t* byteptr = (uint8_t*)buf;
	for (size_t i = 0; i < buflen; i++) {
		if (!if_rx(iface, byteptr++)) return i;
	}

	return buflen;
}


/** Send a binary blob. False on error. */
static size_t if_txb(ComIface *iface, const void *blob, size_t size)
{
	if (blob == NULL) return false;
	//if (!com_tx_rdy_wait(iface, opts(iface)->wait_timeout)) return false;

	const uint8_t* byteptr = (const uint8_t*)blob;
	for (size_t i = 0; i < size; i++) {
		if (!if_tx(iface, *byteptr++)) return i;
	}

	return size;
}


/** Check for incoming data */
static void if_poll(ComIface *iface)
{
	if (if_rx_rdy(iface)) {
		// call user cb
		if (iface->rx_callback) {
			iface->rx_callback(iface);
		}
	}
}


// ---- Init interface ----

//void usart_iface_set_baudrate(ComIface *iface, uint32_t baud)
//{
//	USART_TypeDef* USARTx = opts(iface)->dev;
//
//	USART_SetBaudrate(USARTx, baud);
//}


/* public */
ComIface *usart_iface_init(USART_TypeDef* USARTx, uint32_t baud, size_t rxbuf_len, size_t txbuf_len)
{
	assert_param(IS_USART_BAUDRATE(baud));
	assert_param(rxbuf_len > 0);

	ComIface *iface = malloc_s(sizeof(ComIface));

	// --- setup device specific iface data ---

	// Allocate the opts config object
	iface->opts = malloc_s(sizeof(usart_opts));

	// Set device ID
	opts(iface)->dev = USARTx;
	opts(iface)->wait_timeout = DEF_WAIT_TO_MS;

	// Initialize the rx/tx buffers (malloc's the array)
	opts(iface)->rxbuf = cbuf_create(rxbuf_len, 1);

	// zero-length TX buffer -> blocking Tx
	opts(iface)->txbuf = (txbuf_len == 0) ? NULL : cbuf_create(txbuf_len, 1);

	// --- driver instance ---

	iface->rx_rdy = if_rx_rdy;
	iface->tx_rdy = if_tx_rdy;
	iface->tx_done = if_tx_done;

	iface->rx = if_rx;
	iface->unrx = if_unrx;
	iface->tx = if_tx;

	iface->txb = if_txb;
	iface->rxb = if_rxb;

	iface->poll = if_poll;

	iface->rx_callback = NULL; // user can replace


	/* enable peripehral clock, assign to holder var, enable IRQ */
	IRQn_Type irqn;

	if (USARTx == USART1) {
		// USART1
		usart1_iface = iface;
		irqn = USART1_IRQn;
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	} else if (USARTx == USART2) {
		// USART2
		usart2_iface = iface;
		irqn = USART2_IRQn;
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	} else {
		// USART3
		usart3_iface = iface;
		irqn = USART3_IRQn;
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	}

	// Enable IRQ
	NVIC_EnableIRQ(irqn);

	// lower priority than SysTick, but high
	NVIC_SetPriority(irqn, 1); // function does the shifting


	/* Setup UART parameters. */
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = baud;
	USART_Init(USARTx, &usart);

	/* Enable */
	USART_Cmd(USARTx, ENABLE);

	// Enable Rx interrupt
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE); // disable IRQ

	return iface;
}


// ---- IRQ handlers for chained writing and rx ----


/**
 * @brief Common USART irq handler
 * @param iface the interface at which the event occured
 */
static void usart_iface_irq_base(ComIface* iface)
{
	USART_TypeDef* USARTx = opts(iface)->dev;

	if (USARTx->SR & USART_SR_RXNE) {
		// Byte received.
		uint8_t rxb = USARTx->DR & 0xFF;
		if (!cbuf_append(opts(iface)->rxbuf, &rxb)) {
			// Buffer overflow
			// Can't print debug msg, can cause deadlock
		}
	}

	if (USARTx->SR & USART_SR_TXE) {
		// Byte sent.
		uint8_t txb;

		// Send next byte (if buffer is NULL, cbuf_pop also returns false)
		if (cbuf_pop(opts(iface)->txbuf, &txb)) {
			USARTx->DR = txb; // send data
		} else {
			USART_ITConfig(USARTx, USART_IT_TXE, DISABLE); // disable IRQ
		}
	}

	// Clear ORE flag if set
	USART_ClearFlag(USARTx, USART_FLAG_ORE);
}


void USART1_IRQHandler(void)
{
	usart_iface_irq_base(usart1_iface);
}


void USART2_IRQHandler(void)
{
	usart_iface_irq_base(usart2_iface);
}


void USART3_IRQHandler(void)
{
	usart_iface_irq_base(usart3_iface);
}
