/*
 * Informational label only — physical rate = stuffed crystal.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "baud_config.h"

static const baud_crystal_config_t cfg = {
    .label = "c1224",
};

const baud_crystal_config_t *baud_config_get(void)
{
    return &cfg;
}
