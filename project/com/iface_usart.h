#pragma once

#include "main.h"
#include "com_iface.h"

// Hardware USART.

/**
 * @brief Init an USART interface. Caller must configure GPIO's & AF manually.
 * @param USARTx  device
 * @param baud    baud rate (ex.: 9600)
 * @param rxbuf_len  receive buffer size, must be > 0
 * @param txbuf_len  send buffer size. If 0, tx is blocking.
 * @return the iface structure
 */
ComIface *usart_iface_init(USART_TypeDef* USARTx, uint32_t baud, size_t rxbuf_len, size_t txbuf_len);

///** Set baud rate */
//void usart_iface_set_baudrate(ComIface *iface, uint32_t baud);
