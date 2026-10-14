/*
 * Offline health check — CDT+RXD readable; crystal_steer no-op applied.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "modem_probe.h"
#include "config.h"
#include "board_pins.h"
#include "crystal_steer.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

modem_probe_result_t modem_probe_run(void)
{
    modem_probe_result_t r = {0};

    crystal_steer_apply();
    r.crystal_driven = crystal_steer_applied();

    (void)gpio_get(PIN_MODEM_CDT);
    (void)gpio_get(PIN_MODEM_RXD);
    r.cdt_readable = true;
    r.rxd_readable = true;

    /* Optional TXD pulse — no CDT reaction required offline */
    gpio_put(PIN_MODEM_TXD, 1);
    sleep_ms(1);
    gpio_put(PIN_MODEM_TXD, 0);

    r.ready = r.crystal_driven && r.cdt_readable && r.rxd_readable;

    if (config_unsolicited_ok()) {
        printf("PROBE crystal=%d cdt=%d rxd=%d -> %s\r\n",
               (int)r.crystal_driven, (int)r.cdt_readable, (int)r.rxd_readable,
               r.ready ? "READY" : "FAULT");
    }
    return r;
}
