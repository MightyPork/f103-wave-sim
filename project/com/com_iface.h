#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

// Implementation-independent UI (command handler)

typedef struct ComIface_struct ComIface;

struct ComIface_struct {
	// --- Variables ---

	/** Implementation-specific data object */
	void *opts;

	/** File descriptor for this stream */
	FILE *file;

	/**
	 * User callback for data ready, triggered on poll().
	 * Can be null.
	 */
	void (*rx_callback)(ComIface *iface);

	// --- Interface methods ---

	/** Data ready to be read (RX buffer Not Empty) */
	bool (*rx_rdy)(ComIface *iface);

	/** Free space in TX buffer */
	bool (*tx_rdy)(ComIface *iface);

	/** Everything sent */
	bool (*tx_done)(ComIface *iface);

	/** Send 1 byte. Blocking, timeout after some time. */
	bool (*tx)(ComIface *iface, uint8_t b);

	/** Read one byte. False on failure. Blocking, timeout after some time. */
	bool (*rx)(ComIface *iface, uint8_t *b);

	/** Unreceive one byte. False on failure. */
	bool (*unrx)(ComIface *iface, uint8_t b);

	/** Send a binary block. False on failure. */
	size_t (*txb)(ComIface *iface, const void *blob, size_t size);

	/** Read a binary block. Returns real read length.  Blocking, timeout after some time. */
	size_t (*rxb)(ComIface *iface, void *buf, size_t length);

	/** Poll for changes */
	void (*poll)(ComIface *iface);
};


// ---- Functions for working with iface --------------

/** Wait for incoming byte */
bool com_rx_wait(ComIface *iface, uint16_t timeout);

/** Wait for free space in tx buffer */
bool com_tx_rdy_wait(ComIface *iface, uint16_t timeout);

/** Wait for tx complete */
bool com_tx_done_wait(ComIface *iface, uint16_t timeout);

/** Check if there's data in the receive buffer */
bool com_rx_rdy(ComIface *iface);

/** Check if transmit buffer can accept data */
bool com_tx_rdy(ComIface *iface);

/** Check if transmit buffer is empty */
bool com_tx_done(ComIface *iface);

/** Send 1 byte */
bool com_tx(ComIface *iface, uint8_t b);

/** Read one byte. False on failure. Fails on timeout. */
bool com_rx(ComIface *iface, uint8_t *b);

/** Unrx one byte. False on failure. */
bool com_unrx(ComIface *iface, uint8_t b);

/** Send a binary blob. False on failure. Fails on timeout. */
size_t com_tx_block(ComIface *iface, const void *blob, size_t size);

/** Read a blob. Returns real read length. Stops on timeout. */
size_t com_rx_block(ComIface *iface, void *buf, size_t length);

/** Poll for changes */
void com_poll(ComIface *iface);

/** Send 1 char */
bool com_tx_char(ComIface *iface, char c);

/** Read one char. False on failure. Fails on timeout. */
bool com_rx_char(ComIface *iface, char *c);

/** Send a string. False on failure. */
size_t com_tx_str(ComIface *iface, const char *str);

/** Read a string. Returns real read length. Stops on timeout. */
size_t com_rx_str(ComIface *iface, char *buf, size_t length);


/**
 * Printf to a serial interface.
 * Max length is 255 chars.
 */
void com_printf(ComIface *iface, const char *fmt, ...)
	__attribute__ ((format (printf, 2, 3)));


/**
 * Printf to a serial interface, with va_list.
 */
void com_vprintf(ComIface *iface, const char *fmt, va_list va);


// ---- VT100/ANSI color support -------------

/** ANSI formatting attributes */
typedef enum {
	// Non-colour Attributes
	FMT_RESET = 0, // Reset all attributes
	FMT_BRIGHT = 1, // Bright
	FMT_DIM = 2, // Dim
	FMT_UNDER = 4, // Underscore
	FMT_BLINK = 5, // Blink
	FMT_INVERS = 7, // Reverse
	FMT_HIDDEN = 8, // Hidden
	FMT_ITALIC = 16, // Italic font
	FMT_FAINT  = 32, // Faint color

	// Foreground Colours
	FMT_BLACK = 30, // Black
	FMT_RED = 31, // Red
	FMT_GREEN = 32, // Green
	FMT_YELLOW = 33, // Yellow
	FMT_BLUE = 34, // Blue
	FMT_MAGENTA = 35, // Magenta
	FMT_CYAN = 36, // Cyan
	FMT_WHITE = 37, // White

	// Background Colours
	FMT_BLACK_BG = 40, // Black
	FMT_RED_BG = 41, // Red
	FMT_GREEN_BG = 42, // Green
	FMT_YELLOW_BG = 43, // Yellow
	FMT_BLUE_BG = 44, // Blue
	FMT_MAGENTA_BG = 45, // Magenta
	FMT_CYAN_BG = 46, // Cyan
	FMT_WHITE_BG = 47, // White
} ANSI_attr_t;

#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5,4,3,2,1)
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,N,...) N

#define com_v100_attr(iface, ...) com_v100_attr_(iface, VA_NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

/**
 * Send formatting code to a com interface
 */
void com_v100_attr_(ComIface *iface, uint8_t count, ...);
