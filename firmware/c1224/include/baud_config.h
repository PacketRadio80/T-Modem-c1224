#ifndef C1224_BAUD_CONFIG_H
#define C1224_BAUD_CONFIG_H

/**
 * Informational only — physical rate = stuffed crystal (Y1/Y2).
 * One FW image; no baud switch; no dual UF2.
 */
typedef struct {
    const char *label; /* "crystal" — rate is board stuffing */
} baud_crystal_config_t;

const baud_crystal_config_t *baud_config_get(void);

#endif
