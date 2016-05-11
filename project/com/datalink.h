#pragma once

/**
 * SBMP setup & funcs
 */

#include "main.h"
#include <sbmp.h>

#define DG_REQUEST_RAW 40 // request raw vector. Sample count [u16], Frequency [u32]
#define DG_REQUEST_FFT 41 // request fft vector. Sample count [u16], Frequency [u32]. Result - count/2 bins. Count must be 2^n, 16..2048
#define DG_REQUEST_STORE_REF 42 // calculate signal signature & store for comparing
#define DG_REQUEST_COMPARE_REF 43
// wifi status & control
#define DG_SETMODE_AP 44 // request AP mode (AP button pressed)
#define DG_WPS_START 45 // start WPS
#define DG_WIFI_STATUS 46 // WiFi status report
#define DG_REQUEST_STM_VERSION 47 // Get acquisition module firmware version


extern SBMP_Endpoint *dlnk_ep;

void dlnk_init(void);

/** Received datagram handler */
extern void dlnk_rx(SBMP_Datagram *dg);
