/*
 * Timed NRZI bitbang to TCM3105 TXD/RXD — v1.0-prototype best effort.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "afsk_bitbang.h"
#include "hdlc.h"
#include "board_pins.h"
#include "ptt.h"
#include "status.h"
#include "config.h"
#include "hardware/gpio.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include <string.h>

#define BIT_US           (1000000u / (unsigned)C1224_BITRATE)
#define TX_BITS_MAX      ((HDLC_MAX_INFO + 4) * 10 + 64)
#define RX_IDLE_MARK     true

static uint8_t g_tx_bits[TX_BITS_MAX];
static volatile bool g_tx_busy;
static volatile bool g_tone_active;
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
    g_tone_active = false;
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

/** sleep_ms that feeds the watchdog in 100 ms slices — tx_delay/txtail
 *  are operator-configurable up to 65 s, the watchdog resets at 8 s. */
static void sleep_ms_wd(uint32_t ms)
{
    while (ms > 0u) {
        uint32_t slice = ms > 100u ? 100u : ms;
        sleep_ms(slice);
        watchdog_update();
        ms -= slice;
    }
}

void afsk_tx_tone(bool on, bool high_tone)
{
    if (on) {
        if (g_tx_busy) {
            return;
        }
        g_tone_active = true;
        /* The TCM3105 turns a held TXD level into a continuous AFSK tone:
         * mark (TXD=1) is the idle ~1200 Hz, the other level (TXD=0) is
         * the higher space tone ~2200 Hz (Bell 202). Non-blocking so the
         * main loop keeps feeding the watchdog for the whole "Dauer" tone. */
        g_nrzi_level = !high_tone;
        gpio_put(PIN_MODEM_TXD, high_tone ? 0 : 1);
        status_set(C1224_STATE_SENDING);
        status_publish();
        ptt_set(true);
    } else {
        g_tone_active = false;
        gpio_put(PIN_MODEM_TXD, 1);
        g_nrzi_level = true;
        ptt_set(false);
        status_set(C1224_STATE_READY);
        status_publish();
    }
}

bool afsk_tx_tone_active(void)
{
    return g_tone_active;
}

bool afsk_tx_frame(const uint8_t *info, size_t info_len)
{
    if (g_tx_busy || g_tone_active || !info || info_len == 0 || info_len > HDLC_MAX_INFO) {
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
    /* KISS TXDELAY contract (AGENTS.md §4): PTT keys, radio settles,
     * then data. Value is set by KISS param 0x01 or SET tx_delay_ms. */
    sleep_ms_wd(config_get()->tx_delay_ms);

    g_nrzi_level = true;
    gpio_put(PIN_MODEM_TXD, 1);

    unsigned bits_since_wd = 0;
    for (size_t i = 0; i < nbits; i++) {
        nrzi_emit(g_tx_bits[i] != 0);
        busy_wait_us_32(BIT_US);
        if (++bits_since_wd >= 256u) {
            /* Bit-bang loop never sleeps — feed the 8 s watchdog. At
             * 300 Bd a max frame (HDLC_MAX_INFO=512) exceeds 17 s air
             * time; without this, the watchdog resets mid-TX (USB
             * re-enumeration, PTT drop). 256 bits = max 853 ms at 300 Bd. */
            watchdog_update();
            bits_since_wd = 0;
        }
    }

    gpio_put(PIN_MODEM_TXD, 1);
    /* KISS TXTAIL contract: keep PTT keyed after data before release. */
    sleep_ms_wd(config_get()->txtail_ms);
    ptt_set(false);
    status_set(C1224_STATE_READY);
    status_publish();
    g_tx_busy = false;
    return true;
}

void afsk_rx_poll(void)
{
    if (g_tx_busy || g_tone_active) {
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
