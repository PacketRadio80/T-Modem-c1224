#ifndef C1224_STATUS_H
#define C1224_STATUS_H

#include <stdbool.h>

typedef enum {
    C1224_STATE_STARTING = 0,
    C1224_STATE_READY,
    C1224_STATE_FAULT,
    C1224_STATE_SENDING,
    C1224_STATE_SIGNAL,   /* CDT active (Signal/RX) */
    C1224_STATE_PACKET    /* RXD packet sense (may coexist with signal) */
} c1224_state_t;

/** Operator-facing text for state (from INI). */
const char *c1224_state_string(c1224_state_t st);
void status_set(c1224_state_t st);
c1224_state_t status_get(void);
/** Recompute composite state from PTT + sense; publish if changed. */
void status_tick(void);
void status_publish(void);

#endif
