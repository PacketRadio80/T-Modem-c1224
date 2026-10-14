/**
 * Bitbang / timed NRZI on TCM3105 TXD (GP6) and RXD (GP7).
 * Rate matches stuffed crystal — compile C1224_BITRATE=1200|2400.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef C1224_AFSK_BITBANG_H
#define C1224_AFSK_BITBANG_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef C1224_BITRATE
#define C1224_BITRATE 1200
#endif

void afsk_bitbang_init(void);

/** Transmit HDLC info payload (AX.25 frame without flags/FCS). Auto-PTT. */
bool afsk_tx_frame(const uint8_t *info, size_t info_len);

/** Poll RX sampler; call often from main loop. May emit via callback. */
typedef void (*afsk_rx_frame_fn)(const uint8_t *info, size_t len);
void afsk_rx_set_callback(afsk_rx_frame_fn fn);
void afsk_rx_poll(void);

bool afsk_tx_busy(void);
unsigned afsk_bitrate(void);

#endif /* C1224_AFSK_BITBANG_H */
