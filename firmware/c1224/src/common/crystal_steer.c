/*
 * Crystal policy — v1.0 single-XTAL: physical rate = stuffed crystal.
 * FW does not switch baud; dual UF2 obsolete; crystal_steer = no-op.
 * Y1 4.4336 → 1200 · Y2 6.5536 → 2400 · GP14/15 unused (mux abandoned).
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "crystal_steer.h"

static bool g_applied;

void crystal_steer_apply(void)
{
    /* No-op: board stuffing (not GPIO mux / not FW baud) selects the crystal. */
    g_applied = true;
}

bool crystal_steer_applied(void)
{
    return g_applied;
}
