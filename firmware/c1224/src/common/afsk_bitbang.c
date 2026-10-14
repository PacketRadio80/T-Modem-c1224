/*
 * Timed NRZI bitbang to TCM3105 TXD/RXD — v1.0-prototype best effort.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "afsk_bitbang.h"
#include "hdlc.h"
#include "board_pins.h"
#include "ptt.h"
#include "status.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string.h>

#define BIT_US           (1000000u / (unsigned)C1224_BITRATE)
#define TX_BITS_MAX      ((HDLC_MAX_INFO + 4) * 10 + 64)
#define RX_IDLE_MARK     true

static uint8_t g_tx_bits[TX_BITS_MAX];
static volatile bool g_tx_busy;
static bool g_nrzi_level;
static hdlc_rx_t g_rx;
static bool g_rx_nrzi;
static absolute_time_t g_next_sample;
static afsk_rx_frame_fn g_rx_cb;
static uint8_t g_rx_out[HDLC_MAX_INFO];

void afsk_bitbang_init(void)
{
    gpio_put(PIN_MODEM_TXD, 1);
    g_tx_busy = false;
    g_nrzi_level = true;
    g_rx_nrzi = RX_IDLE_MARK;
    hdlc_rx_init(&g_rx);
    g_rx_cb = NULL;
    g_next_sample = get_absolute_time();
}

unsigned afsk_bitrate(void)
{
    return (unsigned)C1224_BITRATE;
}

bool afsk_tx_busy(void)
{
    return g_tx_busy;
}

void afsk_rx_set_callback(afsk_rx_frame_fn fn)
{
    g_rx_cb = fn;
}

static void nrzi_emit(bool data_bit)
{
    /* AX.25 NRZI: 0 = transition, 1 = no transition */
    if (!data_bit) {
        g_nrzi_level = !g_nrzi_level;
    }
    gpio_put(PIN_MODEM_TXD, g_nrzi_level ? 1 : 0);
}

bool afsk_tx_frame(const uint8_t *info, size_t info_len)
{
    if (g_tx_busy || !info || info_len == 0 || info_len > HDLC_MAX_INFO) {
        return false;
    }
    size_t nbits = hdlc_encode_bits(info, info_len, g_tx_bits, TX_BITS_MAX, 2);
    if (nbits == 0) {
        return false;
    }

    g_tx_busy = true;
    status_set(C1224_STATE_SENDING);
    status_publish();
    ptt_set(true);
    sleep_ms(40); /* key-up settle — prototype */

    g_nrzi_level = true;
    gpio_put(PIN_MODEM_TXD, 1);

    for (size_t i = 0; i < nbits; i++) {
        nrzi_emit(g_tx_bits[i] != 0);
        busy_wait_us_32(BIT_US);
    }

    gpio_put(PIN_MODEM_TXD, 1);
    sleep_ms(20);
    ptt_set(false);
    status_set(C1224_STATE_READY);
    status_publish();
    g_tx_busy = false;
    return true;
}

void afsk_rx_poll(void)
{
    if (g_tx_busy) {
        return;
    }
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(g_next_sample, now) < 0) {
        return;
    }
    g_next_sample = delayed_by_us(now, BIT_US);

    bool level = gpio_get(PIN_MODEM_RXD);
    bool data_bit = (level == g_rx_nrzi); /* 1 = no transition */
    if (level != g_rx_nrzi) {
        g_rx_nrzi = level;
    }

    size_t out_len = 0;
    if (hdlc_rx_bit(&g_rx, data_bit, g_rx_out, &out_len) && g_rx_cb && out_len > 0) {
        g_rx_cb(g_rx_out, out_len);
    }
}
