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

/** Continuous AFSK test tone (bench "Tone-Pump"): hold TXD level + PTT.
 *  high_tone=true  -> space level (TXD=0) ~2200 Hz (the high tone).
 *  high_tone=false -> mark  level (TXD=1) ~1200 Hz (the low/idle tone).
 *  on=false releases the tone and PTT. Non-blocking; main loop feeds the
 *  watchdog while the tone is held. */
void afsk_tx_tone(bool on, bool high_tone);
bool afsk_tx_tone_active(void);

/** Poll RX sampler; call often from main loop. May emit via callback. */
typedef void (*afsk_rx_frame_fn)(const uint8_t *info, size_t len);
void afsk_rx_set_callback(afsk_rx_frame_fn fn);
void afsk_rx_poll(void);

bool afsk_tx_busy(void);
unsigned afsk_bitrate(void);

#endif /* C1224_AFSK_BITBANG_H */
