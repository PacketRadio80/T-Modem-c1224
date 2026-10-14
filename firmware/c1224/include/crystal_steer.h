#ifndef C1224_CRYSTAL_STEER_H
#define C1224_CRYSTAL_STEER_H

#include <stdbool.h>

/** v1.0 no-op — rate = stuffed crystal; no XTAL mux. */
void crystal_steer_apply(void);
bool crystal_steer_applied(void);

#endif
