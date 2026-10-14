/*
 * Status from PTT + dual sense; texts from INI.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "status.h"
#include "config.h"
#include "display.h"
#include "ptt.h"
#include "sense.h"
#include <stdio.h>

static c1224_state_t g_state = C1224_STATE_STARTING;

const char *c1224_state_string(c1224_state_t st)
{
    const c1224_config_t *cfg = config_get();
    switch (st) {
    case C1224_STATE_STARTING:
        return cfg->starting;
    case C1224_STATE_READY:
        return cfg->ready;
    case C1224_STATE_FAULT:
        return cfg->fault;
    case C1224_STATE_SENDING:
        return cfg->sending;
    case C1224_STATE_SIGNAL:
        return cfg->signal;
    case C1224_STATE_PACKET:
        return cfg->packet;
    default:
        return cfg->fault;
    }
}

void status_set(c1224_state_t st)
{
    g_state = st;
}

c1224_state_t status_get(void)
{
    return g_state;
}

void status_publish(void)
{
    const char *s = c1224_state_string(g_state);
    if (config_unsolicited_ok()) {
        printf("STATUS %s\r\n", s);
    }
    display_update();
}

void status_tick(void)
{
    if (g_state == C1224_STATE_FAULT || g_state == C1224_STATE_STARTING) {
        display_update();
        return;
    }

    c1224_state_t next;
    if (ptt_get()) {
        next = C1224_STATE_SENDING;
    } else if (sense_packet_active()) {
        next = C1224_STATE_PACKET;
    } else if (sense_signal_active()) {
        next = C1224_STATE_SIGNAL;
    } else {
        next = C1224_STATE_READY;
    }

    if (next != g_state) {
        g_state = next;
        status_publish();
    } else {
        display_update();
    }
}
